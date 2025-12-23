#ifndef SYNTH_UTILS
#define SYNTH_UTILS

#include "synth/Engine.h"
#include "synth/Oscillator.h"
#include "utils/Waveform.h"

#include <vector>

namespace SynthUtils {
void generateSineValues(std::vector<int16_t> &samples, double frequency,
                        const int duration, const int sampleRate);

float getHertzFromSemitoneOffset(int semitones);

} // namespace SynthUtils

#endif
