#include "myutils.h"
#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

int main() {
  // Audio parameters
  const int SAMPLE_RATE = 44100;  // CD quality: 44,100 samples per second
  const int DURATION_SECONDS = 2; // Length of audio
                                  //
  // Generate 3 notes(hz) -> C4, E4-flat, G4 (Cminor triad)
  std::vector<double> notesToGenerate{
      audioutils::getHertzFromSemitoneOffset(-9),
      audioutils::getHertzFromSemitoneOffset(-6),
      audioutils::getHertzFromSemitoneOffset(-2)};

  assert(notesToGenerate.size() <= INT_MAX);

  // Calculate total number of samples
  const int32_t TOTAL_SAMPLES =
      SAMPLE_RATE * DURATION_SECONDS * static_cast<int>(notesToGenerate.size());

  // Generate sine wave samples
  // Audio is just an array of numbers representing air pressure over time
  std::vector<int16_t> samples;

  assert(TOTAL_SAMPLES > 0);
  samples.reserve(static_cast<size_t>(TOTAL_SAMPLES));

  std::ofstream wavFile{audioutils::createWavFile()};

  for (auto frequency : notesToGenerate)
    audioutils::generateSineValues(samples, frequency, DURATION_SECONDS,
                                   SAMPLE_RATE);

  if (!wavFile) {
    std::cerr << "Error: Could not create output.wav\n";
    return 1;
  }

  std::cout << "Writing WAV file...\n";

  audioutils::writeWavMetadata(wavFile, TOTAL_SAMPLES, SAMPLE_RATE);

  // --- DATA CHUNK ---
  // Contains the actual audio samples
  audioutils::writeString(wavFile, "data", 4);

  // Data chunk size (number of samples * bytes per sample)
  audioutils::writeInt32(wavFile, TOTAL_SAMPLES * 2);

  // Write all the audio samples
  for (auto sample : samples) {
    audioutils::writeInt16(wavFile, sample);
  }

  wavFile.close();

  std::cout << "Success! Created output.wav\n";
  std::cout << "Play it with any audio player to hear your sine wave.\n";

  return 0;
}
