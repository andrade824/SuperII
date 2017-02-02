#ifndef APPLEVIDEO_H
#define APPLEVIDEO_H

#include "QSfmlCanvas.h"

#include <cstdint>

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

class Video : public QSFMLCanvas
{
public:
    Video(QWidget * parent);

private:
    void OnInit();

    void OnUpdate();

private:
    /**
     * Height and width of Apple II video resolution.
     */
    static constexpr int VIDEO_WIDTH = 280;
    static constexpr int VIDEO_HEIGHT = 192;

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

#endif // APPLEVIDEO_H
