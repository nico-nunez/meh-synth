#pragma once

#include <cstdint>

namespace synth_io {
struct NoteEventQueue;
struct SynthSession;
} // namespace synth_io

namespace app_input {
using hSynthSession = synth_io::SynthSession *;

int startKeyInputCapture(hSynthSession);

uint8_t asciiToMidi(char key);
} // namespace app_input
