/**
 * Emulates a simple 8ohm speaker. When a read/write is performed on this
 * module, a small click noise is generated. Programs create tones by producing
 * these clicks in a certain frequency.
 */
#include "Speaker.h"

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioOutput>

/**
 * Constructor.
 */
Speaker::Speaker() :
    IMemoryMapped(SPEAKER_START_ADDR, SPEAKER_END_ADDR),
    _samples(),
    _format()
{
    _format.setSampleRate(SAMPLE_RATE);
    _format.setChannelCount(1);
    _format.setSampleSize(16);
    _format.setCodec("audio/pcm");
    _format.setByteOrder(QAudioFormat::LittleEndian);
    _format.setSampleType(QAudioFormat::UnSignedInt);

    for(quint16 i = 0; i < SAMPLE_RATE; ++i)
        _samples[i] = i + (0xFFFF - SAMPLE_RATE);
}

/**
 * Emit a click when the speaker's address is referenced.
 *
 * @param addr The address to read from.
 *
 * @return Always returns 0.
 */
uint8_t Speaker::Read(uint16_t addr)
{
    if(addr == SPEAKER_START_ADDR)
        emit_click();

    return 0;
}

/**
 * Emit a click when the speaker's address is referenced.
 *
 * @param addr The address to read from.
 */
void Speaker::Write(uint16_t addr, uint8_t)
{
    if(addr == SPEAKER_START_ADDR)
        emit_click();
}

/**
 * Output a single click noise.
 */
void Speaker::emit_click()
{
    QAudioOutput output(_format);

    QIODevice *io = output.start();
    io->write(reinterpret_cast<char*>(_samples), 2 * 2);

    output.stop();
}
