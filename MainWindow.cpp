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
MainWindow::MainWindow(Video *emulator_video, QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addWidget(emulator_video);

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
 * Destructor.
 */
MainWindow::~MainWindow()
{
    delete _ui;
}
