#include "DisassemblyWindow.h"
#include "EmulatorCore.h"
#include "MainWindow.h"
#include "SettingsDialog.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QWidget>

#include <fstream>

/**
 * Constructor.
 *
 * @param emulator_video Pointer to the emulator's video object. This is used
 *        to render the emulator video output.
 * @param parent Parent for this widget.
 */
MainWindow::MainWindow(EmulatorCore &emu, QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _emu(emu)
{
    _ui->setupUi(this);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addWidget(emu.GetVideo());

    QWidget *window = new QWidget();
    window->setLayout(main_layout);

    setCentralWidget(window);

    _status_text = new QLabel();
    _ui->statusbar->addPermanentWidget(_status_text);
}

/**
 * Set the text in the status bar.
 *
 * @param string The text to set.
 */
void MainWindow::SetStatusText(const QString &string)
{
    _status_text->setText(string);
}

/**
 * Pass a KeyPressEvent down to the EmulatorCore.
 *
 * @param event An even describing the key that was pressed.
 */
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    /**
     * Don't handle "auto-repeat keys" caused by holding the key down. Only
     * check actual key presses.
     */
    if(!event->isAutoRepeat())
        _emu.UpdateKeyboardStrobe(event);
}

/**
 * Destructor.
 */
MainWindow::~MainWindow()
{
    delete _ui;
}

/**
 * Reset the emulator.
 */
void MainWindow::on_actionReset_triggered()
{
    _emu.ResetCpu();
    _ui->statusbar->showMessage("CPU Reset.", STATUS_TEXT_TIMEOUT);
}

/**
 * Power cycle the emulator system. This resets every module (not just the CPU,
 * unlike the Reset() method).
 */
void MainWindow::on_actionPower_Cycle_triggered()
{
    _emu.PowerCycle();
    _ui->statusbar->showMessage("System has been power cycled",
                                STATUS_TEXT_TIMEOUT);
}

/**
 * Save the emulator state to a file.
 */
void MainWindow::on_actionQuick_Save_triggered()
{
    save_state("quicksave.a2s");
}

/**
 * Load the emulator state from a file.
 */
void MainWindow::on_actionQuick_Load_triggered()
{
    load_state("quicksave.a2s");
}


/**
 * Save a custom state file.
 */
void MainWindow::on_actionSave_State_triggered()
{
    QString filename = QFileDialog::getSaveFileName(
                this,
                "Save State",
                "",
                "Apple State (*.a2s)");

    if(filename.size() != 0)
        save_state(filename);
}

/**
 * Load a custom state file.
 */
void MainWindow::on_actionLoad_State_triggered()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                "Load State",
                "",
                "Apple State (*.a2s)");

    if(filename.size() != 0)
        load_state(filename);
}

/**
 * Save emulator state out to a file.
 *
 * @param filename The state to save.
 */
void MainWindow::save_state(QString filename)
{
    std::ofstream output(filename.toStdString(),
                         std::ios::out | std::ios::binary);

    if(output.is_open())
    {
        if(_emu.SaveState(output))
        {
            _ui->statusbar->showMessage("State saved.", STATUS_TEXT_TIMEOUT);
        }
        else
        {
            QMessageBox::warning(this,
                                 "Can't Save Emulator State",
                                 "There was an error while trying to save the "
                                    "emulator state. Ensure that you have "
                                    "write access in the working directory.");
        }

        output.close();
    }
    else
    {

        QMessageBox::warning(this,
                             "Can't Save Emulator State",
                             "Unable to open 'quicksave.a2s' as a writable "
                                "file");
    }
}

/**
 * Load emulator state from a file.
 *
 * @param filename The state to open.
 */
void MainWindow::load_state(QString filename)
{
    std::ifstream input(filename.toStdString(),
                        std::ios::in | std::ios::binary);

    if(input.is_open())
    {
        if(_emu.LoadState(input))
        {
            _ui->statusbar->showMessage("State loaded.", STATUS_TEXT_TIMEOUT);
        }
        else
        {
            QMessageBox::warning(this,
                                 "Can't Load Emulator State",
                                 "There was an error while trying to load the "
                                    "emulator state. This could be caused by "
                                    "a corrupted state file.\n\nThe system "
                                    " is in an invalid state and will reset.");

            _emu.PowerCycle();
        }

        input.close();
    }
    else
    {

        QMessageBox::warning(this,
                             "Can't Load Emulator State",
                             "Unable to open 'quicksave.a2s' as a readable "
                                "file. Ensure this file exists in the current "
                                "working directory.");
    }
}

/**
 * Open up the settings dialog.
 */
void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog *settings = new SettingsDialog(_emu, this);
    settings->open();
}

/**
 * Opens up the View Memory window.
 */
void MainWindow::on_actionView_Memory_triggered()
{

}

/**
 * Opens up the disassembly window.
 */
void MainWindow::on_actionDisassembly_triggered()
{
    DisassemblyWindow *dis = new DisassemblyWindow(_emu, this);
    dis->show();
}
