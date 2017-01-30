/**
 * Represents the Video generator module in the Apple II.
 */
#include "Video.h"
#include "SFML/Window.hpp"

/**
 * Height and width of Apple II video resolution.
 */
static constexpr int APPLE_VIDEO_WIDTH = 280;
static constexpr int APPLE_VIDEO_HEIGHT = 192;

/**
 * Constructor.
 *
 * @param parent Parent of this widget.
 * @param size Size of this
 */
Video::Video(QWidget * parent) :
    QSFMLCanvas(parent, QSize(APPLE_VIDEO_WIDTH, APPLE_VIDEO_HEIGHT))
{ }

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
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        // Clear screen
        clear(sf::Color::Red);
    }
    else
        clear(sf::Color::Blue);
}
