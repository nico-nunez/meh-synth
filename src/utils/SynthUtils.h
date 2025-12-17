#ifndef SYNTH_UTILS
#define SYNTH_UTILS

#include "synth/Oscillator.h"
#include "utils/AudioUtils.h"
#include "utils/Waveform.h"
#include <vector>

namespace SynthUtils {
void generateSineValues(std::vector<int16_t> &samples, double frequency,
                        const int duration, const int sampleRate);

float getHertzFromSemitoneOffset(int semitones);

AudioUtils::Sequence
getSequenceFromFreqs(AudioUtils::FreqSequence &freqSquence,
                     Synth::OscType oscType,
                     float sampleRate = Synth::DEFAULT_SAMPLE_RATE);

} // namespace SynthUtils

#endif
