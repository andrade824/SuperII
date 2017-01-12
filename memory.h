#ifndef MEMORY_H
#define MEMORY_H

#include "ireadwrite.h"

#include <cstdint>
#include <string>

using std::string;

class Memory : public IReadWrite
{
public:
    explicit Memory(uint32_t size);
    ~Memory();

    Memory(const Memory &copy) = delete;
    Memory & operator=(const Memory &rhs) = delete;

    bool LoadExecutable(const string &path, uint16_t start_addr);

    uint8_t Read(uint16_t addr) const override;

    void Write(uint16_t addr, uint8_t data) override;

private:
    uint8_t *_memory;
    uint32_t _size;
};

#endif // MEMORY_H
