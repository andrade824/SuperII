#ifndef IREADWRITE_H
#define IREADWRITE_H

#include <cstdint>

/**
 * @brief Standard interface for modules that provide a way to read/write
 *        memory mapped values.
 */
class IReadWrite
{
public:
    // Read an 8-bit value from this device
    virtual uint8_t Read(uint16_t addr) const = 0;

    // Write an 8-bit value to a value within a 16-bit address space
    virtual void Write(uint16_t addr, uint8_t data) = 0;

    // Yay, polymorphism
    virtual ~IReadWrite() {}
};

#endif // IREADWRITE_H
