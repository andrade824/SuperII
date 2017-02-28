#ifndef IMEMORYMAPPED_H
#define IMEMORYMAPPED_H

#include <cstdint>

/**
 * Standard interface for modules that provide a way to read/write memory mapped
 * values.
 */
class IMemoryMapped
{
public:
    /**
     * Constructor.
     *
     * @param start_addr Inclusive start address of this device in the address
     *                   space.
     * @param end_addr Inclusive end address of this device in the address
     *                 space.
     */
    IMemoryMapped(uint16_t start_addr, uint16_t end_addr) :
        _start_addr(start_addr),
        _end_addr(end_addr)
    { }

    /**
     * Start Address Getter.
     */
    uint16_t GetStartAddr() const
    {
        return _start_addr;
    }

    /**
     * End Address Getter.
     */
    uint16_t GetEndAddr() const
    {
        return _end_addr;
    }

    /**
     * Read an 8-bit value from this device.
     *
     * @param addr The 16-bit address to read from.
     * @param no_side_fx True if this read shouldn't cause any side effects
     *                   (used by the memory view and disassembly).
     *
     * @return The value at address 'addr'.
     */
    virtual uint8_t Read(uint16_t addr, bool no_side_fx = false) = 0;

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
    virtual ~IMemoryMapped() {}

protected:
    uint16_t _start_addr;
    uint16_t _end_addr;
};

#endif // IMEMORYMAPPED_H
