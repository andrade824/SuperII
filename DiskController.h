#ifndef DISKCONTROLLER_H
#define DISKCONTROLLER_H

#include "Cpu.h"
#include "DiskImage.h"
#include "IMemoryMapped.h"

class DiskController : public IMemoryMapped
{
private:
    /**
     * Used to describe the phase of the motor that was last enabled.
     */
    enum DiskPhase { PHASE_0 = 0, PHASE_1, PHASE_2, PHASE_3 };

public:
    DiskController(Cpu &cpu);

    uint8_t Read(uint16_t addr, bool no_side_fx = false) override;
    void Write(uint16_t addr, uint8_t data) override;

private:
    void toggle_switch(uint16_t addr);

    void update_phase(DiskPhase new_phase);

    void perform_read_write();

private:
    /**
     * It takes the disk controller four CPU cycles to read/write a single bit.
     */
    static constexpr int CYCLES_PER_BIT = 4;

private:
    /**
     * Used to retrieve cycle counts whenever a read/write is requested.
     * This is used to know how many bits to move the "motor" by (every four
     * cycles the motor moves one bit).
     */
    Cpu &_cpu;

    /**
     * Holds the data to read/write.
     */
    uint8_t _data_reg;
    /**
     * False for shifting data from the disk image into the data register on
     * each cycle, true for loading data from the data bus into the data
     * register on the next cycle.
     */
    bool _shift_load;

    /**
     * False for reading, true for writing.
     *
     * This switch and the _shift_load switch determine the action the disk
     * controller takes each cycle:
     *
     * !_shift_load && !_read_write = Reading data (shift new data)
     * _shift_load && !_read_write = Read status of write-protect switch
     * !_shift_load && _read_write = Shift out one bit and write it
     * _shift_load && _read_write = Load data to write into the data register
     */
    bool _read_write;

    /**
     * The last phase that was enabled
     */
    DiskPhase _last_phase;

    /**
     * The current track to read/write to.
     *
     * @note There are only 35 tracks on a single disk and to move one track
     *       two phases have to be enabled. If you only enable a single phase
     *       then the disk is on a "half-track". To support this correctly,
     *       this variable goes from 0 to 69 (it keeps track of the half
     *       tracks), and to find the current actual track for reading/writing
     *       you need to divide this variable by two (shift right by one).
     */
    uint8_t _cur_track;
};

#endif // DISKCONTROLLER_H
