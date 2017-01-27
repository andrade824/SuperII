#ifndef MEMORY_H
#define MEMORY_H

#include "IMemoryMapped.h"

#include <cstdint>

class Memory : public IMemoryMapped
{
public:
    explicit Memory(uint16_t start_addr, uint16_t end_addr, bool write_protect);
    ~Memory();

    Memory(const Memory &copy) = delete;
    Memory & operator=(const Memory &rhs) = delete;

    void LoadMemory(const uint8_t *data, uint16_t data_size);

    uint8_t Read(uint16_t addr) const override;

    void Write(uint16_t addr, uint8_t data) override;

private:
    /**
     * The actual memory data.
     */
    uint8_t *_memory;

    /**
     * The size of the memory.
     */
    uint32_t _size;

    /**
     * True if the memory is write-protected.
     */
    bool _write_protect;
};

#endif // MEMORY_H
