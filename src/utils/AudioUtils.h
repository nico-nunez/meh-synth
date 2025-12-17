#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include "synth/Oscillator.h"
#include <cstdint>
#include <vector>

namespace AudioUtils {
inline constexpr float DEFAULT_AMPLITUDE{0.5f};

using Sequence = std::vector<Synth::OscillatorGroup>;

using FreqGroup = std::vector<float>;
using FreqSequence = std::vector<FreqGroup>;

void playOscillator(std::vector<int16_t> &samples, Synth::Oscillator &osc,
                    int duration, float amplitude = DEFAULT_AMPLITUDE);

void renderSequence(std::vector<int16_t> &samples, Sequence &sequence,
                    int duration, float amplitude = DEFAULT_AMPLITUDE);

} // namespace AudioUtils
#endif
