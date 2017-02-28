#include "CpuRegistersWindow.h"
#include "Cpu.h"
#include "ui_CpuRegistersWindow.h"

/**
 * Constructor.
 *
 * @param emu Reference to the main emulator core.
 * @param parent Parent widget for this window.
 */
CpuRegistersWindow::CpuRegistersWindow(EmulatorCore &emu, QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CpuRegistersWindow),
    _refresh_timer(this),
    _emu(emu)
{
    _ui->setupUi(this);

    connect(&_refresh_timer,
            &QTimer::timeout,
            this,
            &CpuRegistersWindow::refresh_timer_timeout);

    _refresh_timer.start(REFRESH_TIMEOUT);
}

/**
 * Update the GUI with the latest register values.
 */
void CpuRegistersWindow::refresh_timer_timeout()
{
    CpuContext context = _emu.GetCpuContext();

    QString temp = "0x" + tr("%1").arg(context.pc, 4, 16, QChar('0')).toUpper();
    _ui->pcEdit->setText(temp);

    temp = "0x" + tr("%1").arg(context.acc, 2, 16, QChar('0')).toUpper();
    _ui->accEdit->setText(temp);

    temp = "0x" + tr("%1").arg(context.x, 2, 16, QChar('0')).toUpper();
    _ui->xEdit->setText(temp);

    temp = "0x" + tr("%1").arg(context.y, 2, 16, QChar('0')).toUpper();
    _ui->yEdit->setText(temp);

    temp = "0x" + tr("%1").arg(context.sp, 2, 16, QChar('0')).toUpper();
    _ui->spEdit->setText(temp);

    temp = "0x" + tr("%1").arg(context.sr, 2, 16, QChar('0')).toUpper();
    _ui->srEdit->setText(temp);
}

/**
 * Destructor.
 */
CpuRegistersWindow::~CpuRegistersWindow()
{
    delete _ui;
}
