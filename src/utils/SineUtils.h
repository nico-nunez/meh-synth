#include <cstdint>
#include <vector>

#ifndef SINE_UTILS_H
#define SINE_UTILS_H

namespace SineUtils {

void generateSineValues(std::vector<int16_t> &samples, double frequency,
                        const int duration, const int sampleRate);

double getHertzFromSemitoneOffset(int semitones);

} // namespace SineUtils
#endif
