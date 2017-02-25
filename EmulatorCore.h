#ifndef EMULATORCORE_H
#define EMULATORCORE_H

#include "Cpu.h"
#include "IState.h"
#include "Keyboard.h"
#include "Memory.h"
#include "Speaker.h"
#include "SystemBus.h"
#include "Video.h"

#include <QKeyEvent>

#include <fstream>

class EmulatorCore
{
public:
    /**
     * Size of the ROM in bytes.
     */
    static constexpr uint32_t ROM_SIZE = 12288;

public:
    EmulatorCore();

    void ResetCpu();
    void PowerCycle();

    void LoadRom(uint8_t data[ROM_SIZE]);

    void RunFrame(int FPS);

    bool SaveState(std::ofstream &output);
    bool LoadState(std::ifstream &input);

    Video* GetVideo() const;

    void UpdateKeyboardStrobe(const QKeyEvent *key);

private:
    /**
     * Magic value placed at the beginning of a saved state.
     */
    static constexpr uint32_t STATE_MAGIC = 0xDEADBEEF;

    /**
     * Provides the main access point between all of the components in the
     * emulated system.
     */
    SystemBus _bus;

    /**
     * 6502 CPU.
     */
    Cpu _cpu;

    /**
     * 48K of main memory.
     */
    Memory _mem;

    /**
     * 12K of ROM.
     */
    Memory _rom;

    /**
     * Video module. This is both a QWidget (so QT can render it) and an SFML
     * RenderWindow (so SFML can draw to it).
     */
    Video *_video;

    /**
     * Keyboard module.
     */
    Keyboard _keyboard;

    /**
     * Speaker module.
     */
    Speaker _speaker;

    /**
     * The number of extra cycles ran after each CPU execution. These will be
     * subtracted from the next amount of CPU cycles that are run.
     */
    uint32_t _leftover_cycles;
};

#endif // EMULATORCORE_H
