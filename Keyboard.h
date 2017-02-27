#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "IMemoryMapped.h"
#include "IState.h"

#include <cstddef>
#include <fstream>
#include <functional>
#include <unordered_map>

#include <QKeyEvent>

/**
 * A hashing algorithm that will let Qt keyboard events be used as an index
 * into the unordered_map data structure.
 */
struct KeyEventHasher
{
    std::size_t operator()(const QKeyEvent &key) const;
};

/**
 * Keyboard event comparison operator overload meant to be used by the
 * unordered_map data structure.
 */
bool operator==(const QKeyEvent &lhs, const QKeyEvent &rhs);

/**
 * Structure to represent an Apple II scancode.
 */
struct Scancode
{
    /**
     * The scancode value.
     */
    uint8_t code;

    /**
     * The textual represenatation of the key (e.g., "Space", "Left").
     */
    QString text;
};

/**
 * Alias type for the unordered map of key bindings.
 */
using key_mappings = std::unordered_map<QKeyEvent, Scancode, KeyEventHasher>;

class Keyboard : public IMemoryMapped, public IState
{
public:
    Keyboard();

    void Reset();

    void UpdateKeyboardStrobe(const QKeyEvent *key);

    key_mappings GetMappings() const;
    void SetMappings(key_mappings key_map);

    uint8_t Read(uint16_t addr) override;
    void Write(uint16_t addr, uint8_t) override;

    void SaveState(std::ofstream &output) override;
    void LoadState(std::ifstream &input) override;

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
    key_mappings _key_map = {
        { { QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier }, {0xA0, "Space"} },

        { { QEvent::KeyPress, Qt::Key_0, Qt::NoModifier }, {0xB0, "0"} },

        { { QEvent::KeyPress, Qt::Key_1, Qt::NoModifier }, {0xB1, "1"} },
        { { QEvent::KeyPress, Qt::Key_Exclam, Qt::ShiftModifier }, {0xA1, "!"} },

        { { QEvent::KeyPress, Qt::Key_2, Qt::NoModifier }, {0xB2, "2"} },
        { { QEvent::KeyPress, Qt::Key_QuoteDbl, Qt::ShiftModifier }, {0xA2, "\""} },

        { { QEvent::KeyPress, Qt::Key_3, Qt::NoModifier }, {0xB3, "3"} },
        { { QEvent::KeyPress, Qt::Key_NumberSign, Qt::ShiftModifier }, {0xA3, "#"} },

        { { QEvent::KeyPress, Qt::Key_4, Qt::NoModifier }, {0xB4, "4"} },
        { { QEvent::KeyPress, Qt::Key_Dollar, Qt::ShiftModifier }, {0xA4, "$"} },

        { { QEvent::KeyPress, Qt::Key_5, Qt::NoModifier }, {0xB5, "5"} },
        { { QEvent::KeyPress, Qt::Key_Percent, Qt::ShiftModifier }, {0xA5, "%"} },

        { { QEvent::KeyPress, Qt::Key_6, Qt::NoModifier }, {0xB6, "6"} },
        { { QEvent::KeyPress, Qt::Key_Ampersand, Qt::ShiftModifier }, {0xA6, "&"} },

        { { QEvent::KeyPress, Qt::Key_7, Qt::NoModifier }, {0xB7, "7"} },
        { { QEvent::KeyPress, Qt::Key_Apostrophe, Qt::NoModifier }, {0xA7, "'"} },

        { { QEvent::KeyPress, Qt::Key_8, Qt::NoModifier }, {0xB8, "8"} },
        { { QEvent::KeyPress, Qt::Key_ParenLeft, Qt::ShiftModifier }, {0xA8, "("} },

        { { QEvent::KeyPress, Qt::Key_9, Qt::NoModifier }, {0xB9, "9"} },
        { { QEvent::KeyPress, Qt::Key_ParenRight, Qt::ShiftModifier }, {0xA9, ")"} },

        { { QEvent::KeyPress, Qt::Key_Colon, Qt::ShiftModifier }, {0xBA, ":"} },
        { { QEvent::KeyPress, Qt::Key_Asterisk, Qt::ShiftModifier }, {0xAA, "*"} },

        { { QEvent::KeyPress, Qt::Key_Semicolon, Qt::NoModifier }, {0xBB, ";"} },
        { { QEvent::KeyPress, Qt::Key_Plus, Qt::ShiftModifier }, {0xAB, "+"} },

        { { QEvent::KeyPress, Qt::Key_Comma, Qt::NoModifier }, {0xAC, ","} },
        { { QEvent::KeyPress, Qt::Key_Less, Qt::ShiftModifier }, {0xBC, "<"} },

        { { QEvent::KeyPress, Qt::Key_Minus, Qt::NoModifier }, {0xAD, "-"} },
        { { QEvent::KeyPress, Qt::Key_Equal, Qt::NoModifier }, {0xBD, "="} },

        { { QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier }, {0xAE, "."} },
        { { QEvent::KeyPress, Qt::Key_Greater, Qt::ShiftModifier }, {0xBE, ">"} },

        { { QEvent::KeyPress, Qt::Key_Slash, Qt::NoModifier }, {0xAF, "/"} },
        { { QEvent::KeyPress, Qt::Key_Question, Qt::ShiftModifier }, {0xBF, "?"} },

        { { QEvent::KeyPress, Qt::Key_A, Qt::NoModifier }, {0xC1, "A"} },
        { { QEvent::KeyPress, Qt::Key_A, Qt::ControlModifier }, {0x81, "Ctrl+A"} },
        { { QEvent::KeyPress, Qt::Key_A, Qt::ShiftModifier }, {0xC1, "A"} },

        { { QEvent::KeyPress, Qt::Key_B, Qt::NoModifier }, {0xC2, "B"} },
        { { QEvent::KeyPress, Qt::Key_B, Qt::ControlModifier }, {0x82, "CTRL+B"} },
        { { QEvent::KeyPress, Qt::Key_B, Qt::ShiftModifier }, {0xC2, "B"} },

        { { QEvent::KeyPress, Qt::Key_C, Qt::NoModifier }, {0xC3, "C"} },
        { { QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier }, {0x83, "CTRL+C"} },
        { { QEvent::KeyPress, Qt::Key_C, Qt::ShiftModifier }, {0xC3, "C"} },

        { { QEvent::KeyPress, Qt::Key_D, Qt::NoModifier }, {0xC4, "D"} },
        { { QEvent::KeyPress, Qt::Key_D, Qt::ControlModifier }, {0x84, "CTRL+D"} },
        { { QEvent::KeyPress, Qt::Key_D, Qt::ShiftModifier }, {0xC4, "D"} },

        { { QEvent::KeyPress, Qt::Key_E, Qt::NoModifier }, {0xC5, "E"} },
        { { QEvent::KeyPress, Qt::Key_E, Qt::ControlModifier }, {0x85, "CTRL+E"} },
        { { QEvent::KeyPress, Qt::Key_E, Qt::ShiftModifier }, {0xC5, "E"} },

        { { QEvent::KeyPress, Qt::Key_F, Qt::NoModifier }, {0xC6, "F"} },
        { { QEvent::KeyPress, Qt::Key_F, Qt::ControlModifier }, {0x86, "CTRL+F"} },
        { { QEvent::KeyPress, Qt::Key_F, Qt::ShiftModifier }, {0xC6, "F"} },

        { { QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier }, {0x8D, "Return"} },

        { { QEvent::KeyPress, Qt::Key_G, Qt::NoModifier }, {0xC7, "G"} },
        { { QEvent::KeyPress, Qt::Key_G, Qt::ControlModifier }, {0x87, "CTRL+G"} },
        { { QEvent::KeyPress, Qt::Key_G, Qt::ShiftModifier }, {0xC7, "G"} },

        { { QEvent::KeyPress, Qt::Key_H, Qt::NoModifier }, {0xC8, "H"} },
        { { QEvent::KeyPress, Qt::Key_H, Qt::ControlModifier }, {0x88, "CTRL+H"} },
        { { QEvent::KeyPress, Qt::Key_H, Qt::ShiftModifier }, {0xC8, "H"} },

        { { QEvent::KeyPress, Qt::Key_I, Qt::NoModifier }, {0xC9, "I"} },
        { { QEvent::KeyPress, Qt::Key_I, Qt::ControlModifier }, {0x89, "CTRL+I"} },
        { { QEvent::KeyPress, Qt::Key_I, Qt::ShiftModifier }, {0xC9, "I"} },

        { { QEvent::KeyPress, Qt::Key_J, Qt::NoModifier }, {0xCA, "J"} },
        { { QEvent::KeyPress, Qt::Key_J, Qt::ControlModifier }, {0x8A, "CTRL+J"} },
        { { QEvent::KeyPress, Qt::Key_J, Qt::ShiftModifier }, {0xCA, "J"} },

        { { QEvent::KeyPress, Qt::Key_K, Qt::NoModifier }, {0xCB, "K"} },
        { { QEvent::KeyPress, Qt::Key_K, Qt::ControlModifier }, {0x8B, "CTRL+K"} },
        { { QEvent::KeyPress, Qt::Key_K, Qt::ShiftModifier }, {0xCB, "K"} },

        { { QEvent::KeyPress, Qt::Key_L, Qt::NoModifier }, {0xCC, "L"} },
        { { QEvent::KeyPress, Qt::Key_L, Qt::ControlModifier }, {0x8C, "CTRL+L"} },
        { { QEvent::KeyPress, Qt::Key_L, Qt::ShiftModifier }, {0xCC, "L"} },

        { { QEvent::KeyPress, Qt::Key_M, Qt::NoModifier }, {0xCD, "M"} },
        { { QEvent::KeyPress, Qt::Key_M, Qt::ControlModifier }, {0x8D, "Return"} },
        { { QEvent::KeyPress, Qt::Key_M, Qt::ShiftModifier }, {0xDD, "SHIFT+M"} },
        { { QEvent::KeyPress, Qt::Key_M, Qt::ControlModifier |
                                         Qt::ShiftModifier }, {0x9D, "CTRL+SHIFT+M"} },

        { { QEvent::KeyPress, Qt::Key_N, Qt::NoModifier }, {0xCE, "N"} },
        { { QEvent::KeyPress, Qt::Key_N, Qt::ControlModifier }, {0x8E, "CTRL+N"} },
        { { QEvent::KeyPress, Qt::Key_AsciiCircum, Qt::ShiftModifier }, {0xDE, "^"} },
        { { QEvent::KeyPress, Qt::Key_AsciiCircum, Qt::ControlModifier |
                                                   Qt::ShiftModifier }, {0x9E, "CTRL+^"} },

        { { QEvent::KeyPress, Qt::Key_O, Qt::NoModifier }, {0xCF, "O"} },
        { { QEvent::KeyPress, Qt::Key_O, Qt::ControlModifier }, {0x8F, "CTRL+O"} },
        { { QEvent::KeyPress, Qt::Key_O, Qt::ShiftModifier }, {0xCF, "O"} },

        { { QEvent::KeyPress, Qt::Key_P, Qt::NoModifier }, {0xD0, "P"} },
        { { QEvent::KeyPress, Qt::Key_P, Qt::ControlModifier }, {0x90, "CTRL+P"} },
        { { QEvent::KeyPress, Qt::Key_At, Qt::ShiftModifier }, {0xC0, "@"} },
        { { QEvent::KeyPress, Qt::Key_At, Qt::ControlModifier |
                                          Qt::ShiftModifier }, {0x80, "CTRL+@"} },

        { { QEvent::KeyPress, Qt::Key_Q, Qt::NoModifier }, {0xD1, "Q"} },
        { { QEvent::KeyPress, Qt::Key_Q, Qt::ControlModifier }, {0x91, "CTRL+Q"} },
        { { QEvent::KeyPress, Qt::Key_Q, Qt::ShiftModifier }, {0xD1, "Q"} },

        { { QEvent::KeyPress, Qt::Key_R, Qt::NoModifier }, {0xD2, "R"} },
        { { QEvent::KeyPress, Qt::Key_R, Qt::ControlModifier }, {0x92, "CTRL+R"} },
        { { QEvent::KeyPress, Qt::Key_R, Qt::ShiftModifier }, {0xD2, "R"} },

        { { QEvent::KeyPress, Qt::Key_S, Qt::NoModifier }, {0xD3, "S"} },
        { { QEvent::KeyPress, Qt::Key_S, Qt::ControlModifier }, {0x93, "CTRL+S"} },
        { { QEvent::KeyPress, Qt::Key_S, Qt::ShiftModifier }, {0xD3, "S"} },

        { { QEvent::KeyPress, Qt::Key_T, Qt::NoModifier }, {0xD4, "T"} },
        { { QEvent::KeyPress, Qt::Key_T, Qt::ControlModifier }, {0x94, "CTRL+T"} },
        { { QEvent::KeyPress, Qt::Key_T, Qt::ShiftModifier }, {0xD4, "T"} },

        { { QEvent::KeyPress, Qt::Key_U, Qt::NoModifier }, {0xD5, "U"} },
        { { QEvent::KeyPress, Qt::Key_U, Qt::ControlModifier }, {0x95, "CTRL+U"} },
        { { QEvent::KeyPress, Qt::Key_U, Qt::ShiftModifier }, {0xD5, "U"} },

        { { QEvent::KeyPress, Qt::Key_V, Qt::NoModifier }, {0xD6, "V"} },
        { { QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier }, {0x96, "CTRL+V"} },
        { { QEvent::KeyPress, Qt::Key_V, Qt::ShiftModifier }, {0xD6, "V"} },

        { { QEvent::KeyPress, Qt::Key_W, Qt::NoModifier }, {0xD7, "W"} },
        { { QEvent::KeyPress, Qt::Key_W, Qt::ControlModifier }, {0x97, "CTRL+W"} },
        { { QEvent::KeyPress, Qt::Key_W, Qt::ShiftModifier }, {0xD7, "W"} },

        { { QEvent::KeyPress, Qt::Key_X, Qt::NoModifier }, {0xD8, "X"} },
        { { QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier }, {0x98, "CTRL+X"} },
        { { QEvent::KeyPress, Qt::Key_X, Qt::ShiftModifier }, {0xD8, "X"} },

        { { QEvent::KeyPress, Qt::Key_Y, Qt::NoModifier }, {0xD9, "Y"} },
        { { QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier }, {0x99, "CTRL+Y"} },
        { { QEvent::KeyPress, Qt::Key_Y, Qt::ShiftModifier }, {0xD9, "Y"} },

        { { QEvent::KeyPress, Qt::Key_Z, Qt::NoModifier }, {0xDA, "Z"} },
        { { QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier }, {0x9A, "CTRL+Z"} },
        { { QEvent::KeyPress, Qt::Key_Z, Qt::ShiftModifier }, {0xDA, "Z"} },

        { { QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier }, {0x95, "Right"} },

        { { QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier }, {0x88, "Left"} },
        { { QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier }, {0x88, "Left"} },

        { { QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier }, {0x9B, "Escape"} }
    };
};

#endif // KEYBOARD_H
