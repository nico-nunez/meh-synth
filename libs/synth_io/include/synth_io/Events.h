#pragma once

#include <cstdint>

namespace synth_io {
enum class NoteEventType { NoteOff, NoteOn };

struct NoteEvent {
  NoteEventType type = NoteEventType::NoteOff;
  uint8_t midiNote = 0;
  uint8_t velocity = 0;
};

struct ParamEvent {
  uint8_t id = 0;
  float value = 0.0f; // Normalized [0, 1]
};

} // namespace synth_io
