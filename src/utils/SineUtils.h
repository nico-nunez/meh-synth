#include "synth/Oscillator.h"
#include <cstdint>
#include <vector>

#ifndef SINE_UTILS_H
#define SINE_UTILS_H

namespace {
constexpr float DEFAULT_AMPLITUDE{0.5f};
}

namespace SineUtils {

void generateSineValues(std::vector<int16_t> &samples, double frequency,
                        const int duration, const int sampleRate);

float getHertzFromSemitoneOffset(int semitones);

void playOscillator(std::vector<int16_t> &samples, Synth::Oscillator &osc,
                    int duration, float amplitude = DEFAULT_AMPLITUDE);

void playOscChord(std::vector<int16_t> &samples,
                  std::vector<Synth::Oscillator> &oscillators, int duration,
                  float amplitude = DEFAULT_AMPLITUDE);

} // namespace SineUtils
#endif
