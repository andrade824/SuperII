#ifndef SYSTEMBUS_H
#define SYSTEMBUS_H

#include "IMemoryMapped.h"

#include <cstdint>
#include <vector>

/**
 * Provides the connection between all of the different components in the
 * emulated system using a simple Read()/Write() interface.
 */
class SystemBus
{
private:
    struct IoDevice
    {
        /**
         * The memory mapped device.
         */
        IMemoryMapped *device;

        /**
         * Inclusive start address of the device within the address space.
         */
        uint16_t start;

        /**
         * Inclusive end address of the device within the address space.
         */
        uint16_t end;
    };

public:
    SystemBus();

    SystemBus(const SystemBus &copy) = delete;
    SystemBus& operator=(const SystemBus &rhs) = delete;

    void Register(IMemoryMapped *device);
    void Register(IMemoryMapped *device, uint16_t start, uint16_t end);

    uint8_t Read(uint16_t addr, bool no_side_fx = false);
    void Write(uint16_t addr, uint8_t data);

private:
    /**
     * List of memory mapped devices that define the address space for this bus.
     */
    std::vector<IoDevice> _devices;
};

#endif // SYSTEMBUS_H
