#ifndef VIDEO_H
#define VIDEO_H

#include "IMemoryMapped.h"
#include "Memory.h"

#include <cstdint>

#include <SFML/Graphics.hpp>

#include <QPaintEngine>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QWidget>

class Video : public QWidget, public sf::RenderWindow, public IMemoryMapped
{
    Q_OBJECT

public:
    Video(Memory &mem, QWidget *parent = 0);

private:
    void OnUpdate();

    virtual QPaintEngine* paintEngine() const;

    virtual void showEvent(QShowEvent*);

    virtual void paintEvent(QPaintEvent*);

    virtual void resizeEvent(QResizeEvent *);

    uint8_t Read(uint16_t addr);
    void Write(uint16_t addr, uint8_t);

private:
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
