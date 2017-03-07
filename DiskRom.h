#ifndef DISKROM_H
#define DISKROM_H

#include "IMemoryMapped.h"

#include <cstdint>

class DiskRom : public IMemoryMapped
{
public:
    DiskRom();

    uint8_t Read(uint16_t addr, bool no_side_fx = false) override;
    void Write(uint16_t addr, uint8_t data) override;

private:
    /**
     * Start and end addresses (inclusive) for the Disk ROM.
     */
    static constexpr uint16_t ROM_START = 0xC600;
    static constexpr uint16_t ROM_END = 0xC6FF;
};

#endif // DISKROM_H
