/**
 * Represents the Video generator module in the Apple II.
 */
#include "character_rom.h"
#include "Video.h"

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window.hpp>

/**
 * Constructor.
 *
 * @param mem A reference to main memory where the graphics data is stored.
 *            This memory is assumed to be 48K in size.
 * @param parent Parent of this widget.
 */
Video::Video(Memory &mem, QWidget*) :
    IMemoryMapped(VIDEO_START_ADDR, VIDEO_END_ADDR),
    _main_mem(mem),
    _use_graphics(false),
    _use_full_screen(true),
    _use_page1(true),
    _use_lo_res(true),
    _flash_timer(),
    _flash_invert(false),
    _initialized(false),
    _pixels(),
    _texture(),
    _sprite(_texture)
{
    /**
     * Setup some states to allow direct rendering into the widget.
     */
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    /**
     * This widget doesn't get focus (keyboard events are handled by the
     * MainWindow).
     */
    setFocusPolicy(Qt::NoFocus);

    /**
      * Setup the texture/sprite to draw the framebuffer on. The texture
      * contains the raw pixel array while the sprite is an SFML drawable
      * entity that gets rendered to the screen.
      */
    _texture.create(VIDEO_WIDTH, VIDEO_HEIGHT);
    _sprite.setTexture(_texture, true);
}

/**
 * Reset the video module to its default state.
 */
void Video::Reset()
{
    _use_graphics = false;
    _use_full_screen = true;
    _use_page1 = true;
    _use_lo_res = true;
}

/**
 * Get the text color.
 *
 * @return The color of the text.
 */
uint32_t Video::GetTextColor() const
{
    return _text_color;
}

/**
 * Set the text color.
 *
 * @param red The red component.
 * @param green The green component.
 * @param blue The blue component.
 */
void Video::SetTextColor(int red, int green, int blue)
{
    _text_color = (0xFF << 24) |
                  ((blue & 0xFF) << 16) |
                  ((green & 0xFF) << 8) |
                  (red & 0xFF);
}

/**
 * Toggle a soft-switch through a read operation.
 *
 * @param addr The address to read.
 * @param no_side_fx True if this read shouldn't cause any side effects
 *                   (used by the memory view and disassembly).
 *
 * @return Always returns zero.
 */
uint8_t Video::Read(uint16_t addr, bool no_side_fx)
{
    bool ret_val = false;

    if(!no_side_fx)
        toggle_switch(addr);

    switch(addr)
    {
        case 0xC050:
        case 0xC051: ret_val = _use_graphics; break;
        case 0xC052:
        case 0xC053: ret_val = _use_full_screen; break;
        case 0xC054:
        case 0xC055: ret_val = _use_page1; break;
        case 0xC056:
        case 0xC057: ret_val = _use_lo_res; break;
    }

    return (ret_val) ? 1 : 0;
}

/**
 * Toggle a soft-switch through a write operation.
 *
 * @param addr The address to write.
 */
void Video::Write(uint16_t addr, uint8_t)
{
    toggle_switch(addr);
}

/**
 * Save the Video state out to a file.
 *
 * @param output The file to write to.
 */
void Video::SaveState(std::ofstream &output)
{
    output.write(reinterpret_cast<char*>(&_use_graphics),
                 sizeof(_use_graphics));

    output.write(reinterpret_cast<char*>(&_use_full_screen),
                 sizeof(_use_full_screen));

    output.write(reinterpret_cast<char*>(&_use_page1), sizeof(_use_page1));

    output.write(reinterpret_cast<char*>(&_use_lo_res), sizeof(_use_lo_res));
}

/**
 * Load the Video state out of a file.
 *
 * @param input The file to read from.
 */
void Video::LoadState(std::ifstream &input)
{
    input.read(reinterpret_cast<char*>(&_use_graphics),
                 sizeof(_use_graphics));

    input.read(reinterpret_cast<char*>(&_use_full_screen),
                 sizeof(_use_full_screen));

    input.read(reinterpret_cast<char*>(&_use_page1), sizeof(_use_page1));

    input.read(reinterpret_cast<char*>(&_use_lo_res), sizeof(_use_lo_res));
}


/**
 * A showEvent is triggered just display a QWidget is displayed.
 */
void Video::showEvent(QShowEvent*)
{
    if (!_initialized)
    {
        /**
         * Create the SFML window with the widget handle and set it render at
         * the correct Apple II resolution (280 by 192).
         */
        sf::RenderWindow::create((sf::WindowHandle) winId());
        setView(sf::View(sf::FloatRect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT)));

        _initialized = true;
    }
}

/**
 * Returns null since SFML handles drawing to the screen.
 *
 * @return Null since no paint engine is implemented.
 */
QPaintEngine* Video::paintEngine() const
{
    return 0;
}

/**
 * Gets called every time the screen needs to be redrawn or whenever repaint()
 * is called.
 */
void Video::paintEvent(QPaintEvent*)
{
    clear();

    render();

    _texture.update(_pixels);

    draw(_sprite);

    display();
}

/**
 * Re-create the SFML RenderWindow every time the widget's size changes. This
 * ensures that SFML is drawing everything at the correct dimensions.
 *
 * Since the RenderWindow was just regenerated, it's also necessary to reset
 * the view to display the correct resolution (by default the view is the same
 * size as the window).
 */
void Video::resizeEvent(QResizeEvent*)
{
    sf::RenderWindow::create((sf::WindowHandle) winId());
    setView(sf::View(sf::FloatRect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT)));
}

/**
 * Redraw the screen.
 */
void Video::render()
{
    if(_use_graphics)
    {
        if(_use_lo_res)
            render_lores();
        else
            render_hires();

        if(!_use_full_screen)
            render_text();
    }
    else
    {
        render_text();
    }
}

/**
 * Render text to the screen.
 *
 * This method handles the differences between page 1/2 and full screen/mixed
 * screen modes.
 *
 * The video memory is mapped strangely. The first eight rows, second eight
 * rows, and third eight rows have different starting addresses that are 0x28
 * apart from each other (with the first starting address being the starting
 * address of the page).
 *
 * Each of the eight rows within each of those groups is then 0x80 apart from
 * each other.
 */
void Video::render_text()
{
    constexpr uint32_t PAGE1_START = 0x400;
    constexpr uint32_t PAGE2_START = 0x800;

    const uint16_t page_start = (_use_page1) ? PAGE1_START : PAGE2_START;

    /**
     * Handle flashing text characters.
     */
    if(_flash_timer.getElapsedTime().asMilliseconds() >= 250)
    {
        _flash_invert = !_flash_invert;
        _flash_timer.restart();
    }

    /**
     * In mixed graphics mode, text only appears on the bottom four lines, so
     * only display the top 20 rows if the Video is in text mode.
     */
    if(!_use_graphics)
    {
        for(int row = 0; row < 20; ++row)
        {
            const uint8_t group_offset = 0x28 * (row / 8);
            const uint16_t row_offset = ((row & 0x7) * 0x80);
            const uint16_t video_addr = page_start + group_offset + row_offset;

            for(int col = 0; col < 40; ++col)
                render_char(_main_mem.Read(video_addr + col), col, row);
        }
    }

    /**
     * Always display the bottom four rows regardless of display mode.
     */
    for(int row = 20; row < 24; ++row)
    {
        const uint16_t row_offset = ((row & 0x7) * 0x80);
        const uint16_t video_addr = page_start + 0x50 + row_offset;

        for(int col = 0; col < 40; ++col)
            render_char(_main_mem.Read(video_addr + col), col, row);
    }
}

/**
 * Render a single character to the screen.
 *
 * @param char_index The index of the character to draw in the character ROM.
 * @param x X-index of the character to draw.
 * @param y Y-index of the character to draw.
 */
void Video::render_char(uint8_t char_index, int x, int y)
{
    const bool normal_char = (char_index & 0x80) ? true : false;
    const bool invert_char = !normal_char && !(char_index & 0x40);
    const bool flash_char = !normal_char && !invert_char;

    const bool invert_colors = invert_char || (flash_char && _flash_invert);

    const int pixel_x = x * 7;
    const int pixel_y = y * 8;

    for(int row = 0; row < 8; ++row)
    {
        for(int col = 0; col < 7; ++col)
        {
            set_text_pixel(char_rom[char_index][row][col],
                           invert_colors,
                           pixel_x + (6 - col),
                           pixel_y + row);
        }
    }
}

/**
 * Sets a single pixel in the framebuffer for a text character.
 *
 * @param pixel The value from the character ROM for this pixel.
 * @param invert Whether to invert the colors for this pixel.
 * @param x X-position of the pixel.
 * @param y Y-position of the pixel.
 */
void Video::set_text_pixel(bool pixel, bool invert, int x, int y)
{
    constexpr uint32_t BG_COLOR = 0xFF000000;
    const uint32_t color = (pixel ^ invert) ? _text_color : BG_COLOR;

    *(uint32_t *)(_pixels + (y * VIDEO_WIDTH * 4) + (x * 4)) = color;
}

/**
 * Render a page of Lo-res graphics.
 */
void Video::render_lores()
{
    constexpr uint32_t PAGE1_START = 0x400;
    constexpr uint32_t PAGE2_START = 0x800;

    const uint16_t page_start = (_use_page1) ? PAGE1_START : PAGE2_START;

    /**
     * Always display the top 20 rows regardless of display mode.
     */
    for(int row = 0; row < 20; ++row)
    {
        const uint8_t group_offset = 0x28 * (row / 8);
        const uint16_t row_offset = ((row & 0x7) * 0x80);
        const uint16_t video_addr = page_start + group_offset + row_offset;

        for(int col = 0; col < 40; ++col)
            render_lores_block(_main_mem.Read(video_addr + col), col, row);
    }

    /**
     * In mixed screen mode, text appears on the bottom four rows, so don't
     * display the bottom four rows if the Video is in mixed screen mode.
     */
    if(_use_full_screen)
    {
        for(int row = 20; row < 24; ++row)
        {
            const uint16_t row_offset = ((row & 0x7) * 0x80);
            const uint16_t video_addr = page_start + 0x50 + row_offset;

            for(int col = 0; col < 40; ++col)
                render_lores_block(_main_mem.Read(video_addr + col), col, row);
        }
    }
}

/**
 * Draw a single pair of blocks (the blocks are vertically adjacent to each
 * other).
 *
 * The blocks are four pixels tall by seven pixels wide. Together, the blocks
 * make up one eight by seven block of pixels.
 *
 * The lower nybble of "block" describes the color for the upper block.
 * The upper nybble of "block" describes the color for the lower block.
 *
 * @param block The color values for the two blocks.
 * @param x X-position of the block.
 * @param y Y-position of the block.
 */
void Video::render_lores_block(uint8_t block, int x, int y)
{
    static constexpr uint32_t colors[16] = {
        0xFF000000, /* Black */
        0xFF601EE3, /* Red */
        0xFFCB2525, /* Dark Blue */
        0xFFFD44FF, /* Purple */
        0xFF60A300, /* Dark Green */
        0xFF9C9C9C, /* Gray */
        0xFFFDCF14, /* Medium Blue */
        0xFFFFC3D0, /* Light Blue */
        0xFF037260, /* Brown */
        0xFF3C6AFF, /* Orange */
        0xFF9C9C9C, /* Gray */
        0xFFD0A0FF, /* Pink */
        0xFF3CF514, /* Light Green */
        0xFF8DDDD0, /* Yellow */
        0xFFD0FF72, /* Aqua */
        0xFFFFFFFF  /* White */
    };

    const int pixel_x = x * 28;
    const int pixel_y = y * VIDEO_WIDTH * 32;

    for(int row = 0; row < 8; ++row)
    {
        const uint32_t color = (row < 4) ? colors[block & 0xF] :
                                           colors[(block & 0xF0) >> 4];

        for(int col = 0; col < 7; ++col)
        {
            const int row_offset = pixel_y + (row * VIDEO_WIDTH * 4);
            const int col_offset = pixel_x + (col * 4);
            *(uint32_t *)(_pixels + row_offset + col_offset) = color;
        }
    }
}

/**
 * Render a page of Hi-res graphics.
 */
void Video::render_hires()
{
    constexpr uint32_t PAGE1_START = 0x2000;
    constexpr uint32_t PAGE2_START = 0x4000;

    const uint16_t page_start = (_use_page1) ? PAGE1_START : PAGE2_START;

    /**
     * Always display the top 20 rows regardless of display mode.
     */
    for(int block = 0; block < 20; ++block)
    {
        const uint8_t group_offset = 0x28 * (block / 8);
        const uint16_t block_offset = ((block & 0x7) * 0x80);
        const uint16_t video_addr = page_start + group_offset + block_offset;

        for(int row = 0; row < 8; ++row)
            render_hires_row((block * 8) + row, video_addr + (row * 0x400));
    }

    /**
     * In mixed screen mode, text appears on the bottom four rows, so don't
     * display the bottom four rows if the Video is in mixed screen mode.
     */
    if(_use_full_screen)
    {
        for(int block = 20; block < 24; ++block)
        {
            const uint16_t block_offset = ((block & 0x7) * 0x80);
            const uint16_t video_addr = page_start + 0x50 + block_offset;

            for(int row = 0; row < 8; ++row)
                render_hires_row((block * 8) + row, video_addr + (row * 0x400));
        }
    }
}

/**
 * Render an entire row of hi-res pixels.
 *
 * @param row_num Index of the row to render.
 * @param row_addr Address where this row's contents are in memory.
 */
void Video::render_hires_row(int row_num, uint16_t row_addr)
{
    for(int col = 0; col < 40; ++col)
    {
        uint8_t prev_data = (col > 0) ? _main_mem.Read(row_addr + col - 1) : 0;
        uint8_t data = _main_mem.Read(row_addr + col);
        uint8_t next_data = (col < 39) ? _main_mem.Read(row_addr + col + 1) : 0;

        uint8_t color_group = (data & 0x80) >> 7;

        for(int pixel = 0; pixel < 7; ++pixel)
        {
            uint8_t adjacent_pixels = 0;

            if(pixel == 0)
            {
                adjacent_pixels |= (prev_data & 0x40) | ((data >> 1) & 1);
            }
            else if(pixel == 6)
            {
                adjacent_pixels |= (next_data & 1) | ((data >> 5) & 1);
            }
            else
            {
                adjacent_pixels |= ((data >> (pixel - 1)) & 1) |
                                   ((data >> (pixel + 1)) & 1);
            }

            render_hires_pixel(color_group,
                               (data >> pixel) & 1,
                               adjacent_pixels,
                               (col * 7) + pixel,
                               row_num);
        }
    }
}

/**
 * Render a single hi-res pixel.
 *
 * @param color_group '0' for Purple/Green, otherwise Blue/Orange
 * @param pixel The pixel to render.
 * @param adjacent_pixel '0' if no pixel next to this is set, any other value
 *                       otherwise.
 * @param x X-coordinate of the pixel.
 * @param y Y-coordinate of the pixel.
 */
void Video::render_hires_pixel(uint8_t color_group,
                        uint8_t pixel,
                        uint8_t adjacent_pixels,
                        int x,
                        int y)
{
    uint32_t color = 0;

    if(pixel == 0)
        color = 0xFF000000; /* Black */
    else if(pixel != 0 && adjacent_pixels != 0)
        color = 0xFFFFFFFF; /* White */
    else if(pixel != 0 && color_group == 0 && (x & 1) == 0)
        color = 0xFFFD44FF; /* Purple */
    else if(pixel != 0 && color_group == 0 && (x & 1) == 1)
        color = 0xFF3CF514; /* Green */
    else if(pixel != 0 && color_group == 1 && (x & 1) == 0)
        color = 0xFFFDCF14; /* Blue */
    else if(pixel != 0 && color_group == 1 && (x & 1) == 1)
        color = 0xFF3C6AFF; /* Orange */

    const int row_offset = y * VIDEO_WIDTH * 4;
    const int col_offset = x * 4;
    *(uint32_t *)(_pixels + row_offset + col_offset) = color;
}

/**
 * Toggles a soft switch.
 *
 * @param addr The address of the soft switch to toggle.
 */
void Video::toggle_switch(uint16_t addr)
{
    switch(addr)
    {
        case 0xC050: _use_graphics = true; break;
        case 0xC051: _use_graphics = false; break;
        case 0xC052: _use_full_screen = true; break;
        case 0xC053: _use_full_screen = false; break;
        case 0xC054: _use_page1 = true; break;
        case 0xC055: _use_page1 = false; break;
        case 0xC056: _use_lo_res = true; break;
        case 0xC057: _use_lo_res = false; break;
    }
}
