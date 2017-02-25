#ifndef MEMORY_H
#define MEMORY_H

#include "IMemoryMapped.h"
#include "IState.h"

#include <cstdint>

#include <fstream>

class Memory : public IMemoryMapped, public IState
{
public:
    explicit Memory(uint16_t start_addr, uint16_t end_addr, bool write_protect);
    ~Memory();

    Memory(const Memory &copy) = delete;
    Memory & operator=(const Memory &rhs) = delete;

    void Reset();

    void LoadMemory(const uint8_t *data, uint16_t data_size);

    uint8_t Read(uint16_t addr) override;
    void Write(uint16_t addr, uint8_t data) override;

    void SaveState(std::ofstream &output) override;
    void LoadState(std::ifstream &input) override;

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
