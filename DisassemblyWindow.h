#ifndef DISASSEMBLYWINDOW_H
#define DISASSEMBLYWINDOW_H

#include "EmulatorCore.h"

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class DisassemblyWindow;
}

class DisassemblyWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DisassemblyWindow(EmulatorCore &emu, QWidget *parent = 0);
    ~DisassemblyWindow();

private slots:
    void refresh_timer_timeout();

    void on_startAddrSpin_editingFinished();

    void on_endAddrSpin_editingFinished();

    void on_updateBtn_clicked();

    void on_updateFromPcBtn_clicked();

    void on_contBreakBtn_clicked();

    void on_singleStepBtn_clicked();

    void on_toggleBpBtn_clicked();

private:
    void update_table(uint16_t start, uint16_t end);

    void closeEvent(QCloseEvent *event);

private:
    /**
     * The period at which to check the emulator's state and refresh the GUI.
     */
    static constexpr int REFRESH_TIMEOUT = 250;

    /**
     * Contains all of the UI elements generated in the Qt Forms Designer.
     */
    Ui::DisassemblyWindow *_ui;

    /**
     * Timer used to periodically check if the emulator is paused.
     */
    QTimer _refresh_timer;

    /**
     * A reference to the currently running emulator.
     */
    EmulatorCore &_emu;
};

#endif // DISASSEMBLYWINDOW_H
