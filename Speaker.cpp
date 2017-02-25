/**
 * Emulates a simple 8ohm speaker. When a read/write is performed on this
 * module, a small click noise is generated. Programs create tones by producing
 * these clicks in a certain frequency.
 *
 * But you can't just emit a click every time the speaker module is accessed
 * because of the way the CPU is handled. The CPU runs as fast as it possibly
 * can for one frame's worth of cycles, and then it sleeps for the rest of the
 * frame. If a click was emitted every time the CPU accessed the speaker, the
 * clicks would be emitted at the speed of the computer the emulator is running
 * on and not at Apple II speeds.
 *
 * To make up for this, the number of clicks wanting to be emitted in a single
 * frame is recorded and at the end of every frame, are played back at the
 * correct speed.
 */
#include "Speaker.h"

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioOutput>

#include <queue>

/**
 * Constructor.
 */
Speaker::Speaker(Cpu &cpu) :
    IMemoryMapped(SPEAKER_START_ADDR, SPEAKER_END_ADDR),
    _cpu(cpu),
    _prev_cycle_count(0),
    _toggle_cycles(),
    _speaker_state(false),
    _format(),
    _output(nullptr),
    _audio_io(nullptr)
{
    _format.setSampleRate(SAMPLE_RATE);
    _format.setChannelCount(1);
    _format.setSampleSize(16);
    _format.setCodec("audio/pcm");
    _format.setByteOrder(QAudioFormat::LittleEndian);
    _format.setSampleType(QAudioFormat::SignedInt);

    _output = new QAudioOutput(_format);
    _output->setBufferSize(10000);
    _audio_io = _output->start();
}

/**
 * Reset the speaker state.
 */
void Speaker::Reset()
{
    _prev_cycle_count = 0;
    _speaker_state = false;

    while(!_toggle_cycles.empty())
        _toggle_cycles.pop();
}

/**
 * Play "num_cycles" worth of audio data.
 *
 * @note It is critical that this function be called at the same rate as the CPU
 *       (aka, every frame). If not, the audio will become out of sync with the
 *       CPU.
 *
 * @param num_cycles How much audio to play. This should be identical to how
 *                   many cycles the CPU ran within the last frame.
 */
void Speaker::PlayAudio(uint32_t num_cycles)
{
    constexpr float CYCLES_PER_SAMPLE = 1023000.0f / SAMPLE_RATE;
    const unsigned int num_samples =
            static_cast<unsigned int>(num_cycles / CYCLES_PER_SAMPLE);

    int16_t *samples = new int16_t[num_samples];

    for(unsigned int i = 0; i < num_samples; ++i)
    {
        if(!_toggle_cycles.empty()  &&
           ((i * CYCLES_PER_SAMPLE + _prev_cycle_count) >=
           _toggle_cycles.front()))
        {
            _toggle_cycles.pop();
            _speaker_state = !_speaker_state;
        }


        samples[i] = (_speaker_state) ? 16000 : 0;
    }

    _audio_io->write(reinterpret_cast<char*>(samples), num_samples * 2);

    delete [] samples;

    _prev_cycle_count = _cpu.GetTotalCycles();
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
        _toggle_cycles.push(_cpu.GetTotalCycles());

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
        _toggle_cycles.push(_cpu.GetTotalCycles());
}

/**
 * Save the Speaker state out to a file.
 *
 * @param output The file to write to.
 */
void Speaker::SaveState(std::ofstream &output)
{
    output.write(reinterpret_cast<char*>(&_prev_cycle_count),
                 sizeof(_prev_cycle_count));

    output.write(reinterpret_cast<char*>(&_speaker_state),
                 sizeof(_speaker_state));
}

/**
 * Load the Speaker state out of a file.
 *
 * @param input The file to read from.
 */
void Speaker::LoadState(std::ifstream &input)
{
    input.read(reinterpret_cast<char*>(&_prev_cycle_count),
                 sizeof(_prev_cycle_count));

    input.read(reinterpret_cast<char*>(&_speaker_state),
                 sizeof(_speaker_state));

    /**
     * Ensure any sound data from before the LoadState doesn't play.
     */
    while(!_toggle_cycles.empty())
        _toggle_cycles.pop();
}

/**
 * Destructor.
 */
Speaker::~Speaker()
{
    if(_output->state() == QAudio::ActiveState)
        _output->stop();

    delete _output;
}
