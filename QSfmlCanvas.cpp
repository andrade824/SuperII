/**
 * Handles drawing an SFML widget to the screen. This class is meant to be
 * overridden by a child class to perform application-specific drawing.
 */

#ifdef Q_WS_X11
    #include <Qt/qx11info_x11.h>
    #include <X11/Xlib.h>
#endif

#include "QSfmlCanvas.h"

QSFMLCanvas::QSFMLCanvas(QWidget* parent, const QSize& size, unsigned int frame_time) :
    QWidget(parent), m_initialized (false)
{
    // Setup some states to allow direct rendering into the widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // Set strong focus to enable keyboard events to be received
    setFocusPolicy(Qt::StrongFocus);

    // Setup the widget geometry
    resize(size);

    // Setup the timer
    m_timer.setInterval(frame_time);
}

void QSFMLCanvas::showEvent(QShowEvent*)
{
    if (!m_initialized)
    {
        // Under X11, we need to flush the commands sent to the server to ensure that
        // SFML will get an updated view of the windows
        #ifdef Q_WS_X11
            XFlush(QX11Info::display());
        #endif

        // Create the SFML window with the widget handle
        sf::RenderWindow::create((sf::WindowHandle) winId());

        // Let the derived class do its specific stuff
        OnInit();

        // Setup the timer to trigger a refresh at specified framerate
        connect(&m_timer, SIGNAL(timeout()), this, SLOT(repaint()));
        m_timer.start();

        m_initialized = true;
    }
}

QPaintEngine* QSFMLCanvas::paintEngine() const
{
    return 0;
}

void QSFMLCanvas::paintEvent(QPaintEvent*)
{
    clear();

    // Let the derived class do its specific stuff
    OnUpdate();

    // Display on screen
    display();
}

void QSFMLCanvas::resizeEvent(QResizeEvent *event)
{
    setView(sf::View(sf::FloatRect(0, 0, event->size().width(), event->size().height())));
}

QSFMLCanvas::~QSFMLCanvas()
{

}
