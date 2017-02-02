#ifndef VIDEO_H
#define VIDEO_H

#include <cstdint>

#include <SFML/Graphics.hpp>

#include <QPaintEngine>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QWidget>

class Video : public QWidget, public sf::RenderWindow
{
    Q_OBJECT

public:
    Video(QWidget *parent = 0);

private:
    void OnUpdate();

    virtual QPaintEngine* paintEngine() const;

    virtual void showEvent(QShowEvent*);

    virtual void paintEvent(QPaintEvent*);

    virtual void resizeEvent(QResizeEvent *);

private:
    /**
     * Height and width of Apple II video resolution.
     */
    static constexpr int VIDEO_WIDTH = 280;
    static constexpr int VIDEO_HEIGHT = 192;

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
