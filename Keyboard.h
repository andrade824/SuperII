#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "IMemoryMapped.h"

#include <cstddef>
#include <functional>
#include <unordered_map>

#include <QKeyEvent>

/**
 * A hashing algorithm that will let the Qt keyboard events be used as a key
 * in the unordered_map data structure.
 */
struct KeyEventHasher
{
    std::size_t operator()(const QKeyEvent &key) const
    {
        return static_cast<uint32_t>(key.key()) |
                   ((key.modifiers() & Qt::AltModifier) ? 0x80000000 : 0) |
                   ((key.modifiers() & Qt::ControlModifier) ? 0x40000000 : 0) |
                   ((key.modifiers() & Qt::ShiftModifier) ? 0x20000000 : 0) |
                   ((key.modifiers() & Qt::MetaModifier) ? 0x10000000 : 0);
    }
};

/**
 * Keyboard event comparison operator overload meant to be used by the
 * unordered_map data structure.
 */
//bool operator==(const sf::Event::KeyEvent &lhs,
//                const sf::Event::KeyEvent &rhs);

class Keyboard : public IMemoryMapped
{
public:
    Keyboard();

    void UpdateKeyboardStrobe(const QKeyEvent *key);

    void UpdateKeyboardMapping(QKeyEvent key, uint8_t scancode);

    uint8_t Read(uint16_t addr);
    void Write(uint16_t addr, uint8_t);

private:
    /**
     * Start and end addresses (inclusive) for the Keyboard registers.
     */
    static constexpr int KEYBOARD_START_ADDR = 0xC000;
    static constexpr int KEYBOARD_END_ADDR = 0xC010;

    /**
     * Keyboard data.
     */
    uint8_t _data;

    /**
     * Default mapping of keyboard keys.
     */
    std::unordered_map<
        sf::Event::KeyEvent,
        uint8_t,
        KeyEventHasher,
        std::function<bool(const sf::Event::KeyEvent&,
                           const sf::Event::KeyEvent&)>> _key_map = {
        { { sf::Keyboard::Key::A, false, false, false, false}, 0xC1 },
        { { sf::Keyboard::Key::A, false, true, false, false}, 0x81 },
        { { sf::Keyboard::Key::A, false, false, true, false}, 0xC1 },
        { { sf::Keyboard::Key::A, false, true, true, false}, 0x81 }
    };
};

#endif // KEYBOARD_H
