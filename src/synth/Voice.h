#ifndef SYNTH_VOICE_H
#define SYNTH_VOICE_H

#include "synth/Envelope.h"
#include "synth/Oscillator.h"

namespace Synth {
class Voice {
public:
  Voice(OscillatorType oscType = OscillatorType::Sine,
        float sampleRate = DEFAULT_SAMPLE_RATE);

  void noteOn(float freq);
  void noteOff();

  float process();

  bool isAvailable() const;

  void setOscillatorType(OscillatorType oscType);
  void setSampleRate(float sampleRate);

private:
  Oscillator mOscillator;
  Envelope mEnvelope;
  bool mIsActive = false;
};

} // namespace Synth

#endif // !SYNTH_VOICE_H
