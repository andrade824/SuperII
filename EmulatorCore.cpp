/**
 * Ties all of the different emulator components together and handles any
 * actions the GUI might want to make to affect the emulator's operation.
 */

#include "EmulatorCore.h"
#include "instrs_6502.h"

#include <cstdint>

/**
 * Constructor.
 */
EmulatorCore::EmulatorCore() :
    _bus(),
    _cpu(_bus, instrs_6502),
    _mem(0, 0xBFFF, false),
    _rom(0xD000, 0xFFFF, true),
    _video(new Video(_mem)),
    _keyboard(),
    _speaker(_cpu),
    _leftover_cycles(0)
{
    _bus.Register(&_mem);
    _bus.Register(&_rom);
    _bus.Register(_video);
    _bus.Register(&_keyboard);
    _bus.Register(&_speaker);
}

/**
 * Load data into ROM.
 *
 * @note The size of the Apple II+ ROM is 12KB.
 *
 * @param data The data to load into ROM.
 */
void EmulatorCore::LoadRom(uint8_t data[ROM_SIZE])
{
    _rom.LoadMemory(data, ROM_SIZE);
    _cpu.Reset();
}

/**
 * Run for one video frame (for 60FPS this is 16.667ms).
 *
 * This involves checking SFML events to grab keyboard input, running for one
 * frame's worth of CPU cycles, and updating the video output.
 *
 * @param FPS How many frames per second to run at.
 */
void EmulatorCore::RunFrame(int FPS)
{
    /**
     * Calculate how many CPU cycles are to be executed in one frame.
     *
     * The standard Apple II CPU frequency is 1.023MHz.
     */
    constexpr uint32_t CPU_FREQ = 1023000;
    const uint32_t CYCLES_PER_FRAME = CPU_FREQ / FPS;

    _leftover_cycles = _cpu.Execute(CYCLES_PER_FRAME - _leftover_cycles);

    _video->repaint();

    _speaker.PlayAudio(CYCLES_PER_FRAME - _leftover_cycles);
}

/**
 * Returns back the Video module. This is used by the MainWindow to render the
 * video module.
 *
 * @return The video module.
 */
Video* EmulatorCore::GetVideo() const
{
    return _video;
}

/**
 * Tell the Keyboard module that a key was pressed down.
 *
 * @param key The key that was pressed.
 */
void EmulatorCore::UpdateKeyboardStrobe(const QKeyEvent *key)
{
    _keyboard.UpdateKeyboardStrobe(key);
}
