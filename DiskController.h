#ifndef DISKCONTROLLER_H
#define DISKCONTROLLER_H

#include "Cpu.h"
#include "DiskDrive.h"
#include "IMemoryMapped.h"
#include "IState.h"

#include <fstream>
#include <string>

class DiskController : public IMemoryMapped, public IState
{
public:
    /**
     * Used for distinguishing between the disk in drive 0, and the disk in
     * drive 1.
     */
    enum DriveId { DRIVE_0 = 0, DRIVE_1 };

    /**
     * Start and end addresses (inclusive) for the Disk ROM.
     */
    static constexpr uint16_t DISK_ROM_START = 0xC600;
    static constexpr uint16_t DISK_ROM_END = 0xC6FF;

public:
    DiskController(Cpu &cpu);

    void Reset();

    void LoadDisk(std::string filename,
                  DriveId drive,
                  uint8_t data[DiskDrive::DISK_SIZE]);
    void UnloadDisk(DriveId drive);

    uint8_t Read(uint16_t addr, bool no_side_fx = false) override;
    void Write(uint16_t addr, uint8_t data) override;

    void SaveState(std::ofstream &output) override;
    void LoadState(std::ifstream &input) override;

    std::string GetDiskFilename(DriveId drive) const;

private:
    void toggle_switch(uint16_t addr);

    void perform_read_write(uint16_t addr, uint8_t data_bus);
    void update_data_reg(uint8_t data_bus);

private:
    /**
     * Start and end addresses (inclusive) for the Disk controller.
     */
    static constexpr int DISK_START_ADDR = 0xC0E0;
    static constexpr int DISK_END_ADDR = 0xC0EF;

    /**
     * It takes the disk controller four CPU cycles to read/write a single bit.
     */
    static constexpr int CYCLES_PER_BIT = 4;

    /**
     * The amount of cycles to hold a valid byte before clearing the data
     * register and shifting in new data.
     */
    static constexpr int VALID_BYTE_CYCLES = 8;

    /**
     * Used to describe the phase of the motor that was last enabled.
     */
    enum DiskPhase { PHASE_0 = 0, PHASE_1, PHASE_2, PHASE_3 };

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
     */
    bool _read_write;

    /**
     * True if the motor is enabled, false otherwise.
     */
    bool _motor_on;

    /**
     * True if drive 0 is enabled, false for drive 1.
     */
    bool _drive_0_enabled;

    /**
     * The last phase that was enabled
     */
    DiskPhase _cur_phase;

    /**
     * The current track to read/write to.
     *
     * @note There are only 35 tracks on a single disk and to move one track,
     *       two phases have to be enabled. If you only enable a single phase
     *       then the disk is on a "half-track". To support this correctly,
     *       this variable goes from 0 to 69 (it keeps track of the half
     *       tracks), and to find the current actual track for reading/writing
     *       you need to divide this variable by two (shift right by one).
     */
    int _cur_track;

    /**
     * How many cycles need to be executed in the next read/write to cause
     * the motor to move one bit.
     */
    uint32_t _leftover_cycles;

    /**
     * What the CPU's cycle count was the last time a read/write occurred.
     */
    uint32_t _last_cycle_count;

    /**
     * The disk inserted into drive 0.
     */
    DiskDrive _drive0;

    /**
     * The disk inserted into drive 1.
     */
    DiskDrive _drive1;
};

#endif // DISKCONTROLLER_H
