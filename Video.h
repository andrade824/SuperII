#ifndef VIDEO_H
#define VIDEO_H

#include "IMemoryMapped.h"
#include "IState.h"
#include "Memory.h"

#include <cstdint>
#include <fstream>

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics.hpp>

#include <QPaintEngine>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QWidget>

class Video :
        public QWidget,
        public sf::RenderWindow,
        public IMemoryMapped,
        public IState
{
    Q_OBJECT

public:
    Video(Memory &mem, QWidget *parent = 0);

    void Reset();

    uint8_t Read(uint16_t addr) override;
    void Write(uint16_t addr, uint8_t) override;

    void SaveState(std::ofstream &output) override;
    void LoadState(std::ifstream &input) override;

private:
    virtual void showEvent(QShowEvent*);

    virtual QPaintEngine* paintEngine() const;

    virtual void paintEvent(QPaintEvent*);

    virtual void resizeEvent(QResizeEvent *);

    void render();

    void render_text();
    void render_char(uint8_t char_index, int x, int y);
    void set_text_pixel(bool pixel, bool invert, int x, int y);

    void render_lores();
    void render_lores_block(uint8_t block, int x, int y);

    void render_hires();
    void render_hires_row(int row_num, uint16_t row_addr);
    void render_hires_pixel(uint8_t color_group,
                            uint8_t pixel,
                            uint8_t adjacent_pixels,
                            int x,
                            int y);

    void toggle_switch(uint16_t addr);

private:
    /**
     * Height and width of Apple II video resolution.
     */
    static constexpr int VIDEO_WIDTH = 280;
    static constexpr int VIDEO_HEIGHT = 192;

    /**
     * Start and end addresses (inclusive) for the Video soft-switches.
     */
    static constexpr int VIDEO_START_ADDR = 0xC050;
    static constexpr int VIDEO_END_ADDR = 0xC057;

    /**
     * A reference to main memory where the graphics data is stored.
     */
    Memory &_main_mem;

    /**
     * Graphics/Text soft switch. True for graphics mode, false for text mode.
     */
    bool _use_graphics;

    /**
     * Full Screen/Mixed Screen soft switch. True for full screen, false for
     * mixed screen.
     */
    bool _use_full_screen;

    /**
     * Page 1/2 soft swich. True for page 1, false for page 2.
     */
    bool _use_page1;

    /**
     * Lo-res/Hi-res soft switch. True for lo-res graphics, false for hi-res.
     */
    bool _use_lo_res;

    /**
     * Timer used to determine when to flash characters (every 250ms).
     */
    sf::Clock _flash_timer;

    /**
     * True if "flashing" characters need to be inverted. This is set after
     * checking flash_timer.
     */
    bool _flash_invert;

    /**
     * True if the RenderWindow has been initialized already.
     */
    bool _initialized;

    /**
     * The pixels that will get drawn to the screen. Every four bytes represents
     * a pixel in the RGBA format.
     */
    uint8_t _pixels[VIDEO_HEIGHT * VIDEO_WIDTH * 4];

    /**
     * The texture that contains the pixels. This cannot be drawn by itself;
     * this texture needs to be wrapped into a sprite to be drawn.
     */
     sf::Texture _texture;

     /**
      * The drawable entity that will display the array of pixels.
      */
     sf::Sprite _sprite;
};

#endif // VIDEO_H
