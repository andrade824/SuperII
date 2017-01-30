#ifndef APPLEVIDEO_H
#define APPLEVIDEO_H

#include "QSfmlCanvas.h"

class Video : public QSFMLCanvas
{
public:
    Video(QWidget * parent);

private:
    void OnInit();

    void OnUpdate();
};

#endif // APPLEVIDEO_H
