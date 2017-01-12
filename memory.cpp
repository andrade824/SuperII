/**
 * Represents a memory module within the Apple II computer.
 */
#include "memory.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;

/**
 * Constructor.
 *
 * @param size Size of the memory to allocate.
 */
Memory::Memory(uint32_t size) : _memory(nullptr), _size(size)
{
    if (size != 0)
        _memory = new uint8_t[size];
}

/**
 * Load a binary from a file and place it into memory.
 *
 * @param path Path to the executable to load up.
 * @param start_addr Address where to put the executable.
 *
 * @return True on successful load, false otherwise.
 */
bool Memory::LoadExecutable(const string &path, uint16_t start_addr)
{
    bool file_loaded = true;
    uint32_t size = 0;

    ifstream file(path, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        size = file.tellg();

        if ((start_addr + size) <= _size) {
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char*>(_memory + start_addr), size);
        } else {
            file_loaded = false;
            cout << "Can't load executable with size " << size
                 << " at starting address " << start_addr << endl;
        }
    } else {
        file_loaded = false;
        cout << "Failed to open executable '" << path << "'" << endl;
    }

    file.close();

    return file_loaded;
}

/**
 * Read a single 8-bit quantity out of memory.
 *
 * @param addr Address to read from
 *
 * @return The data at that address
 */
uint8_t Memory::Read(uint16_t addr) const
{
    uint8_t value = 0;

    if(addr == 0xF004) {
        value = getchar();

        if(value == '\n')
            value = '\r';
    } else
        value = _memory[addr];

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
    if(addr == 0xF001)
        printf("%c", data);
    else
        _memory[addr] = data;
}

/**
 * Destructor.
 */
Memory::~Memory()
{
    if(_memory != nullptr)
        delete [] _memory;
}
