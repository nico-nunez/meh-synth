#ifndef SYNTH_UTILS
#define SYNTH_UTILS

#include "synth/Engine.h"
#include "synth/Oscillator.h"
#include "utils/Waveform.h"

#include <string>
#include <vector>

namespace SynthUtils {
void generateSineValues(std::vector<int16_t> &samples, double frequency,
                        const int duration, const int sampleRate);

float semitoneToFrequency(int semitones);

float midiToFrequency(int midiValue);

int noteNameToMidi(std::string_view noteName);

float noteNameToFrequency(std::string_view noteName);

Synth::NoteEvent createNoteEvent(float frequency);

} // namespace SynthUtils

#endif
