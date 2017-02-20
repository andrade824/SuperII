#ifndef SPEAKER_H
#define SPEAKER_H

#include "IMemoryMapped.h"

#include <QAudioFormat>

class Speaker : public IMemoryMapped
{
public:
    Speaker();

    uint8_t Read(uint16_t addr);
    void Write(uint16_t addr, uint8_t);

private:
    void emit_click();

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
     * A buffer to store the samples for a "click" noise.
     */
    quint16 _samples[SAMPLE_RATE];

    /**
     * Data structure to describe the audio output. This will be a standard
     * 44.1KHz, 16-bit audio.
     */
    QAudioFormat _format;
};

#endif // SPEAKER_H
