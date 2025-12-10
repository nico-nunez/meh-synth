#include "myutils.h"
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>

namespace audioutils {

// Create WAV file
std::ofstream createWavFile(const std::string &filename) {
  std::ofstream wavFile(filename, std::ios::binary);
  return wavFile;
}

// WAV file format uses "chunks" - blocks of data with a 4-byte ID and size
// The main structure is: RIFF chunk -> fmt chunk -> data chunk

void writeString(std::ofstream &file, const char *str, int32_t length) {
  // Write a fixed-length string to the file
  file.write(str, length);
}

void writeInt32(std::ofstream &file, int32_t value) {
  assert(value <= INT_MAX);

  // WAV format uses little-endian (least significant byte first)
  file.write(reinterpret_cast<const char *>(&value), 4);
}

void writeInt16(std::ofstream &file, int16_t value) {
  file.write(reinterpret_cast<const char *>(&value), 2);
}

void writeWavMetadata(std::ofstream &file, int32_t numSamples,
                      int32_t sampleRate) {
  // WAV files have a specific structure. Let's build it piece by piece:

  // --- RIFF HEADER ---
  // This identifies the file as a RIFF file (Resource Interchange File
  // Format)
  writeString(file, "RIFF", 4);

  // File size minus 8 bytes (for "RIFF" and this size field itself)
  int32_t fileSize =
      36 + (numSamples * 2); // 36 = header size, 2 = bytes per sample
  writeInt32(file, fileSize);

  // WAVE format identifier
  writeString(file, "WAVE", 4);

  // --- FORMAT CHUNK ---
  // Describes the audio format
  writeString(file, "fmt ", 4); // Note the space after "fmt"

  // Format chunk size (16 bytes for PCM)
  writeInt32(file, 16);

  // Audio format (1 = PCM, uncompressed)
  writeInt16(file, 1);

  // Number of channels (1 = mono, 2 = stereo)
  writeInt16(file, 1);

  // Sample rate (samples per second)
  writeInt32(file, sampleRate);

  // Byte rate (sample rate * channels * bytes per sample)
  writeInt32(file, sampleRate * 1 * 2);

  // Block align (channels * bytes per sample)
  writeInt16(file, 1 * 2);

  // Bits per sample
  writeInt16(file, 16);
}

/* Semitone ratio is 2^(1/12). This is multiplied by the number
 * of semitones from A4 for the note that's to be calculated.
 * This value (ratio * semitones) is multiplied by 440hz (A4)
 * to get the value of the desired note in hertz.
 *
 * Example: C4 = 440hz * (2^((1/12) * -9)), where C4 is -9 semitones from A4
 */
double getHertzFromSemitoneOffset(int semitones) {
  return 440.0 * std::pow(2.0, (1.0 / 12.0) * semitones);
}

void generateSineValues(std::vector<int16_t> &samples, double frequency,
                        const int duration, const int sampleRate) {
  // Volume (0.0 to 1.0, keeping it at 50% to avoid clipping)
  const double AMPLITUDE = 0.5;
  const int32_t numSamples = sampleRate * duration;

  std::cout << "Generating " << duration << " seconds of " << frequency
            << "HZ sine wave...\n";

  for (int32_t i = 0; i < numSamples; ++i) {
    // The sine wave formula: sin(2 * π * frequency * time)
    // time = i / SAMPLE_RATE (current sample number divided by samples per
    // second)
    double time = static_cast<double>(i) / sampleRate;
    double value = AMPLITUDE * std::sin(2.0 * M_PI * frequency * time);

    // Convert from floating point (-1.0 to 1.0) to 16-bit integer (-32768 to
    // 32767) This is how audio is stored in WAV files
    int16_t sample = static_cast<int16_t>(value * 32767.0);
    samples.push_back(sample);
  }
}

} // namespace audioutils
