#include "CpuRegistersWindow.h"
#include "DisassemblyWindow.h"
#include "DiskController.h"
#include "EmulatorCore.h"
#include "MainWindow.h"
#include "SettingsDialog.h"
#include "ui_MainWindow.h"
#include "ViewMemoryWindow.h"

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
    _status_text(nullptr),
    _disk_busy(nullptr),
    _check_disk_busy(nullptr),
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

    _disk_busy = new QRadioButton("Disk Busy", this);
    _disk_busy->setChecked(false);
    _disk_busy->setFocusPolicy(Qt::NoFocus);
    _ui->statusbar->addPermanentWidget(_disk_busy);

    _check_disk_busy = new QTimer(this);
    _check_disk_busy->setInterval(DISK_BUSY_TIMEOUT);

    connect(_check_disk_busy,
            &QTimer::timeout,
            this,
            &MainWindow::disk_busy_timeout);

    _check_disk_busy->start();
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
                             "Unable to open the state file as a writable "
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
            QString drive0_filename = QString::fromStdString(
                        _emu.GetDiskFilename(DiskController::DRIVE_0));
            _ui->actionDrive_0->setText("Drive 0: " + drive0_filename + "...");

            QString drive1_filename = QString::fromStdString(
                        _emu.GetDiskFilename(DiskController::DRIVE_1));
            _ui->actionDrive_1->setText("Drive 1: " + drive1_filename + "...");

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
                             "Unable to open the state file as a readable "
                                "file.");
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
 * Opens up the disassembly window.
 */
void MainWindow::on_actionDisassembly_triggered()
{
    DisassemblyWindow *dis = new DisassemblyWindow(_emu, this);
    dis->show();
}

/**
 * Opens up the CPU Registers window.
 */
void MainWindow::on_actionCPU_Registers_triggered()
{
    CpuRegistersWindow *reg = new CpuRegistersWindow(_emu, this);
    reg->show();
}

/**
 * Opens up the View Memory window.
 */
void MainWindow::on_actionView_Memory_triggered()
{
    ViewMemoryWindow *mem = new ViewMemoryWindow(_emu, this);
    mem->show();
}

/**
 * Load a disk into the disk controller.
 *
 * @param drive Which disk drive to load the disk image into.
 */
void MainWindow::load_disk(DiskController::DriveId drive)
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                "Load Disk",
                "",
                "Disk Image (*.dsk)");

    if(filename.size() != 0)
    {
        std::ifstream input(filename.toStdString(),
                            std::ios::ate | std::ios::binary);

        if(input.is_open())
        {
            uint32_t filesize = input.tellg();

            if(filesize == DiskDrive::DISK_SIZE)
            {
                uint8_t file_data[DiskDrive::DISK_SIZE];
                input.seekg(0, std::ios::beg);
                input.read(reinterpret_cast<char*>(file_data), filesize);

                _emu.LoadDisk(filename.toStdString(), drive, file_data);

                if(drive == DiskController::DRIVE_0)
                    _ui->actionDrive_0->setText("Drive 0: " + filename + "...");
                else
                    _ui->actionDrive_1->setText("Drive 1: " + filename + "...");

                _ui->statusbar->showMessage("Disk loaded.", STATUS_TEXT_TIMEOUT);
            }
            else
            {
                QMessageBox::warning(this,
                                     "Can't Load Disk Image",
                                     "There was an error while trying to load "
                                        "the disk image. Ensure the image you "
                                        "selected is a valid image (should be "
                                        "exactly 140KB in size).");

                _emu.UnloadDisk(drive);

                if(drive == DiskController::DRIVE_0)
                    _ui->actionDrive_0->setText("Drive 0: None...");
                else
                    _ui->actionDrive_1->setText("Drive 1: None...");
            }

            input.close();
        }
        else
        {

            QMessageBox::warning(this,
                                 "Can't Load Disk Image",
                                 "Unable to open the disk image as a readable "
                                    "file.");
        }
    }
}

/**
 * Loads a disk into Drive 0.
 */
void MainWindow::on_actionDrive_0_triggered()
{
    load_disk(DiskController::DRIVE_0);
}

/**
 * Loads a disk into Drive 1.
 */
void MainWindow::on_actionDrive_1_triggered()
{
    load_disk(DiskController::DRIVE_1);
}

/**
 * Check to see if the disk is busy, and update the status bar if so.
 */
void MainWindow::disk_busy_timeout()
{
    if(_emu.GetDiskBusy())
        _disk_busy->setChecked(!_disk_busy->isChecked());
    else
        _disk_busy->setChecked(false);
}
