/**
 * Represents a memory module within the Apple II computer.
 */
#include "Memory.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>

/**
 * Constructor.
 *
 * @param start_addr Start address of this memory device.
 * @param end_addr End address of this memory device.
 */
Memory::Memory(uint16_t start_addr, uint16_t end_addr, bool write_protect) :
    IMemoryMapped(start_addr, end_addr),
    _memory(nullptr),
    _size(end_addr - start_addr + 1),
    _write_protect(write_protect)
{
    assert(_size != 0);
    _memory = new uint8_t[_size];
}

/**
 * Reset the memory back to all zeroes.
 */
void Memory::Reset()
{
    memset(_memory, 0x0, _size);
}

/**
 * Copy data into this memory device starting at the bottom of memory.
 *
 * @note If the data is larger than the memory, then the data will be
 *       truncated to fit.
 *
 * @param data The data to copy.
 * @param size The size of the data.
 */
void Memory::LoadMemory(const uint8_t *data, uint16_t data_size)
{
    uint16_t bytes_to_copy = (data_size > _size) ? _size : data_size;

    std::memcpy(_memory, data, bytes_to_copy);
}

/**
 * Read a single 8-bit quantity out of memory.
 *
 * @param addr Address to read from
 *
 * @return The data at that address
 */
uint8_t Memory::Read(uint16_t addr)
{
    uint8_t value = 0;

    assert((uint16_t)(addr - _start_addr) < _size);

    value = _memory[addr - _start_addr];

    return value;
}

/**
 * Write a single 8-bit quantity to memory.
 *
 * @param addr The address being written.
 * @param data The data to write to this memory.
 */
void Memory::Write(uint16_t addr, uint8_t data)
{
    if(!_write_protect)
        _memory[addr - _start_addr] = data;
}

/**
 * Save the Memory state out to a file.
 *
 * @param output The file to write to.
 */
void Memory::SaveState(std::ofstream &output)
{
    output.write(reinterpret_cast<char*>(_memory), _size);
}

/**
 * Load the Memory state out of a file.
 *
 * @param input The file to read from.
 */
void Memory::LoadState(std::ifstream &input)
{
    input.read(reinterpret_cast<char*>(_memory), _size);
}


/**
 * Destructor.
 */
Memory::~Memory()
{
    if(_memory != nullptr)
        delete [] _memory;
}
