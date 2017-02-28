#ifndef CPUREGISTERSWINDOW_H
#define CPUREGISTERSWINDOW_H

#include "EmulatorCore.h"

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class CpuRegistersWindow;
}

class CpuRegistersWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CpuRegistersWindow(EmulatorCore &emu, QWidget *parent = 0);

    ~CpuRegistersWindow();

private slots:
    void refresh_timer_timeout();

private:
    /**
     * The period at which to check the emulator's state and refresh the GUI.
     */
    static constexpr int REFRESH_TIMEOUT = 250;

    /**
     * Contains all of the UI elements generated in the Qt Forms Designer.
     */
    Ui::CpuRegistersWindow *_ui;

    /**
     * Timer used to periodically check if the emulator is paused.
     */
    QTimer _refresh_timer;

    /**
     * A reference to the currently running emulator.
     */
    EmulatorCore &_emu;
};

#endif // CPUREGISTERSWINDOW_H
