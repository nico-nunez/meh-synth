#pragma once

namespace device_io {
struct NoteEventQueue;

void enableRawTerminal();
void disableRawTerminal();

// TODO(nico): this should probably be a callback with void* userContext
void captureKeyboardInputs();
} // namespace device_io
