#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Video.h"

#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(Video *emulator_video, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->addWidget(emulator_video);

    QWidget *window = new QWidget();
    window->setLayout(main_layout);

    // Set QWidget as the central layout of the main window
    setCentralWidget(window);
}

MainWindow::~MainWindow()
{
    delete ui;
}
