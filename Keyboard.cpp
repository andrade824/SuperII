/**
 * Emulates a standard Apple II keyboard.
 */
#include "Keyboard.h"

#include <QKeyEvent>

/**
 * Hashes a Qt KeyEvent to a single 32-bit number.
 *
 * @param key The key to hash.
 *
 * @return The hashed value.
 */
std::size_t KeyEventHasher::operator()(const QKeyEvent &key) const
{
    return static_cast<uint32_t>(key.key()) |
           ((key.modifiers() & Qt::AltModifier) ? 0x80000000 : 0) |
           ((key.modifiers() & Qt::ControlModifier) ? 0x40000000 : 0) |
           ((key.modifiers() & Qt::ShiftModifier) ? 0x20000000 : 0) |
           ((key.modifiers() & Qt::MetaModifier) ? 0x10000000 : 0);
}

/**
 * Keyboard event equality operator overload meant to be used by the
 * unordered_map data structure.
 *
 * @param lhs Left hand side of the equality.
 * @param rhs Right hand side of the equality.
 *
 * @return True if the two events are identical, otherwise false.
 */
bool operator==(const QKeyEvent &lhs, const QKeyEvent &rhs)
{
    return (lhs.key() == rhs.key()) &&
           (lhs.modifiers() == rhs.modifiers()) &&
           (lhs.type() == rhs.type());
}

/**
 * Constructor.
 */
Keyboard::Keyboard() :
    IMemoryMapped(KEYBOARD_START_ADDR, KEYBOARD_END_ADDR),
    _data(0)
{ }

/**
 * Reset the keyboard strobe.
 */
void Keyboard::Reset()
{
    _data = 0;
}

/**
 * Update the keyboard strobe register if a valid key was pressed.
 *
 * @param key The key that was pressed.
 */
void Keyboard::UpdateKeyboardStrobe(const QKeyEvent *key)
{
    if(_key_map.count(*key) != 0)
    {
        _data = _key_map[*key].code;
    }
}

/**
 * Gets the keyboard mappings.
 *
 * @return The hash table of keyboard mappings.
 */
key_mappings Keyboard::GetMappings() const
{
    return _key_map;
}

/**
 * Update the keyboard mappings.
 *
 * @param key_map The new keyboard mappings.
 */
void Keyboard::SetMappings(key_mappings key_map)
{
    _key_map = key_map;
}

/**
 * Read the keyboard data or clear the keyboard strobe.
 *
 * @param addr The address to read.
 * @param no_side_fx True if this read shouldn't cause any side effects
 *                   (used by the memory view and disassembly).
 *
 * @return Keyboard data or zero.
 */
uint8_t Keyboard::Read(uint16_t addr, bool no_side_fx)
{
    uint8_t ret_val = 0;

    switch(addr)
    {
        case 0xC000: ret_val = _data; break;
        case 0xC010:
            if(!no_side_fx)
                _data &= 0x7F;
            break;
    }

    return ret_val;
}

/**
 * Potentially clear the keyboard strobe.
 *
 * @param addr The address to write to.
 */
void Keyboard::Write(uint16_t addr, uint8_t)
{
    if(addr == 0xC010)
        _data &= 0x7F;
}

/**
 * Save the Keyboard state out to a file.
 *
 * @param output The file to write to.
 */
void Keyboard::SaveState(std::ofstream &output)
{
    output.write(reinterpret_cast<char*>(&_data), sizeof(_data));
}

/**
 * Load the Keyboard state out of a file.
 *
 * @param input The file to read from.
 */
void Keyboard::LoadState(std::ifstream &input)
{
    input.read(reinterpret_cast<char*>(&_data), sizeof(_data));
}
