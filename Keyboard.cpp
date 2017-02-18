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
 * Update the keyboard strobe register if a valid key was pressed.
 *
 * @param key The key that was pressed.
 */
void Keyboard::UpdateKeyboardStrobe(const QKeyEvent *key)
{
    if(_key_map.count(*key) != 0)
    {
        _data = _key_map[*key];
    }
}

void Keyboard::UpdateKeyboardMapping(QKeyEvent, uint8_t)
{

}

/**
 * Read the keyboard data or clear the keyboard strobe.
 *
 * @param addr The address to read.
 *
 * @return Keyboard data or zero.
 */
uint8_t Keyboard::Read(uint16_t addr)
{
    uint8_t ret_val = 0;

    switch(addr)
    {
        case 0xC000: ret_val = _data; break;
        case 0xC010: _data &= 0x7F; break;
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