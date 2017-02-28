#include "DisassemblyWindow.h"
#include "instrs_6502.h"
#include "ui_DisassemblyWindow.h"

#include <QTimer>

/**
 * Constructor.
 *
 * @param emu Reference to the main emulator core.
 * @param parent Parent widget for this window.
 */
DisassemblyWindow::DisassemblyWindow(EmulatorCore &emu, QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::DisassemblyWindow),
    _refresh_timer(this),
    _emu(emu)
{
    _ui->setupUi(this);

    connect(&_refresh_timer,
            &QTimer::timeout,
            this,
            &DisassemblyWindow::refresh_timer_timeout);
    _refresh_timer.start(REFRESH_TIMEOUT);

    _ui->asmTable->setHorizontalHeaderLabels({"Address", "Opcode", "Acronym"});

    static constexpr int ROM_START = 0xD000;
    static constexpr int ROM_END = 0xFFFF;
    update_table(ROM_START, ROM_END);

    _ui->bpSpin->setEnabled(_emu.GetBpEnabled());
    _ui->bpSpin->setValue(_emu.GetBpAddr());

    refresh_timer_timeout();
}

/**
 * Update the table with the latest memory.
 *
 * @param start Inclusive start address.
 * @param end Inclusive end address.
 */
void DisassemblyWindow::update_table(uint16_t start, uint16_t end)
{
    QByteArray mem;
    _emu.GetMemory(mem, start, end);

    _ui->asmTable->clearContents();
    _ui->asmTable->setRowCount(0);

    for(int i = 0; i < mem.size();)
    {
        CpuInstruction instr = instrs_6502[mem[i] & 0xFF];
        _ui->asmTable->setRowCount(_ui->asmTable->rowCount() + 1);
        const int cur_row = _ui->asmTable->rowCount() - 1;

        QString addr_string = "0x" +
                              tr("%1").arg(i + start, 4, 16, QChar('0')).toUpper();
        _ui->asmTable->setItem(cur_row, 0, new QTableWidgetItem(addr_string));

        QString bytes = "";
        for(int j = 0; j < instr.size; ++j)
            bytes += "0x" + tr("%1").arg(mem[i+j] & 0xFF, 2, 16, QChar('0')).toUpper() + " ";

        _ui->asmTable->setItem(cur_row, 1, new QTableWidgetItem(bytes));

        QString acronym = QString::fromStdString(instr.acronym);
        _ui->asmTable->setItem(cur_row, 2, new QTableWidgetItem(acronym));

        i += instr.size;
    }
}

/**
 * Make sure the emulator is in a stable state when the disassembly is closed.
 *
 * @param event The close event.
 */
void DisassemblyWindow::closeEvent(QCloseEvent *)
{
    _emu.SetPaused(false);
    _emu.SetBpEnabled(false);
}

/**
 * Check to see if the emulator is paused, and if so, update the GUI.
 */
void DisassemblyWindow::refresh_timer_timeout()
{
    if(_emu.GetPaused())
    {
        if(!_ui->singleStepBtn->isEnabled())
            on_updateFromPcBtn_clicked();

        _ui->contBreakBtn->setText("Continue");
        _ui->singleStepBtn->setEnabled(true);

    }
    else
    {
        _ui->contBreakBtn->setText("Break");
        _ui->singleStepBtn->setEnabled(false);
    }
}

/**
 * Ensure that the start address never goes above the end address.
 */
void DisassemblyWindow::on_startAddrSpin_editingFinished()
{
    if(_ui->startAddrSpin->value() >= _ui->endAddrSpin->value())
        _ui->startAddrSpin->setValue(_ui->endAddrSpin->value() - 1);
}

/**
 * Ensure that the end address never goes below the start address.
 */
void DisassemblyWindow::on_endAddrSpin_editingFinished()
{
    if(_ui->endAddrSpin->value() <= _ui->startAddrSpin->value())
        _ui->endAddrSpin->setValue(_ui->startAddrSpin->value() + 1);
}

/**
 * Update the table with the memory addresses in the spin boxes.
 */
void DisassemblyWindow::on_updateBtn_clicked()
{
    update_table(_ui->startAddrSpin->value() & 0xFFFF,
                 _ui->endAddrSpin->value() & 0xFFFF);
}

/**
 * Update the table starting from the PC and going until the top of memory.
 */
void DisassemblyWindow::on_updateFromPcBtn_clicked()
{
    _ui->startAddrSpin->setValue(_emu.GetCpuContext().pc);

    on_updateBtn_clicked();
}

/**
 * Pause or continue the CPU.
 */
void DisassemblyWindow::on_contBreakBtn_clicked()
{
    _emu.SetPaused(!_emu.GetPaused());

    if(!_emu.GetPaused())
        _ui->singleStepBtn->setEnabled(false);
}

/**
 * Single step the CPU.
 */
void DisassemblyWindow::on_singleStepBtn_clicked()
{
    _emu.SingleStep();

    if(_ui->updateCheck->isChecked())
        on_updateFromPcBtn_clicked();
}

/**
 * Toggle a breakpoint on the currently selected line.
 */
void DisassemblyWindow::on_toggleBpBtn_clicked()
{
    if(_emu.GetBpEnabled())
    {
        _emu.SetBpEnabled(false);
        _ui->bpSpin->setEnabled(false);
    }
    else
    {
        const int cur_row = _ui->asmTable->currentRow();
        if(cur_row >= 0)
        {
            QTableWidgetItem *item = _ui->asmTable->item(cur_row, 0);
            const unsigned int bp_addr = item->text().toUInt(nullptr, 16);

            _emu.SetBpAddr(bp_addr & 0xFFFF);
            _emu.SetBpEnabled(true);
            _ui->bpSpin->setEnabled(true);
            _ui->bpSpin->setValue(bp_addr);
        }
    }
}

/**
 * Destructor.
 */
DisassemblyWindow::~DisassemblyWindow()
{
    delete _ui;
}

