#ifndef VIEWMEMORYWINDOW_H
#define VIEWMEMORYWINDOW_H

#include "EmulatorCore.h"

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class ViewMemoryWindow;
}

class ViewMemoryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewMemoryWindow(EmulatorCore &emu, QWidget *parent = 0);

    ~ViewMemoryWindow();

private slots:
    void refresh_timer_timeout();

    void on_autoRefreshCheck_toggled(bool checked);

    void on_refreshSpin_editingFinished();

    void on_refreshBtn_clicked();

    void on_gotoBtn_clicked();

    void on_gotoSpin_editingFinished();

    void on_gotoPcBtn_clicked();

private:
    void update_table(uint16_t start, uint16_t end);

    void scroll_to_row(int row);

private:
    /**
     * The period (in milliseconds) at which update the memory contents.
     */
    static constexpr int REFRESH_TIMEOUT = 5000;

    /**
     * Contains all of the UI elements generated in the Qt Forms Designer.
     */
    Ui::ViewMemoryWindow *_ui;

    /**
     * Timer used to periodically check if the emulator is paused.
     */
    QTimer _refresh_timer;

    /**
     * A reference to the currently running emulator.
     */
    EmulatorCore &_emu;
};

#endif // VIEWMEMORYWINDOW_H
