#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "EmulatorCore.h"

#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(EmulatorCore &emu, QWidget *parent = 0);

    void SetStatusText(const QString &string);

    ~MainWindow();

private:
    void keyPressEvent(QKeyEvent *event);

private:
    /**
     * Contains all of the UI elements generated in the Qt Forms Designer.
     */
    Ui::MainWindow *_ui;

    /**
     * The text to display in the status bar.
     */
    QLabel *_status_text;

    /**
     * A reference to the currently running emulator.
     */
    EmulatorCore &_emu;
};

#endif // MAINWINDOW_H
