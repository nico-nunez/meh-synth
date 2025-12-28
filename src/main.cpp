#include "synth/Engine.h"
#include "utils/SynthUtils.h"
#include "utils/WavWriter.h"

#include <cassert>
#include <cstddef>
#include <vector>

int main() {
  // Audio parameters
  const int SAMPLE_RATE = 44100;    // CD quality: 44,100 samples per second
  const int DURATION_SECONDS = 4.0; // Length of audio

  std::vector<std::vector<std::string>> noteSequence{
      {"C4", "D#4", "G4"}, {"F4", "G#4", "C5"}, {"G4", "A#5", "D5"}};
  // std::vector<std::vector<std::string>> noteSequence{{"C4"}, {"D#4"},
  // {"G4"}};

  Synth::Sequence noteEventSequence{};

  for (const auto &noteGroup : noteSequence) {
    Synth::NoteEventGroup eventGroup{};

    for (const auto &note : noteGroup) {
      float noteFrequency{SynthUtils::noteNameToFrequency(note)};
      eventGroup.push_back(SynthUtils::createNoteEvent(noteFrequency));
    }
    noteEventSequence.push_back(eventGroup);
  }

  // Generate waveform samples
  // Audio is just an array of values representing air pressure over time
  // Values: floats (0.0 - 1.0)
  Synth::Engine synthEngine{SAMPLE_RATE, Synth::OscillatorType::Square};

  auto audioBuffer{synthEngine.process(noteEventSequence, DURATION_SECONDS)};

  int32_t fileSampleRate{static_cast<int32_t>(SAMPLE_RATE)};

  WavWriter::writeWavFile("output.wav", audioBuffer, fileSampleRate);

  return 0;
}
