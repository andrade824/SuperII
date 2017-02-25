#ifndef SPEAKER_H
#define SPEAKER_H

#include "Cpu.h"
#include "IMemoryMapped.h"
#include "IState.h"

#include <QAudioOutput>
#include <QAudioFormat>
#include <QIODevice>

#include <fstream>
#include <queue>

class Speaker : public IMemoryMapped, public IState
{
public:
    Speaker(Cpu &cpu);

    void Reset();

    void PlayAudio(uint32_t num_cycles);

    uint8_t Read(uint16_t addr) override;
    void Write(uint16_t addr, uint8_t) override;

    void SaveState(std::ofstream &output) override;
    void LoadState(std::ifstream &input) override;

    ~Speaker();

private:
    /**
     * Start and end addresses (inclusive) for the Keyboard registers.
     */
    static constexpr int SPEAKER_START_ADDR = 0xC030;
    static constexpr int SPEAKER_END_ADDR = 0xC030;


    /**
     * The sample rate to output at.
     */
    static constexpr int SAMPLE_RATE = 44100;

    /**
     * Used to retrieve cycle counts whenever a speaker toggle is requested.
     * This is used to know how far apart each toggle should be when the audio
     * samples are being generated.
     */
    Cpu &_cpu;

    /**
     * The cycle count of the CPU the last time audio was requested.
     */
    uint32_t _prev_cycle_count;

    /**
     * A list of CPU cycle counts at which the speaker should be toggled.
     */
    std::queue<uint32_t> _toggle_cycles;

    /**
     * True if the speaker is in a logic high state, false if in a logic low
     * state.
     */
    bool _speaker_state;

    /**
     * Data structure to describe the audio output. This will be standard
     * 44.1KHz, 16-bit audio.
     */
    QAudioFormat _format;

    /**
     * The Audio output device that will be playing the audio.
     */
    QAudioOutput *_output;

    /**
     * Samples are written to this I/O device before they get played.
     */
    QIODevice *_audio_io;
};

#endif // SPEAKER_H
