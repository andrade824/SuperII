/**
 * Provides the main access point between all of the components in the emulated
 * system. When the processor makes a read/write request, that request is
 * sent to this module to determine which component needs to satisfy that
 * request.
 *
 * If the CPU tries to read from an address with no associated component, it
 * will read 0xFF. Writes to dummy addresses have no effect.
 */

#include "SystemBus.h"

#include <algorithm>

/**
 * Constructor.
 */
SystemBus::SystemBus() : _devices()
{ }

/**
 * Register a device within the address space.
 *
 * @note When a read/write is requested on the bus, the devices will be searched
 *       in the order they were registered. For best performance, register
 *       devices that will be accessed more often before other regions.
 *
 * @note It's up to the user to ensure that memory regions don't overlap.
 *
 * @param region The memory region to add to the search list when a read/write
 *               occurs.
 */
void SystemBus::Register(IMemoryMapped *device)
{
    _devices.push_back(device);
}

/**
 * Attempt to read from a device on the system bus. If no device is registered
 * at the requested address, then 0xFF is returned.
 *
 * @note This method searches devices starting with the device that was
 *       registered first. Therefore, devices registered first will be read
 *       from faster than others farther down the list.
 *
 * @param addr The address to read from.
 * @param no_side_fx True if this read shouldn't cause any side effects
 *                   (used by the memory view and disassembly).
 *
 * @return Data if a device is registered at 'addr', otherwise 0x00.
 */
uint8_t SystemBus::Read(uint16_t addr, bool no_side_fx)
{
    auto found_device = find_if(_devices.begin(),
                                _devices.end(),
                                [&] (const IMemoryMapped *device)
                                {
                                    return (addr >= device->GetStartAddr()) &&
                                           (addr <= device->GetEndAddr());
                                });

    if(found_device != _devices.end())
        return (*found_device)->Read(addr, no_side_fx);
    else
        return 0x00;
}

/**
 * Attempt to write to a device on the system bus. If no device is registered
 * at the requested address, then nothing is written.
 *
 * @note This method searches devices starting with the device that was
 *       registered first. Therefore, devices registered first will be written
 *       to faster than others farther down the list.
 *
 * @param addr The address to write to.
 */
void SystemBus::Write(uint16_t addr, uint8_t data)
{
    auto found_device = find_if(_devices.begin(),
                                _devices.end(),
                                [&] (const IMemoryMapped *device)
                                {
                                    return (addr >= device->GetStartAddr()) &&
                                           (addr <= device->GetEndAddr());
                                });

    if(found_device != _devices.end())
        (*found_device)->Write(addr, data);
}
