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
public:
    SystemBus();

    SystemBus(const SystemBus &copy) = delete;
    SystemBus& operator=(const SystemBus &rhs) = delete;

    void Register(IMemoryMapped *device);

    uint8_t Read(uint16_t addr) const;
    void Write(uint16_t addr, uint8_t data);

private:
    /**
     * List of memory mapped devices that define the address space for this bus.
     */
    std::vector<IMemoryMapped*> _devices;
};

#endif // SYSTEMBUS_H
