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

class Keyboard : public IMemoryMapped, public IState
{
public:
    Keyboard();

    void Reset();

    void UpdateKeyboardStrobe(const QKeyEvent *key);

    void UpdateKeyboardMapping(QKeyEvent key, uint8_t scancode);

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
    std::unordered_map<QKeyEvent, uint8_t, KeyEventHasher> _key_map = {
        { { QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier }, 0xA0 },
        { { QEvent::KeyPress, Qt::Key_Space, Qt::ControlModifier }, 0xA0 },
        { { QEvent::KeyPress, Qt::Key_Space, Qt::ShiftModifier }, 0xA0 },
        { { QEvent::KeyPress, Qt::Key_Space, Qt::ControlModifier |
                                             Qt::ShiftModifier }, 0xA0 },

        { { QEvent::KeyPress, Qt::Key_0, Qt::NoModifier }, 0xB0 },
        { { QEvent::KeyPress, Qt::Key_0, Qt::ControlModifier }, 0xB0 },

        { { QEvent::KeyPress, Qt::Key_1, Qt::NoModifier }, 0xB1 },
        { { QEvent::KeyPress, Qt::Key_1, Qt::ControlModifier }, 0xB1 },
        { { QEvent::KeyPress, Qt::Key_Exclam, Qt::ShiftModifier }, 0xA1 },
        { { QEvent::KeyPress, Qt::Key_Exclam, Qt::ControlModifier |
                                              Qt::ShiftModifier }, 0xA1 },

        { { QEvent::KeyPress, Qt::Key_2, Qt::NoModifier }, 0xB2 },
        { { QEvent::KeyPress, Qt::Key_2, Qt::ControlModifier }, 0xB2 },
        { { QEvent::KeyPress, Qt::Key_QuoteDbl, Qt::ShiftModifier }, 0xA2 },
        { { QEvent::KeyPress, Qt::Key_QuoteDbl, Qt::ControlModifier |
                                                Qt::ShiftModifier }, 0xA2 },

        { { QEvent::KeyPress, Qt::Key_3, Qt::NoModifier }, 0xB3 },
        { { QEvent::KeyPress, Qt::Key_3, Qt::ControlModifier }, 0xB3 },
        { { QEvent::KeyPress, Qt::Key_NumberSign, Qt::ShiftModifier }, 0xA3 },
        { { QEvent::KeyPress, Qt::Key_NumberSign, Qt::ControlModifier |
                                                  Qt::ShiftModifier }, 0xA3 },

        { { QEvent::KeyPress, Qt::Key_4, Qt::NoModifier }, 0xB4 },
        { { QEvent::KeyPress, Qt::Key_4, Qt::ControlModifier }, 0xB4 },
        { { QEvent::KeyPress, Qt::Key_Dollar, Qt::ShiftModifier }, 0xA4 },
        { { QEvent::KeyPress, Qt::Key_Dollar, Qt::ControlModifier |
                                              Qt::ShiftModifier }, 0xA4 },

        { { QEvent::KeyPress, Qt::Key_5, Qt::NoModifier }, 0xB5 },
        { { QEvent::KeyPress, Qt::Key_5, Qt::ControlModifier }, 0xB5 },
        { { QEvent::KeyPress, Qt::Key_Percent, Qt::ShiftModifier }, 0xA5 },
        { { QEvent::KeyPress, Qt::Key_Percent, Qt::ControlModifier |
                                               Qt::ShiftModifier }, 0xA5 },

        { { QEvent::KeyPress, Qt::Key_6, Qt::NoModifier }, 0xB6 },
        { { QEvent::KeyPress, Qt::Key_6, Qt::ControlModifier }, 0xB6 },
        { { QEvent::KeyPress, Qt::Key_Ampersand, Qt::ShiftModifier }, 0xA6 },
        { { QEvent::KeyPress, Qt::Key_Ampersand, Qt::ControlModifier |
                                                 Qt::ShiftModifier }, 0xA6 },

        { { QEvent::KeyPress, Qt::Key_7, Qt::NoModifier }, 0xB7 },
        { { QEvent::KeyPress, Qt::Key_7, Qt::ControlModifier }, 0xB7 },
        { { QEvent::KeyPress, Qt::Key_Apostrophe, Qt::NoModifier }, 0xA7 },
        { { QEvent::KeyPress, Qt::Key_Apostrophe, Qt::ControlModifier }, 0xA7 },

        { { QEvent::KeyPress, Qt::Key_8, Qt::NoModifier }, 0xB8 },
        { { QEvent::KeyPress, Qt::Key_8, Qt::ControlModifier }, 0xB8 },
        { { QEvent::KeyPress, Qt::Key_ParenLeft, Qt::ShiftModifier }, 0xA8 },
        { { QEvent::KeyPress, Qt::Key_ParenLeft, Qt::ControlModifier |
                                                 Qt::ShiftModifier }, 0xA8 },

        { { QEvent::KeyPress, Qt::Key_9, Qt::NoModifier }, 0xB9 },
        { { QEvent::KeyPress, Qt::Key_9, Qt::ControlModifier }, 0xB9 },
        { { QEvent::KeyPress, Qt::Key_ParenRight, Qt::ShiftModifier }, 0xA9 },
        { { QEvent::KeyPress, Qt::Key_ParenRight, Qt::ControlModifier |
                                                  Qt::ShiftModifier }, 0xA9 },

        { { QEvent::KeyPress, Qt::Key_Colon, Qt::ShiftModifier }, 0xBA },
        { { QEvent::KeyPress, Qt::Key_Colon, Qt::ControlModifier |
                                             Qt::ShiftModifier }, 0xBA },
        { { QEvent::KeyPress, Qt::Key_Asterisk, Qt::ShiftModifier }, 0xAA },
        { { QEvent::KeyPress, Qt::Key_Asterisk, Qt::ControlModifier |
                                                Qt::ShiftModifier }, 0xAA },

        { { QEvent::KeyPress, Qt::Key_Semicolon, Qt::NoModifier }, 0xBB },
        { { QEvent::KeyPress, Qt::Key_Semicolon, Qt::ControlModifier }, 0xBB },
        { { QEvent::KeyPress, Qt::Key_Plus, Qt::ShiftModifier }, 0xAB },
        { { QEvent::KeyPress, Qt::Key_Plus, Qt::ControlModifier |
                                            Qt::ShiftModifier }, 0xAB },

        { { QEvent::KeyPress, Qt::Key_Comma, Qt::NoModifier }, 0xAC },
        { { QEvent::KeyPress, Qt::Key_Comma, Qt::ControlModifier }, 0xAC },
        { { QEvent::KeyPress, Qt::Key_Less, Qt::ShiftModifier }, 0xBC },
        { { QEvent::KeyPress, Qt::Key_Less, Qt::ControlModifier |
                                            Qt::ShiftModifier }, 0xBC },

        { { QEvent::KeyPress, Qt::Key_Minus, Qt::NoModifier }, 0xAD },
        { { QEvent::KeyPress, Qt::Key_Minus, Qt::ControlModifier }, 0xAD },
        { { QEvent::KeyPress, Qt::Key_Equal, Qt::NoModifier }, 0xBD },
        { { QEvent::KeyPress, Qt::Key_Equal, Qt::ControlModifier }, 0xBD },

        { { QEvent::KeyPress, Qt::Key_Period, Qt::NoModifier }, 0xAE },
        { { QEvent::KeyPress, Qt::Key_Period, Qt::ControlModifier }, 0xAE },
        { { QEvent::KeyPress, Qt::Key_Greater, Qt::ShiftModifier }, 0xBE },
        { { QEvent::KeyPress, Qt::Key_Greater, Qt::ControlModifier |
                                               Qt::ShiftModifier }, 0xBE },

        { { QEvent::KeyPress, Qt::Key_Slash, Qt::NoModifier }, 0xAF },
        { { QEvent::KeyPress, Qt::Key_Slash, Qt::ControlModifier }, 0xAF },
        { { QEvent::KeyPress, Qt::Key_Question, Qt::ShiftModifier }, 0xBF },
        { { QEvent::KeyPress, Qt::Key_Question, Qt::ControlModifier |
                                               Qt::ShiftModifier }, 0xBF },

        { { QEvent::KeyPress, Qt::Key_A, Qt::NoModifier }, 0xC1 },
        { { QEvent::KeyPress, Qt::Key_A, Qt::ControlModifier }, 0x81 },
        { { QEvent::KeyPress, Qt::Key_A, Qt::ShiftModifier }, 0xC1 },
        { { QEvent::KeyPress, Qt::Key_A, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x81 },

        { { QEvent::KeyPress, Qt::Key_B, Qt::NoModifier }, 0xC2 },
        { { QEvent::KeyPress, Qt::Key_B, Qt::ControlModifier }, 0x82 },
        { { QEvent::KeyPress, Qt::Key_B, Qt::ShiftModifier }, 0xC2 },
        { { QEvent::KeyPress, Qt::Key_B, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x82 },

        { { QEvent::KeyPress, Qt::Key_C, Qt::NoModifier }, 0xC3 },
        { { QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier }, 0x83 },
        { { QEvent::KeyPress, Qt::Key_C, Qt::ShiftModifier }, 0xC3 },
        { { QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x83 },

        { { QEvent::KeyPress, Qt::Key_D, Qt::NoModifier }, 0xC4 },
        { { QEvent::KeyPress, Qt::Key_D, Qt::ControlModifier }, 0x84 },
        { { QEvent::KeyPress, Qt::Key_D, Qt::ShiftModifier }, 0xC4 },
        { { QEvent::KeyPress, Qt::Key_D, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x84 },

        { { QEvent::KeyPress, Qt::Key_E, Qt::NoModifier }, 0xC5 },
        { { QEvent::KeyPress, Qt::Key_E, Qt::ControlModifier }, 0x85 },
        { { QEvent::KeyPress, Qt::Key_E, Qt::ShiftModifier }, 0xC5 },
        { { QEvent::KeyPress, Qt::Key_E, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x85 },

        { { QEvent::KeyPress, Qt::Key_F, Qt::NoModifier }, 0xC6 },
        { { QEvent::KeyPress, Qt::Key_F, Qt::ControlModifier }, 0x86 },
        { { QEvent::KeyPress, Qt::Key_F, Qt::ShiftModifier }, 0xC6 },
        { { QEvent::KeyPress, Qt::Key_F, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x86 },

        { { QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier }, 0x8D },
        { { QEvent::KeyPress, Qt::Key_Return, Qt::ControlModifier }, 0x8D },
        { { QEvent::KeyPress, Qt::Key_Return, Qt::ShiftModifier }, 0x8D },
        { { QEvent::KeyPress, Qt::Key_Return, Qt::ControlModifier |
                                             Qt::ShiftModifier }, 0x8D },

        { { QEvent::KeyPress, Qt::Key_G, Qt::NoModifier }, 0xC7 },
        { { QEvent::KeyPress, Qt::Key_G, Qt::ControlModifier }, 0x87 },
        { { QEvent::KeyPress, Qt::Key_G, Qt::ShiftModifier }, 0xC7 },
        { { QEvent::KeyPress, Qt::Key_G, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x87 },

        { { QEvent::KeyPress, Qt::Key_H, Qt::NoModifier }, 0xC8 },
        { { QEvent::KeyPress, Qt::Key_H, Qt::ControlModifier }, 0x88 },
        { { QEvent::KeyPress, Qt::Key_H, Qt::ShiftModifier }, 0xC8 },
        { { QEvent::KeyPress, Qt::Key_H, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x88 },

        { { QEvent::KeyPress, Qt::Key_I, Qt::NoModifier }, 0xC9 },
        { { QEvent::KeyPress, Qt::Key_I, Qt::ControlModifier }, 0x89 },
        { { QEvent::KeyPress, Qt::Key_I, Qt::ShiftModifier }, 0xC9 },
        { { QEvent::KeyPress, Qt::Key_I, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x89 },

        { { QEvent::KeyPress, Qt::Key_J, Qt::NoModifier }, 0xCA },
        { { QEvent::KeyPress, Qt::Key_J, Qt::ControlModifier }, 0x8A },
        { { QEvent::KeyPress, Qt::Key_J, Qt::ShiftModifier }, 0xCA },
        { { QEvent::KeyPress, Qt::Key_J, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x8A },

        { { QEvent::KeyPress, Qt::Key_K, Qt::NoModifier }, 0xCB },
        { { QEvent::KeyPress, Qt::Key_K, Qt::ControlModifier }, 0x8B },
        { { QEvent::KeyPress, Qt::Key_K, Qt::ShiftModifier }, 0xCB },
        { { QEvent::KeyPress, Qt::Key_K, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x8B },

        { { QEvent::KeyPress, Qt::Key_L, Qt::NoModifier }, 0xCC },
        { { QEvent::KeyPress, Qt::Key_L, Qt::ControlModifier }, 0x8C },
        { { QEvent::KeyPress, Qt::Key_L, Qt::ShiftModifier }, 0xCC },
        { { QEvent::KeyPress, Qt::Key_L, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x8C },

        { { QEvent::KeyPress, Qt::Key_M, Qt::NoModifier }, 0xCD },
        { { QEvent::KeyPress, Qt::Key_M, Qt::ControlModifier }, 0x8D },
        { { QEvent::KeyPress, Qt::Key_M, Qt::ShiftModifier }, 0xDD },
        { { QEvent::KeyPress, Qt::Key_M, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x9D },

        { { QEvent::KeyPress, Qt::Key_N, Qt::NoModifier }, 0xCE },
        { { QEvent::KeyPress, Qt::Key_N, Qt::ControlModifier }, 0x8E },
        { { QEvent::KeyPress, Qt::Key_AsciiCircum, Qt::ShiftModifier }, 0xDE },
        { { QEvent::KeyPress, Qt::Key_AsciiCircum, Qt::ControlModifier |
                                                   Qt::ShiftModifier }, 0x9E },

        { { QEvent::KeyPress, Qt::Key_O, Qt::NoModifier }, 0xCF },
        { { QEvent::KeyPress, Qt::Key_O, Qt::ControlModifier }, 0x8F },
        { { QEvent::KeyPress, Qt::Key_O, Qt::ShiftModifier }, 0xCF },
        { { QEvent::KeyPress, Qt::Key_O, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x8F },

        { { QEvent::KeyPress, Qt::Key_P, Qt::NoModifier }, 0xD0 },
        { { QEvent::KeyPress, Qt::Key_P, Qt::ControlModifier }, 0x90 },
        { { QEvent::KeyPress, Qt::Key_At, Qt::ShiftModifier }, 0xC0 },
        { { QEvent::KeyPress, Qt::Key_At, Qt::ControlModifier |
                                          Qt::ShiftModifier }, 0x80 },

        { { QEvent::KeyPress, Qt::Key_Q, Qt::NoModifier }, 0xD1 },
        { { QEvent::KeyPress, Qt::Key_Q, Qt::ControlModifier }, 0x91 },
        { { QEvent::KeyPress, Qt::Key_Q, Qt::ShiftModifier }, 0xD1 },
        { { QEvent::KeyPress, Qt::Key_Q, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x91 },

        { { QEvent::KeyPress, Qt::Key_R, Qt::NoModifier }, 0xD2 },
        { { QEvent::KeyPress, Qt::Key_R, Qt::ControlModifier }, 0x92 },
        { { QEvent::KeyPress, Qt::Key_R, Qt::ShiftModifier }, 0xD2 },
        { { QEvent::KeyPress, Qt::Key_R, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x92 },

        { { QEvent::KeyPress, Qt::Key_S, Qt::NoModifier }, 0xD3 },
        { { QEvent::KeyPress, Qt::Key_S, Qt::ControlModifier }, 0x93 },
        { { QEvent::KeyPress, Qt::Key_S, Qt::ShiftModifier }, 0xD3 },
        { { QEvent::KeyPress, Qt::Key_S, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x93 },

        { { QEvent::KeyPress, Qt::Key_T, Qt::NoModifier }, 0xD4 },
        { { QEvent::KeyPress, Qt::Key_T, Qt::ControlModifier }, 0x94 },
        { { QEvent::KeyPress, Qt::Key_T, Qt::ShiftModifier }, 0xD4 },
        { { QEvent::KeyPress, Qt::Key_T, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x94 },

        { { QEvent::KeyPress, Qt::Key_U, Qt::NoModifier }, 0xD5 },
        { { QEvent::KeyPress, Qt::Key_U, Qt::ControlModifier }, 0x95 },
        { { QEvent::KeyPress, Qt::Key_U, Qt::ShiftModifier }, 0xD5 },
        { { QEvent::KeyPress, Qt::Key_U, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x95 },

        { { QEvent::KeyPress, Qt::Key_V, Qt::NoModifier }, 0xD6 },
        { { QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier }, 0x96 },
        { { QEvent::KeyPress, Qt::Key_V, Qt::ShiftModifier }, 0xD6 },
        { { QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x96 },

        { { QEvent::KeyPress, Qt::Key_W, Qt::NoModifier }, 0xD7 },
        { { QEvent::KeyPress, Qt::Key_W, Qt::ControlModifier }, 0x97 },
        { { QEvent::KeyPress, Qt::Key_W, Qt::ShiftModifier }, 0xD7 },
        { { QEvent::KeyPress, Qt::Key_W, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x97 },

        { { QEvent::KeyPress, Qt::Key_X, Qt::NoModifier }, 0xD8 },
        { { QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier }, 0x98 },
        { { QEvent::KeyPress, Qt::Key_X, Qt::ShiftModifier }, 0xD8 },
        { { QEvent::KeyPress, Qt::Key_X, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x98 },

        { { QEvent::KeyPress, Qt::Key_Y, Qt::NoModifier }, 0xD9 },
        { { QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier }, 0x99 },
        { { QEvent::KeyPress, Qt::Key_Y, Qt::ShiftModifier }, 0xD9 },
        { { QEvent::KeyPress, Qt::Key_Y, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x99 },

        { { QEvent::KeyPress, Qt::Key_Z, Qt::NoModifier }, 0xDA },
        { { QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier }, 0x9A },
        { { QEvent::KeyPress, Qt::Key_Z, Qt::ShiftModifier }, 0xDA },
        { { QEvent::KeyPress, Qt::Key_Z, Qt::ControlModifier |
                                         Qt::ShiftModifier }, 0x9A },

        { { QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier }, 0x95 },
        { { QEvent::KeyPress, Qt::Key_Right, Qt::ControlModifier }, 0x95 },
        { { QEvent::KeyPress, Qt::Key_Right, Qt::ShiftModifier }, 0x95 },
        { { QEvent::KeyPress, Qt::Key_Right, Qt::ControlModifier |
                                             Qt::ShiftModifier }, 0x95 },

        { { QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier }, 0x88 },
        { { QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier }, 0x88 },
        { { QEvent::KeyPress, Qt::Key_Left, Qt::ControlModifier }, 0x88 },
        { { QEvent::KeyPress, Qt::Key_Left, Qt::ShiftModifier }, 0x88 },
        { { QEvent::KeyPress, Qt::Key_Left, Qt::ControlModifier |
                                            Qt::ShiftModifier }, 0x88 },

        { { QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier }, 0x9B },
        { { QEvent::KeyPress, Qt::Key_Escape, Qt::ControlModifier }, 0x9B },
        { { QEvent::KeyPress, Qt::Key_Escape, Qt::ShiftModifier }, 0x9B },
        { { QEvent::KeyPress, Qt::Key_Escape, Qt::ControlModifier |
                                              Qt::ShiftModifier }, 0x9B }
    };
};

#endif // KEYBOARD_H
