#include "SynthUtils.h"
#include "synth/Engine.h"

#include <cctype>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace SynthUtils {
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

/* Semitone ratio is 2^(1/12). This is multiplied by the number
 * of semitones from A4 for the note that's to be calculated.
 * This value (ratio * semitones) is multiplied by 440hz (A4)
 * to get the value of the desired note in hertz.
 *
 * Example: C4 = 440hz * (2^((1/12) * -9)), where C4 is -9 semitones from A4
 */
float semitoneToFrequency(int semitones) {
  return 440.0f * static_cast<float>(std::pow(2.0, (1.0 / 12.0) * semitones));
}

float midiToFrequency(int midiValue) {
  return semitoneToFrequency(midiValue - 69);
}

int noteNameToMidi(std::string_view noteName) {
  if (noteName.empty())
    throw std::invalid_argument("Empty note name");

  // Map note characters to semitone offsets from 'C'
  static const std::unordered_map<char, int> noteOffests{
      {'C', 0}, {'D', 2}, {'E', 4}, {'F', 5}, {'G', 7}, {'A', 9}, {'B', 11}};

  size_t index = 0;

  // Get first character then increment index
  char noteLetter = static_cast<char>(std::toupper(noteName[index++]));

  // Ensure valid note (A-G)
  if (noteOffests.find(noteLetter) == noteOffests.end()) {
    throw std::invalid_argument("Invalid note letter: " +
                                std::string(1, noteLetter));
  }

  int noteSemitone = noteOffests.at(noteLetter);

  // Check if sharp ('#') or flat ('b')
  if (index < noteName.length()) {
    if (noteName[index] == '#') {
      noteSemitone++;
      index++;
    } else if (noteName[index] == 'b') {
      noteSemitone--;
      index++;
    }
  }

  // Get octive value (e.g., 0, 1, 2, etc.)
  if (index >= noteName.length())
    throw std::invalid_argument("Missing octave number");

  int noteOctive = noteName[index] - '0';
  if (noteOctive < 0 || noteOctive > 9)
    throw std::invalid_argument("Note octave out of range (0-9): " +
                                std::to_string(noteOctive));

  int midiNote = (noteOctive + 1) * 12 + noteSemitone;
  if (midiNote < 0 || midiNote > 127)
    throw std::invalid_argument("MIDI Note out of range (0-127): " +
                                std::to_string(midiNote));
  return midiNote;
}

float noteNameToFrequency(std::string_view noteName) {
  int midiNote{noteNameToMidi(noteName)};
  return midiToFrequency(midiNote);
}

Synth::NoteEvent createNoteEvent(const float frequency) {
  Synth::NoteEvent note;
  note.frequency = frequency;
  return note;
}

} // namespace SynthUtils
