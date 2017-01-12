#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <string>

#include "ireadwrite.h"

using std::string;

class Memory : public IReadWrite
{
public:
    explicit Memory(uint32_t size);
    ~Memory();

    // TODO: Add copy constructor and assignment op overload

    // Load a binary from a file and place it into memory
    bool LoadExecutable(const string &path, uint16_t start_addr);

    // Read a single 8-bit quantity out of memory
    uint8_t Read(uint16_t addr) const override;

    // Write a single 8-bit quantity to memory
    void Write(uint16_t addr, uint8_t data) override;

private:
    uint8_t *_memory;
    uint32_t _size;
};

#endif // MEMORY_H
