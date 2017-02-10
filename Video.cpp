/**
 * Represents the Video generator module in the Apple II.
 */
#include "character_rom.h"
#include "Video.h"

#ifdef Q_WS_X11
    #include <Qt/qx11info_x11.h>
    #include <X11/Xlib.h>
#endif

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
     * This widget only gets focus if it's clicked.
     */
    setFocusPolicy(Qt::ClickFocus);

    /**
      * Setup the texture/sprite to draw the framebuffer on. The texture
      * contains the raw pixel array while the sprite is an SFML drawable
      * entity that gets rendered to the screen.
      */
    _texture.create(VIDEO_WIDTH, VIDEO_HEIGHT);
    _sprite.setTexture(_texture, true);
}

/**
 * A showEvent is triggered just display a QWidget is displayed.
 */
void Video::showEvent(QShowEvent*)
{
    if (!_initialized)
    {
        /**
         * Under X11, we need to flush the commands sent to the server to
         * ensure that SFML will get an updated view of the window.
         */
        #ifdef Q_WS_X11
            XFlush(QX11Info::display());
        #endif

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
    if(!_use_graphics)
        render_text();
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
     * In mixed screen mode, text only appears on the bottom four lines, so only
     * display the top 20 rows if the Video is in full screen mode.
     */
    if(_use_full_screen)
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
    constexpr uint32_t FG_COLOR = 0xFF60A300;
    constexpr uint32_t BG_COLOR = 0xFF000000;
    const uint32_t color = (pixel ^ invert) ? FG_COLOR : BG_COLOR;

    *(uint32_t *)(_pixels + (y * VIDEO_WIDTH * 4) + (x * 4) + 0) = color;
}

/**
 * Toggle a soft-switch through a read operation.
 *
 * @param addr The address to read.
 *
 * @return Always returns zero.
 */
uint8_t Video::Read(uint16_t addr)
{
    toggle_switch(addr);

    return 0;
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
