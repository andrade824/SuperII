/**
 * Handles read/write accesses to disks.
 *
 * This class keeps track of the state of the stepper motor (used to change
 * tracks), and motor position (used to figure out which bit to read/write on
 * the current track).
 *
 * This class also encapsulates the disk controller ROM which is executed on
 * start-up to bootstrap DOS into memory.
 */
#include "DiskController.h"

#include <cstdlib>

/**
 * The on-board ROM for the Disk II controller card. The Apple II+
 * autostart ROM will look for this ROM and if it finds it, will
 * automatically start executing it (hence "autostart").
 */
static constexpr uint8_t disk_rom[256] = {
    0xa2, 0x20, 0xa0, 0x00, 0xa2, 0x03, 0x86, 0x3c,
    0x8a, 0x0a, 0x24, 0x3c, 0xf0, 0x10, 0x05, 0x3c,
    0x49, 0xff, 0x29, 0x7e, 0xb0, 0x08, 0x4a, 0xd0,
    0xfb, 0x98, 0x9d, 0x56, 0x03, 0xc8, 0xe8, 0x10,
    0xe5, 0x20, 0x58, 0xff, 0xba, 0xbd, 0x00, 0x01,
    0x0a, 0x0a, 0x0a, 0x0a, 0x85, 0x2b, 0xaa, 0xbd,
    0x8e, 0xc0, 0xbd, 0x8c, 0xc0, 0xbd, 0x8a, 0xc0,
    0xbd, 0x89, 0xc0, 0xa0, 0x50, 0xbd, 0x80, 0xc0,
    0x98, 0x29, 0x03, 0x0a, 0x05, 0x2b, 0xaa, 0xbd,
    0x81, 0xc0, 0xa9, 0x56, 0x20, 0xa8, 0xfc, 0x88,
    0x10, 0xeb, 0x85, 0x26, 0x85, 0x3d, 0x85, 0x41,
    0xa9, 0x08, 0x85, 0x27, 0x18, 0x08, 0xbd, 0x8c,
    0xc0, 0x10, 0xfb, 0x49, 0xd5, 0xd0, 0xf7, 0xbd,
    0x8c, 0xc0, 0x10, 0xfb, 0xc9, 0xaa, 0xd0, 0xf3,
    0xea, 0xbd, 0x8c, 0xc0, 0x10, 0xfb, 0xc9, 0x96,
    0xf0, 0x09, 0x28, 0x90, 0xdf, 0x49, 0xad, 0xf0,
    0x25, 0xd0, 0xd9, 0xa0, 0x03, 0x85, 0x40, 0xbd,
    0x8c, 0xc0, 0x10, 0xfb, 0x2a, 0x85, 0x3c, 0xbd,
    0x8c, 0xc0, 0x10, 0xfb, 0x25, 0x3c, 0x88, 0xd0,
    0xec, 0x28, 0xc5, 0x3d, 0xd0, 0xbe, 0xa5, 0x40,
    0xc5, 0x41, 0xd0, 0xb8, 0xb0, 0xb7, 0xa0, 0x56,
    0x84, 0x3c, 0xbc, 0x8c, 0xc0, 0x10, 0xfb, 0x59,
    0xd6, 0x02, 0xa4, 0x3c, 0x88, 0x99, 0x00, 0x03,
    0xd0, 0xee, 0x84, 0x3c, 0xbc, 0x8c, 0xc0, 0x10,
    0xfb, 0x59, 0xd6, 0x02, 0xa4, 0x3c, 0x91, 0x26,
    0xc8, 0xd0, 0xef, 0xbc, 0x8c, 0xc0, 0x10, 0xfb,
    0x59, 0xd6, 0x02, 0xd0, 0x87, 0xa0, 0x00, 0xa2,
    0x56, 0xca, 0x30, 0xfb, 0xb1, 0x26, 0x5e, 0x00,
    0x03, 0x2a, 0x5e, 0x00, 0x03, 0x2a, 0x91, 0x26,
    0xc8, 0xd0, 0xee, 0xe6, 0x27, 0xe6, 0x3d, 0xa5,
    0x3d, 0xcd, 0x00, 0x08, 0xa6, 0x2b, 0x90, 0xdb,
    0x4c, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00
};

/**
 * Constructor.
 *
 * @param cpu Reference to the CPU so cycle counts can be detected.
 */
DiskController::DiskController(Cpu &cpu) :
    IMemoryMapped(DISK_START_ADDR, DISK_END_ADDR),
    _cpu(cpu),
    _data_reg(0),
    _shift_load(false),
    _read_write(false),
    _motor_on(false),
    _drive_0_enabled(true),
    _cur_phase(PHASE_0),
    _cur_track(0),
    _leftover_cycles(CYCLES_PER_BIT),
    _last_cycle_count(0),
    _drive0(),
    _drive1()
{ }

/**
 * Loads a disk image into the specified drive.
 *
 * @param drive The drive to load a disk into.
 * @param data The disk image data.
 */
void DiskController::LoadDisk(DriveId drive, uint8_t data[DiskDrive::DISK_SIZE])
{
    switch(drive)
    {
    case DRIVE_0:
        _drive0.LoadDisk(data);
        break;

    case DRIVE_1:
        _drive1.LoadDisk(data);
        break;
    }
}

/**
 * Read data from the disk or toggle a switch.
 *
 * @param addr The address to read.
 * @param no_side_fx True if this read shouldn't cause any side effects
 *                   (used by the memory view and disassembly).
 *
 * @return Data register (if address is even) or zero.
 */
uint8_t DiskController::Read(uint16_t addr, bool no_side_fx)
{
    if(addr >= DISK_ROM_START && addr <= DISK_ROM_END)
        return disk_rom[addr & 0xFF];

    if(!no_side_fx)
        perform_read_write(addr, 0);

    /**
     * A read to any even address will dump the data register onto the data bus.
     */
    return (addr & 1) ? 0 : _data_reg;
}

/**
 * Write to the disk or toggle a soft switch.
 *
 * @param addr The address to write to.
 * @param data The data to write.
 */
void DiskController::Write(uint16_t addr, uint8_t data)
{
    perform_read_write(addr, data);
}

/**
 * Handle toggling disk controller soft switches.
 *
 * @param addr The address of the soft switch to toggle.
 */
void DiskController::toggle_switch(uint16_t addr)
{
    /**
     * The row is the current phase, and the column is how far to move the
     * stepper motor (which determines the track) if that phase is enabled.
     */
    int phase_delta[4][4] = { { 0, 1, 2, -1  },
                              { -1, 0, 1, 2  },
                              { -2, -1, 0, 1 },
                              { 1, -2, -1, 0 } };

    switch(addr)
    {

    /**
     * I take a shortcut and just ignore turning off phases. I assume the
     * program is disabling them correctly and only update the current track
     * number when phases are enabled.
     */
    case 0xC0E0:
    case 0xC0E2:
    case 0xC0E4:
    case 0xC0E6:
        break;

    case 0xC0E1:
        _cur_track += phase_delta[static_cast<int>(_cur_phase)][0];
        _cur_phase = PHASE_0;
        break;

    case 0xC0E3:
        _cur_track += phase_delta[static_cast<int>(_cur_phase)][1];
        _cur_phase = PHASE_1;
        break;

    case 0xC0E5:
        _cur_track += phase_delta[static_cast<int>(_cur_phase)][2];
        _cur_phase = PHASE_2;
        break;

    case 0xC0E7:
        _cur_track += phase_delta[static_cast<int>(_cur_phase)][3];
        _cur_phase = PHASE_3;
        break;

    case 0xC0E8:
        _motor_on = false;
        break;

    case 0xC0E9:
        _motor_on = true;
        break;

    case 0xC0EA:
        _drive_0_enabled = true;
        break;

    case 0xC0EB:
        _drive_0_enabled = false;
        break;

    case 0xC0EC:
        _shift_load = false;
        break;

    case 0xC0ED:
        _shift_load = true;
        break;

    case 0xC0EE:
        _read_write = false;
        break;

    case 0xC0EF:
        _read_write = true;
        break;
    }

    if(_cur_track > (DiskDrive::NUM_TRACKS * 2) - 1)
        _cur_track = (DiskDrive::NUM_TRACKS * 2) - 1;
    else if(_cur_track < 0)
        _cur_track = 0;
}

/**
 * Handle moving the motor (current disk position), reading/writing bits, and
 * toggling soft switches.
 *
 * @param data_bus The data on the data bus (for reads this should be zero,
 *                 for write this should be the value to be written).
 */
void DiskController::perform_read_write(uint16_t addr, uint8_t data_bus)
{
    DiskDrive &drive = (_drive_0_enabled) ? _drive0 : _drive1;
    int cycle_delta = _cpu.GetTotalCycles() - _last_cycle_count;
    bool switches_toggled = false;

    if(_motor_on)
    {
        cycle_delta -= _leftover_cycles;

        while(cycle_delta >= 0)
        {
            /**
             * Run the motor and read bits up until the last bit before
             * toggling any switches. This simulates the motor free-running
             * until the current read takes place, in which any toggles are
             * switched and the last bit is read with the new switches.
             */
            if(cycle_delta < 4)
            {
                toggle_switch(addr);
                switches_toggled = true;
            }

            update_data_reg(data_bus);

            drive.SeekBit(_cur_track / 2);

            /**
             * If a full byte has been loaded (the MSB is '1'), then wait
             * more cycles than usual to give the CPU extra time to read
             * the data.
             */
            if(!_read_write && !_shift_load && (_data_reg & 0x80))
                cycle_delta -= VALID_BYTE_CYCLES;
            else
                cycle_delta -= CYCLES_PER_BIT;
        }

        _leftover_cycles = abs(cycle_delta);
    }
    else
    {
        toggle_switch(addr);
        switches_toggled = true;

        update_data_reg(data_bus);
    }

    if(!switches_toggled)
        toggle_switch(addr);

    _last_cycle_count = _cpu.GetTotalCycles();
}

/**
 * Update the data register based on the shift/load and read/write flags.
 *
 * The _read_write and _shift_load switches determine the action the disk
 * controller takes each cycle:
 *
 * !_read_write && !_shift_load = Read data (shift new data).
 * !_read_write && _shift_load = Read status of write-protect switch.
 * _read_write && !_shift_load = Shift out one bit and write it.
 * _read_write && _shift_load = Load data to write into the data register.
 */
void DiskController::update_data_reg(uint8_t data_bus)
{
    DiskDrive &drive = (_drive_0_enabled) ? _drive0 : _drive1;

    if(!_read_write && !_shift_load)
    {
        /**
         * If the last shift set the MSB, then clear the data register before
         * shifting the next bit. It's assumed that the appropriate amount
         * of time has been given to the CPU to read the data.
         */
        if(_data_reg & 0x80)
            _data_reg = 0;

        _data_reg = (_data_reg << 1) | drive.GetBit(_cur_track / 2);
    }
    else if(!_read_write && _shift_load)
    {
        _data_reg = (drive.GetWriteProtect()) ? 0x80 : 0x0;
    }
    else if(_read_write && !_shift_load)
    {
        _data_reg <<= 1;
        drive.SetBit(_cur_track / 2, (_data_reg >> 7) & 1);
    }
    else if(_read_write && _shift_load)
    {
        _data_reg = data_bus;
        drive.SeekPrevByte(_cur_track / 2);
        drive.SetBit(_cur_track / 2, (_data_reg >> 7) & 1);
    }
}
