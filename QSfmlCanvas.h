#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <SFML/Graphics.hpp>

#include <QResizeEvent>
#include <QTimer>
#include <QWidget>

class QSFMLCanvas : public QWidget, public sf::RenderWindow
{
    Q_OBJECT

public :

    QSFMLCanvas(QWidget * parent, const QSize & size, unsigned int frame_time = 0);

    virtual ~QSFMLCanvas();

private :

    virtual void OnInit() = 0;

    virtual void OnUpdate() = 0;

    virtual QPaintEngine* paintEngine() const;

    virtual void showEvent(QShowEvent*);

    virtual void paintEvent(QPaintEvent*);

    virtual void resizeEvent(QResizeEvent *event);

private:
    QTimer m_timer;
    bool   m_initialized;
};

#endif // QSFMLCANVAS_H
