/**
 * Represents the Video generator module in the Apple II.
 */
#include "character_rom.h"
#include "Video.h"

#ifdef Q_WS_X11
    #include <Qt/qx11info_x11.h>
    #include <X11/Xlib.h>
#endif

#include <SFML/Graphics.hpp>
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

    OnUpdate();

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
void Video::OnUpdate()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        for(int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT * 4; i += 4)
        {
            _pixels[i] = 0x66;
            _pixels[i + 1] = 0xFF;
            _pixels[i + 2] = 0x00;
            _pixels[i + 3] = 0xFF;
        }
    }
    else
    {
        for(int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT * 4; i += 4)
        {
            _pixels[i] = 0x66;
            _pixels[i + 1] = 0x20;
            _pixels[i + 2] = 0x82;
            _pixels[i + 3] = 0xFF;
        }
    }
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
