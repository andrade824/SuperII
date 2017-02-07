#ifndef EMULATORCORE_H
#define EMULATORCORE_H

#include "Cpu.h"
#include "Memory.h"
#include "SystemBus.h"
#include "Video.h"

class EmulatorCore
{
public:
    EmulatorCore();

    void Run(uint32_t num_cycles);
    void RunFrame(float FPS);

    void UpdateVideo();

    Video* GetVideo() const;

private:
    SystemBus _bus;
    Cpu _cpu;
    Memory _mem;
    Memory _rom;
    Video *_video;
    uint32_t _leftover_cycles;
};

#endif // EMULATORCORE_H
