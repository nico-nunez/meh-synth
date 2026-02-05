#ifndef SYNTH_UTILS
#define SYNTH_UTILS

#include <string_view>

namespace SynthUtils {
// Root note == A4
inline constexpr int ROOT_NOTE_MIDI{69};
inline constexpr float ROOT_NOTE_FREQ{440.0f};

float semitoneToFrequency(int semitones);

float midiToFrequency(int midiValue);

int noteNameToMidi(std::string_view noteName);

float noteNameToFrequency(std::string_view noteName);

} // namespace SynthUtils

#endif
