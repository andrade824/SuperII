#ifndef EMULATORCORE_H
#define EMULATORCORE_H

#include "Cpu.h"
#include "DiskController.h"
#include "IState.h"
#include "Keyboard.h"
#include "LanguageCard.h"
#include "Memory.h"
#include "Speaker.h"
#include "SystemBus.h"
#include "Video.h"

#include <QColor>
#include <QKeyEvent>

#include <fstream>
#include <string>

class EmulatorCore
{
public:
    EmulatorCore();

    void SetPaused(bool pause);
    bool GetPaused() const;

    void ResetCpu();
    void PowerCycle();

    CpuContext GetCpuContext() const;

    void LoadDisk(std::string filename,
                  DiskController::DriveId drive,
                  uint8_t data[DiskDrive::DISK_SIZE]);
    void UnloadDisk(DiskController::DriveId drive);
    std::string GetDiskFilename(DiskController::DriveId drive) const;
    bool GetDiskBusy();

    void RunFrame(int FPS);
    void SingleStep();

    uint16_t GetBpAddr() const;
    void SetBpAddr(uint16_t addr);

    bool GetBpEnabled() const;
    void SetBpEnabled(bool enabled);

    QColor GetVideoTextColor() const;
    void SetVideoTextColor(QColor color);

    bool GetSpeakerMute() const;
    void SetSpeakerMute(bool mute);

    key_mappings GetMappings();
    void SetMappings(key_mappings key_map);

    void GetMemory(QByteArray &mem, uint16_t start, uint16_t end);

    void SetTurbo(uint8_t turbo);
    uint8_t GetTurbo() const;

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
     * Language Card add-on that contains the 12KB Applesoft ROM and 16KB of
     * extra RAM.
     */
    LanguageCard _lang_card;

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
     * Disk II controller card.
     */
    DiskController _disk_ctrl;

    /**
     * The number of extra cycles ran after each CPU execution. These will be
     * subtracted from the next amount of CPU cycles that are run.
     */
    uint32_t _leftover_cycles;

    /**
     * True if the emulator is paused, false otherwise.
     */
    bool _paused;

    /**
     * Multiplier used to increase the number of CPU cycles executed in each
     * frame.
     */
    uint8_t _turbo;
};

#endif // EMULATORCORE_H
