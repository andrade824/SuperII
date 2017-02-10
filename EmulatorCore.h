#ifndef EMULATORCORE_H
#define EMULATORCORE_H

#include "Cpu.h"
#include "Memory.h"
#include "SystemBus.h"
#include "Video.h"

class EmulatorCore
{
public:
    /**
     * Size of the ROM in bytes.
     */
    static constexpr uint32_t ROM_SIZE = 12288;

public:
    EmulatorCore();

    void LoadRom(uint8_t data[ROM_SIZE]);

    void RunFrame(int FPS);

    Video* GetVideo() const;

private:
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
     * The number of extra cycles ran after each CPU execution. These will be
     * subtracted from the next amount of CPU cycles that are run.
     */
    uint32_t _leftover_cycles;
};

#endif // EMULATORCORE_H
