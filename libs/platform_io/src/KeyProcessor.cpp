#include "platform_io/KeyProcessor.h"
#include "device_io/KeyCapture.h"
#include "device_io/MidiCapture.h"
#include "platform_io/NoteEventQueue.h"
#include "utils/Logger.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>

namespace platform_io {
// Handle MIDI device events
static void midiCallback(device_io::MidiEvent midiEvent, void *context) {
  auto queue = static_cast<NoteEventQueue *>(context);

  // TODO(nico): handle more than just note on/off events
  if (midiEvent.type == device_io::MidiEvent::Type::NoteOn ||
      midiEvent.type == device_io::MidiEvent::Type::NoteOff) {

    NoteEvent noteEvent{};
    noteEvent.type = midiEvent.type == device_io::MidiEvent::Type::NoteOn
                         ? NoteEventType::NoteOn
                         : NoteEventType::NoteOff;

    noteEvent.midiNote = midiEvent.data1;
    noteEvent.velocity = midiEvent.data2;

    queue->push(noteEvent);
  }
}

// Handle keyboard events
static void keyEventCallback(device_io::KeyEvent event, void *userContext) {
  auto ctx = static_cast<NoteEventQueue *>(userContext);

  // Currently 'z' & 'x' control octive up/down
  // Need to ignore keyup (note off) for now
  if ((event.character == 120 || event.character == 122) &&
      event.type == device_io::KeyEventType::KeyUp) {
    return;
  } else if (event.type == device_io::KeyEventType::KeyDown) {
    // Note "ON" event
    ctx->push(
        NoteEvent{NoteEventType::NoteOn, asciiToMidi(event.character), 127});
  } else if (event.type == device_io::KeyEventType::KeyUp) {
    // Note "OFF" event
    ctx->push(
        NoteEvent{NoteEventType::NoteOff, asciiToMidi(event.character), 127});
  }
  // "ESC" to quit
  if (event.type == device_io::KeyEventType::KeyDown && event.keyCode == 53) {
    printf("ESC pressed, stopping...\n");
    device_io::stopKeyCaptureLoop();
  }
}

int startKeyInputCapture(NoteEventQueue &eventQueue) {
  printf("KeyCapture Example\n");
  printf("------------------\n");
  printf("Press keys to see events. ESC to quit.\n\n");

  // 1. Initialize Cocoa app
  device_io::initKeyCaptureApp();

  // 1a. Setup MIDI on this thread's run loop for now
  constexpr size_t MAX_MIDI_DEVICES = 16;
  device_io::MidiSource midiSourceBuffer[MAX_MIDI_DEVICES];
  size_t numMidiDevices =
      device_io::getMidiSources(midiSourceBuffer, MAX_MIDI_DEVICES);

  device_io::hMidiSession midiSession = nullptr;

  if (numMidiDevices) {
    // Display MIDI source options
    for (size_t i = 0; i < numMidiDevices; i++) {
      printf("%ld. %s\n", i, midiSourceBuffer[i].displayName);
    }

    int srcIndex;
    synth::utils::LogF("Enter midi device number: ");
    std::cin >> srcIndex;

    midiSession = device_io::setupMidiSession({}, midiCallback, &eventQueue);

    device_io::connectMidiSource(midiSession,
                                 midiSourceBuffer[srcIndex].uniqueID);

    device_io::startMidiSession(midiSession);

  } else {
    synth::utils::LogF("No MIDI devices found\n");
  }

  // 2. Create a minimal window (required for local capture without permissions)
  device_io::WindowConfig config = device_io::defaultWindowConfig();
  config.title = "Super Synth";
  config.width = 800;
  config.height = 500;

  if (!createCaptureWindow(config)) {
    printf("Failed to create window\n");
    return 1;
  }

  // 3. Start capturing with local mode (no permissions needed when window
  // focused)
  //    Change to CaptureMode::Global if you need capture when not focused
  //    Change to CaptureMode::Both if you want both behaviors
  if (!startKeyCapture(keyEventCallback, &eventQueue,
                       device_io::CaptureMode::Local)) {
    printf("Failed to start key capture\n");
    return 1;
  }

  const char *windowText =
      "Super Synth\n\n"
      "Press 'z' to go down an octive and 'c' to go up an octive\n\n"
      "================= Keyboard Layout =================\n"
      "|    |   |   |   |   |   |   |   |   |   |   |   |\n"
      "|    |   |   |   |   |   |   |   |   |   |   |   |\n"
      "|    | w |   | E |   |   | T |   | Y |   | U |   |\n"
      "|    |___|   |___|   |   |___|   |___|   |___|   |\n"
      "|      |       |     |     |       |       |     |\n"
      "|      |       |     |     |       |       |     |\n"
      "|  A   |   S   |  D  |  F  |   G   |   H   |  J  |\n"
      "|______|_______|_____|_____|_______|_______|_____|\n\n"
      "Press keys... (ESC to quit)\n";

  // Update window text
  device_io::setWindowText(windowText);

  // 4. Run the event loop (blocks until stopKeyCaptureLoop() called)
  device_io::runKeyCaptureLoop();

  // 5. Cleanup
  device_io::stopKeyCapture();

  if (midiSession) {
    device_io::stopMidiSession(midiSession);
    device_io::cleanupMidiSession(midiSession);
  }

  printf("Done.\n");
  return 0;
}

MIDINote asciiToMidi(char key) {
  static constexpr uint8_t SEMITONES = 12;
  static uint8_t octiveOffset = 0;

  MIDINote midiKey = 0;

  // Change Octive
  if (key == 122) { // ('z')
    --octiveOffset;
  }

  if (key == 120) { // ('x')
    ++octiveOffset;
  }

  // Change Velocity
  // 99  // ('c')
  // 118 // ('v')

  switch (key) {
  case 97: //  ('a') "C"  64
    midiKey = 64;
    break;
  case 119: // ('w') "C#" 65
    midiKey = 65;
    break;
  case 115: // ('s') "D"  66
    midiKey = 66;
    break;
  case 101: // ('e') "D#" 67
    midiKey = 67;
    break;
  case 100: // ('d') "E"  68
    midiKey = 68;
    break;
  case 102: // ('f') "F"  69
    midiKey = 69;
    break;
  case 116: // ('t') "F#" 70
    midiKey = 70;
    break;
  case 103: // ('g') "G"  71
    midiKey = 71;
    break;
  case 121: // ('y') "G#" 72
    midiKey = 72;
    break;
  case 104: // ('h') "A"  73
    midiKey = 73;
    break;
  case 117: // ('u') "A#" 74
    midiKey = 74;
    break;
  case 106: // ('j') "B"  75
    midiKey = 75;
    break;
  case 107: // ('k') "C"  76
    midiKey = 76;
    break;
  case 111: // ('o') "C#" 77
    midiKey = 77;
    break;
  case 108: // ('l') "D"  78
    midiKey = 78;
    break;
  case 112: // ('p') "D#" 79
    midiKey = 79;
    break;

  default:
    return 0; // unmapped key
  }

  return midiKey + (octiveOffset * SEMITONES);
}

} // namespace platform_io
