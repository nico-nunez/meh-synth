#ifndef SYNTH_VOICE_H
#define SYNTH_VOICE_H

#include "Envelope.h"
#include "Oscillator.h"

#include "synth_io/Events.h"
#include <cstdint>

namespace Synth {
struct NoteEvent;

class Voice {
public:
  Voice(OscillatorType oscType = OscillatorType::Saw,
        float sampleRate = DEFAULT_SAMPLE_RATE);

  void noteOn(const synth_io::NoteEvent &event);
  void noteOff();

  float process();

  bool isAvailable() const;
  bool isReleasing() const;

  void setOscillatorType(OscillatorType oscType);
  void setSampleRate(float sampleRate);

  bool shouldStop(uint8_t midiNote);

private:
  Oscillator mOscillator;
  Envelope mEnvelope;
  uint8_t mMidiNote;
  bool mIsActive = false;
};

} // namespace Synth

#endif // !SYNTH_VOICE_H
