/**
 * The Language Card is an add-on card usually placed into Slot 0. This card
 * includes a copy of the Apple II autostart ROM, as well as an additional
 * 16KB of RAM.
 *
 * The Apple II only has a 64KB address space, so how is the language card
 * able to supply an extra 16KB of RAM (on top of the standard 48KB of RAM)
 * and still let users access the ROM and I/O addresses? The answer is bank
 * switching. The Language Card has control addresses (starting at 0xC080)
 * that let's a user switch between RAM and ROM within 0xD000 to 0xFFFF (where
 * ROM usually resides by itself). Astute readers will notice that address
 * range only gives you 12KB of addresses. The other 4KB of RAM is bank-switched
 * into the bottom 4KB of that 12KB region. So, in a nutshell, the upper 8KB
 * are a fixed chunk of RAM, while the bottom 4KB can be swapped between one of
 * two different 4KB regions (which all adds up to 16KB of RAM).
 *
 * Many popular games require this full 64KB of RAM (e.g., Oregon Trail).
 */

#include "applesoft_rom.h"
#include "LanguageCard.h"

#include <cstring>

/**
 * Constructor.
 */
LanguageCard::LanguageCard() :
    IMemoryMapped(ROM_START, ROM_END),
    _ram_static(),
    _ram_bank1(),
    _ram_bank2(),
    _status(WRITE_ENABLE)
{ }

/**
 * Reset the memory back to all zeroes.
 */
void LanguageCard::Reset()
{
    _status = WRITE_ENABLE;
    std::memset(_ram_static, 0x0, 8192);
    std::memset(_ram_bank1, 0x0, 4096);
    std::memset(_ram_bank2, 0x0, 4096);
}

/**
 * Read a single 8-bit quantity out of memory.
 *
 * @param addr Address to read from.
 *
 * @return The data at that address.
 */
uint8_t LanguageCard::Read(uint16_t addr, bool no_side_fx)
{
    uint8_t value = 0;

    if(addr >= LANG_CARD_START && addr <= LANG_CARD_END && !no_side_fx)
    {
        value = handle_control(addr);
    }
    else if((_status & READ_ENABLE) && addr >= ROM_START)
    {
        if(addr >= (ROM_START + 0x1000))
            value = _ram_static[addr - (ROM_START + 0x1000)];
        else if(addr < (ROM_START + 0x1000) && (_status & BANK_SELECT))
            value = _ram_bank1[addr - ROM_START];
        else
            value = _ram_bank2[addr - ROM_START];
    }
    else if(!(_status & READ_ENABLE) && addr >= ROM_START)
    {
        value = applesoft_rom[addr - ROM_START];
    }

    return value;
}

/**
 * Write a single 8-bit quantity to memory.
 *
 * @param addr The address being written.
 * @param data The data to write to this memory.
 */
void LanguageCard::Write(uint16_t addr, uint8_t data)
{
    if(addr >= LANG_CARD_START && addr <= LANG_CARD_END)
    {
        handle_control(addr);
    }
    else if((_status & WRITE_ENABLE) && addr >= ROM_START)
    {
        if(addr >= (ROM_START + 0x1000))
            _ram_static[addr - (ROM_START + 0x1000)] = data;
        else if(addr < (ROM_START + 0x1000) && (_status & BANK_SELECT))
            _ram_bank1[addr - ROM_START] = data;
        else
            _ram_bank2[addr - ROM_START] = data;
    }
}

/**
 * Handle modifying the language card state based on which control address is
 * accessed.
 *
 * @param addr The control address being accessed.
 *
 * @return The language card status before it was potentially modified by this
 *         access.
 */
uint8_t LanguageCard::handle_control(uint16_t addr)
{
    uint8_t old_status = _status;

    /**
     * First two bits of the address determine write-protection and whether to
     * read out of RAM or ROM.
     */
    switch(addr & 0x3)
    {
    case 0x0:
        /**
         * RAM read and RAM write-protect.
         */
        _status = READ_ENABLE;
        break;

    case 0x1:
        /**
         * ROM read. Two or more successive accesses to this address
         * write-enables the RAM.
         */
        _status = (_status & NEXT) ? WRITE_ENABLE : NEXT;
        break;

    case 0x2:
        /**
         * ROM read and RAM write-protect.
         */
        _status = 0;
        break;

    case 0x3:
        /**
         * RAM read. Two or more successive accesses to this address
         * write-enables the RAM.
         */
        _status = (_status & NEXT) ? WRITE_ENABLE : NEXT;
        _status |= READ_ENABLE;
        break;
    }

    /**
     * If bit 3 of the control address is zero, then bank 2 will be mapped into
     * 0xD000-0xDFFF. Otherwise, bank 1 will be mapped.
     */
    _status |= (addr & 0x8) ? BANK_SELECT : 0x0;

    return old_status;
}

/**
 * Save the Language Card state out to a file.
 *
 * @param output The file to write to.
 */
void LanguageCard::SaveState(std::ofstream &output)
{
    output.write(reinterpret_cast<char*>(_ram_static), sizeof(_ram_static));
    output.write(reinterpret_cast<char*>(_ram_bank1), sizeof(_ram_bank1));
    output.write(reinterpret_cast<char*>(_ram_bank2), sizeof(_ram_bank2));
    output.write(reinterpret_cast<char*>(&_status), sizeof(_status));
}

/**
 * Load the Language Card state out of a file.
 *
 * @param input The file to read from.
 */
void LanguageCard::LoadState(std::ifstream &input)
{
    input.read(reinterpret_cast<char*>(_ram_static), sizeof(_ram_static));
    input.read(reinterpret_cast<char*>(_ram_bank1), sizeof(_ram_bank1));
    input.read(reinterpret_cast<char*>(_ram_bank2), sizeof(_ram_bank2));
    input.read(reinterpret_cast<char*>(&_status), sizeof(_status));
}
