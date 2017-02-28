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

private:
    void keyPressEvent(QKeyEvent *event);

    void save_state(QString filename);
    void load_state(QString filename);

private:
    /**
     * The standard timeout for status bar messages.
     */
    static constexpr int STATUS_TEXT_TIMEOUT = 5000;

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
