#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DiskController.h"
#include "EmulatorCore.h"

#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QRadioButton>
#include <QTimer>

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

private slots:
    void on_actionReset_triggered();

    void on_actionPower_Cycle_triggered();

    void on_actionQuick_Save_triggered();

    void on_actionQuick_Load_triggered();

    void on_actionSave_State_triggered();

    void on_actionLoad_State_triggered();

    void on_actionSettings_triggered();

    void on_actionView_Memory_triggered();

    void on_actionDisassembly_triggered();

    void on_actionCPU_Registers_triggered();

    void on_actionDrive_0_triggered();
    
    void on_actionDrive_1_triggered();

    void disk_busy_timeout();
    
    void on_actionSpeed_Up_triggered();

    void on_actionSpeed_Down_triggered();

private:
    void keyPressEvent(QKeyEvent *event);

    void save_state(QString filename);
    void load_state(QString filename);

    void load_disk(DiskController::DriveId drive);

private:
    /**
     * The standard timeout for status bar messages (in ms).
     */
    static constexpr int STATUS_TEXT_TIMEOUT = 5000;

    /**
     * How long to wait between checking to see if the disk is busy (in ms).
     */
     static constexpr int DISK_BUSY_TIMEOUT = 100;

    /**
     * Contains all of the UI elements generated in the Qt Forms Designer.
     */
    Ui::MainWindow *_ui;

    /**
     * Label used to show what the current CPU turbo is.
     */
    QLabel *_turbo_text;

    /**
     * The text to display in the status bar.
     */
    QLabel *_status_text;

    /**
     * Radio button used as an indicator of whether the disk is busy or not.
     */
    QRadioButton *_disk_busy;

    /**
     * Used to periodically check to see if the disk is busy.
     */
    QTimer *_check_disk_busy;

    /**
     * A reference to the currently running emulator.
     */
    EmulatorCore &_emu;
};

#endif // MAINWINDOW_H
