#ifndef SYSTEMBUS_H
#define SYSTEMBUS_H

#include "IMemoryMapped.h"

#include <cstdint>

/**
 * Defines a region within the 6502 CPU's address space and which component
 * will respond to memory accesses to that region.
 */
struct MemoryRegion
{
    /**
     * Inclusive start address of 'device' in the address space.
     */
    uint16_t start;

    /**
     * Inclusive end address of 'device' in the address space.
     */
    uint16_t end;

    /**
     * The device to perform reads/writes on when a memory request is made in
     * the memory region going from 'start' to 'end' inclusively.
     */
    IMemoryMapped &device;
};

class SystemBus : public IMemoryMapped
{
public:
    SystemBus();
};

#endif // SYSTEMBUS_H
