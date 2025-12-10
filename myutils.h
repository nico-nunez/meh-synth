#include <cstdint>
#include <fstream>
#include <string_view>
#include <vector>

#ifndef MY_UTILS_H
#define MY_UTILS_H

namespace audioutils {
// Create WAV file
std::ofstream createWavFile(const std::string &filename = "output.wav");

// Write string to WAV file
void writeString(std::ofstream &file, const char *str, int length);

// Write int 32 to WAV file
void writeInt32(std::ofstream &file, int32_t value);

// Write int 16 to WAV file
void writeInt16(std::ofstream &file, int16_t value);

// Write WAV file metadata
void writeWavMetadata(std::ofstream &file, int32_t numSamples,
                      int32_t sampleRate);

// Calculate Hertz value for desired note base on semitones away from A4
double getHertzFromSemitoneOffset(int semitones);

// Generate sine values for WAV file
void generateSineValues(std::vector<int16_t> &samples, double frequency,
                        const int duration, const int sampleRate);

} // namespace audioutils
#endif
