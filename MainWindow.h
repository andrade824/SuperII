#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Video.h"

#include <QLabel>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Video *emulator_video, QWidget *parent = 0);

    void SetStatusText(const QString &string);

    ~MainWindow();

private:
    Ui::MainWindow *_ui;
    QLabel *_status_text;
};

#endif // MAINWINDOW_H
