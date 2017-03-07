#include "DiskRom.h"

/**
 * Constructor.
 */
DiskRom::DiskRom() :
    IMemoryMapped(ROM_START, ROM_END)
{ }

/**
 * Read a single 8-bit quantity out of memory.
 *
 * @param addr Address to read from
 *
 * @return The data at that address
 */
uint8_t DiskRom::Read(uint16_t addr, bool)
{
    /**
     * The on-board ROM for the Disk II controller card. The Apple II+
     * autostart ROM will look for this ROM and if it finds it, will
     * automatically start executing it (hence "autostart").
     */
    static constexpr uint8_t rom[256] = {
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

    return rom[addr & 0xFF];
}

/**
 * Do nothing since this is a Read-Only memory.
 */
void DiskRom::Write(uint16_t, uint8_t)
{ }
