#include "ViewMemoryWindow.h"
#include "ui_ViewMemoryWindow.h"

/**
 * Constructor.
 *
 * @param emu Reference to the main emulator core.
 * @param parent Parent widget for this window.
 */
ViewMemoryWindow::ViewMemoryWindow(EmulatorCore &emu, QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::ViewMemoryWindow),
    _refresh_timer(this),
    _emu(emu)
{
    _ui->setupUi(this);

    _ui->memoryTable->setHorizontalHeaderLabels({"Address", "Data"});
    _ui->memoryTable->setRowCount(0xFFFF + 1);

    _ui->refreshSpin->setValue(REFRESH_TIMEOUT);

    connect(&_refresh_timer,
            &QTimer::timeout,
            this,
            &ViewMemoryWindow::refresh_timer_timeout);

    _refresh_timer.setInterval(REFRESH_TIMEOUT);
    refresh_timer_timeout();
}

/**
 * Update the GUI with the latest memory values.
 */
void ViewMemoryWindow::refresh_timer_timeout()
{
    QList<QTableWidgetSelectionRange> range = _ui->memoryTable->selectedRanges();
    update_table(0x0, 0xFFFF);

    if(range.size() > 0)
        _ui->memoryTable->setRangeSelected(range[0], true);
}

/**
 * Update the table with the latest memory.
 *
 * @param start Inclusive start address.
 * @param end Inclusive end address.
 */
void ViewMemoryWindow::update_table(uint16_t start, uint16_t end)
{
    QByteArray mem;
    _emu.GetMemory(mem, start, end);

    for(int i = 0; i < mem.size(); ++i)
    {
        QString addr = "0x" +
                       tr("%1").arg(i + start, 4, 16, QChar('0')).toUpper();
        _ui->memoryTable->setItem(i, 0, new QTableWidgetItem(addr));

        QString byte = "0x" +
                       tr("%1").arg(mem[i] & 0xFF, 2, 16, QChar('0')).toUpper();
        _ui->memoryTable->setItem(i, 1, new QTableWidgetItem(byte));
    }
}

/**
 * Scroll the memory table to the wanted row.
 *
 * @param row The row to scroll to.
 */
void ViewMemoryWindow::scroll_to_row(int row)
{
    _ui->memoryTable->scrollToItem(_ui->memoryTable->item(row, 0),
                                   QAbstractItemView::PositionAtTop);
}

/**
 * Enable or disable auto-refresh.
 *
 * @param checked True for auto-refresh, false for manual.
 */
void ViewMemoryWindow::on_autoRefreshCheck_toggled(bool checked)
{
    if(checked)
        _refresh_timer.start();
    else
        _refresh_timer.stop();
}

/**
 * Update the auto-refresh timer interval.
 */
void ViewMemoryWindow::on_refreshSpin_editingFinished()
{
    _refresh_timer.setInterval(_ui->refreshSpin->value());
}

/**
 * Manually refresh the memory view.
 */
void ViewMemoryWindow::on_refreshBtn_clicked()
{
    refresh_timer_timeout();
}

/**
 * Scroll the memory table to a certain address.
 */
void ViewMemoryWindow::on_gotoBtn_clicked()
{
    scroll_to_row(_ui->gotoSpin->value());
}

/**
 * Scroll the memory table to a certain address.
 */
void ViewMemoryWindow::on_gotoSpin_editingFinished()
{
    on_gotoBtn_clicked();
}

/**
 * Scroll the memory table to the PC;
 */
void ViewMemoryWindow::on_gotoPcBtn_clicked()
{
    scroll_to_row(_emu.GetCpuContext().pc);
}

/**
 * Destructor.
 */
ViewMemoryWindow::~ViewMemoryWindow()
{
    delete _ui;
}
