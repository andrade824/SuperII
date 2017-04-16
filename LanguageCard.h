#ifndef LANGUAGECARD_H
#define LANGUAGECARD_H

#include "IMemoryMapped.h"
#include "IState.h"

#include <stdint.h>

class LanguageCard : public IMemoryMapped, public IState
{
public:
    /**
     * Size of the ROM in bytes.
     */
    static constexpr uint32_t ROM_SIZE = 12288;

    /**
     * Inclusive start and end addresses for the ROM/Extra RAM.
     */
    static constexpr uint16_t ROM_START = 0xD000;
    static constexpr uint16_t ROM_END = 0xFFFF;

    /**
     * Inclusive start and end addresses for the Language Card control
     * addresses.
     */
    static constexpr uint16_t LANG_CARD_START = 0xC080;
    static constexpr uint16_t LANG_CARD_END = 0xC08F;

public:
    explicit LanguageCard();

    LanguageCard(const LanguageCard &copy) = delete;
    LanguageCard & operator=(const LanguageCard &rhs) = delete;

    void Reset();

    uint8_t Read(uint16_t addr, bool no_side_fx = false) override;
    void Write(uint16_t addr, uint8_t data) override;

    void SaveState(std::ofstream &output) override;
    void LoadState(std::ifstream &input) override;

private:
    uint8_t handle_control(uint16_t addr);

private:
    /**
     * Flags used to describe the internal state of the language card.
     */
    enum StatusFlag {
        /**
         * When bank 1 is selected, this bit will be high. When bank 2 is
         * selected, this bit will be low.
         */
        BANK_SELECT = 0x1,

        /**
         * When the RAM is read-enabled, this bit will be high. When the ROM
         * is read-enabled, this bit will be low.
         */
        READ_ENABLE = 0x2,

        /**
         * When RAM is write-enabled, this bit wil be high.
         */
        WRITE_ENABLE = 0x4,

        /**
         * To protect RAM from accidental write-enable, two consecutive
         * accesses to a write-enable control address are required. When the
         * first access has been done, this flag is set. When the second
         * access occurs, the write-enable flag is set, and this flag is set
         * low.
         */
        NEXT = 0x8
    };

    /**
     * 8K of non-bank-switched memory that is always available to read/write.
     */
    uint8_t _ram_static[8192];

    /**
     * First bank of bank-switchable 4K memory.
     */
    uint8_t _ram_bank1[4096];

    /**
     * Second bank of bank-switchable 4K memory.
     */
    uint8_t _ram_bank2[4096];

    /**
     * Status of the language card internals.
     */
    uint8_t _status;
};

#endif // LANGUAGECARD_H
