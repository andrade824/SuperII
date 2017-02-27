/**
 * Ties all of the different emulator components together and handles any
 * actions the GUI might want to make to affect the emulator's operation.
 */

#include "EmulatorCore.h"
#include "instrs_6502.h"

#include <cstdint>
#include <fstream>

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
 * Reset the CPU.
 */
void EmulatorCore::ResetCpu()
{
    _cpu.Reset();
}

/**
 * Reset every module to its default state.
 */
void EmulatorCore::PowerCycle()
{
    _cpu.Reset();
    _mem.Reset();
    _video->Reset();
    _keyboard.Reset();
    _speaker.Reset();

    _leftover_cycles = 0;
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
 * Gets the video module's current text color.
 *
 * @return The video text color.
 */
QColor EmulatorCore::GetVideoTextColor() const
{
    uint32_t color = _video->GetTextColor();
    return QColor::fromRgb(color & 0xFF,
                           (color >> 8) & 0xFF,
                           (color >> 16) & 0xFF);
}

/**
 * Set the video module's text color.
 *
 * @param red The red component.
 * @param green The green component.
 * @param blue The blue component.
 */
void EmulatorCore::SetVideoTextColor(QColor color)
{
    _video->SetTextColor(color.red(), color.green(), color.blue());
}

/**
 * Get the speaker mute state.
 *
 * @return True if the speaker is muted, false otherwise.
 */
bool EmulatorCore::GetSpeakerMute() const
{
    return _speaker.GetMute();
}

/**
 * Set the speaker mute state.
 *
 * @param mute True if the speaker is muted, false otherwise.
 */
void EmulatorCore::SetSpeakerMute(bool mute)
{
    _speaker.SetMute(mute);
}

/**
 * Gets the keyboard mappings.
 *
 * @return The hash table of keyboard mappings.
 */
key_mappings EmulatorCore::GetMappings()
{
    return _keyboard.GetMappings();
}

/**
 * Update the keyboard mappings.
 *
 * @param key_map The new keyboard mappings.
 */
void EmulatorCore::SetMappings(key_mappings key_map)
{
    _keyboard.SetMappings(key_map);
}

/**
 * Save the emulator state out to a file.
 *
 * @param output The file to write to.
 *
 * @return True if writing was successful.
 */
bool EmulatorCore::SaveState(std::ofstream &output)
{
    uint32_t temp_magic = STATE_MAGIC;
    output.write(reinterpret_cast<char*>(&temp_magic), sizeof(temp_magic));

    _cpu.SaveState(output);
    _mem.SaveState(output);
    _rom.SaveState(output);
    _video->SaveState(output);
    _keyboard.SaveState(output);
    _speaker.SaveState(output);

    output.write(reinterpret_cast<char*>(&_leftover_cycles),
                 sizeof(_leftover_cycles));

    return !(!output);
}

/**
 * Load the emulator state out of a file.
 *
 * @param input The fiel to read from.
 *
 * @return  True if reading was successful.
 */
bool EmulatorCore::LoadState(std::ifstream &input)
{
    uint32_t temp_magic = 0;
    input.read(reinterpret_cast<char*>(&temp_magic), sizeof(temp_magic));

    if(temp_magic != STATE_MAGIC)
        return false;

    _cpu.LoadState(input);
    if(!input || input.eof())
        return false;

    _mem.LoadState(input);
    if(!input || input.eof())
        return false;

    _rom.LoadState(input);
    if(!input || input.eof())
        return false;

    _video->LoadState(input);
    if(!input || input.eof())
        return false;

    _keyboard.LoadState(input);
    if(!input || input.eof())
        return false;

    _speaker.LoadState(input);
    if(!input || input.eof())
        return false;

    input.read(reinterpret_cast<char*>(&_leftover_cycles),
                 sizeof(_leftover_cycles));

    return !(!input);
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
