#ifndef IREADWRITE_H
#define IREADWRITE_H

#include <cstdint>

/**
 * Standard interface for modules that provide a way to read/write memory mapped
 * values.
 */
class IReadWrite
{
public:
    /**
     * Read an 8-bit value from this device.
     *
     * @param addr The 16-bit address to read from.
     *
     * @return The value at address 'addr'.
     */
    virtual uint8_t Read(uint16_t addr) const = 0;

    /**
     *  Write an 8-bit value to a location within a 16-bit address space.
     *
     * @param addr The 16-bit address to write to.
     * @param data The 8-bit data to write.
     */
    virtual void Write(uint16_t addr, uint8_t data) = 0;

    /**
     * Required for polymorphism.
     */
    virtual ~IReadWrite() {}
};

#endif // IREADWRITE_H
