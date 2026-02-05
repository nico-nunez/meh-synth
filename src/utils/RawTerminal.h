#pragma once

#include <cstdint>

namespace utils {
struct NoteEventQueue;

void enableRawTerminal();
void disableRawTerminal();

uint8_t asciiToMidi(char key);
void captureKeyboardInputs(NoteEventQueue &eventQueue);
} // namespace utils
