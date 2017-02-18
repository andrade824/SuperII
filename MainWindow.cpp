#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Video.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

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
    _ui->statusbar->addWidget(_status_text);
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
     * Don't handle "auto-repeat keys" caused by holding the key down. Only check
     * actual key presses.
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
