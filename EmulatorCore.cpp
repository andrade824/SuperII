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
    _leftover_cycles(0)
{
    _bus.Register(&_mem);
    _bus.Register(&_rom);
    _bus.Register(_video);
}

/**
 * Run for one video frame (for 60FPS this is 16.667ms).
 *
 * This involves checking SFML events to grab keyboard input, running for one
 * frame's worth of CPU cycles, and updating the video output.
 *
 * @param FPS How many frames per second to run at.
 */
void EmulatorCore::RunFrame(float FPS)
{
    /**
     * Calculate how many CPU cycles are to be executed in one frame.
     *
     * The standard Apple II CPU frequency is 1.023MHz.
     */
    constexpr uint32_t CPU_FREQ = 1023000;
    const uint32_t CYCLES_IN_FRAME = static_cast<uint32_t>((1.0f / FPS) / (1.0f / CPU_FREQ));

    Run(CYCLES_IN_FRAME);

    UpdateVideo();
}

/**
 * Run for a certain amount of CPU cycles compensating for any extra cycles that
 * ran last time.
 *
 * @param num_cycles The number of cycles to run on the CPU.
 */
void EmulatorCore::Run(uint32_t num_cycles)
{
    _leftover_cycles = _cpu.Execute(num_cycles - _leftover_cycles) - num_cycles;
}

/**
 * Force the video module to render a new frame.
 */
void EmulatorCore::UpdateVideo()
{
    _video->repaint();
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
