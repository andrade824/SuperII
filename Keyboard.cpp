/**
 * Emulates a standard Apple II keyboard.
 */
#include "Keyboard.h"

#include <QKeyEvent>
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

void Keyboard::UpdateKeyboardMapping(QKeyEvent key, uint8_t scancode)
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

/**
 * Keyboard event comparison operator overload meant to be used by the
 * unordered_map data structure.
 *
 * @param lhs Left hand side of the equality.
 * @param rhs Right hand side of the equality.
 *
 * @return True if the two events are identical, otherwise false.
 */
//bool operator==(const sf::Event::KeyEvent &lhs, const sf::Event::KeyEvent &rhs)
//{
//    return (lhs.code == rhs.code) &&
//           (lhs.alt == rhs.alt) &&
//           (lhs.control == rhs.control) &&
//           (lhs.shift == rhs.shift) &&
//           (lhs.system == rhs.system);
//}
