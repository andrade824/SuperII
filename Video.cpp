/**
 * Represents the Video generator module in the Apple II.
 */
#include "Video.h"

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Window.hpp"

/**
 * Constructor.
 *
 * @param parent Parent of this widget.
 * @param size Size of this
 */
Video::Video(QWidget * parent) :
    QSFMLCanvas(parent, QSize(VIDEO_WIDTH, VIDEO_HEIGHT)),
    _pixels(),
    _texture(),
    _sprite(_texture)
{
    _texture.create(VIDEO_WIDTH, VIDEO_HEIGHT);
}

/**
 * Operations to be performed before the screen starts drawing.
 */
void Video::OnInit()
{

}

/**
 * Redraw the screen.
 */
void Video::OnUpdate()
{
    for(int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT * 4; i += 4)
    {
        _pixels[i] = 0xFF;
        _pixels[i + 1] = 0xFF;
        _pixels[i + 2] = 0x00;
        _pixels[i + 3] = 0xFF;
    }

    _texture.update(_pixels);

    sf::Sprite sprite(_texture);
    draw(sprite);
}
