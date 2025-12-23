#include "Voice.h"
#include "Oscillator.h"

namespace Synth {
Voice::Voice(OscillatorType oscType, float sampleRate)
    : mOscillator(oscType, sampleRate), mEnvelope(sampleRate) {}

void Voice::noteOn(float freq) {
  mOscillator.setFrequency(freq);
  mIsActive = true;

  mEnvelope.noteOn();
}

void Voice::noteOff() { mEnvelope.noteOff(); }

float Voice::process() {
  if (!mIsActive)
    return 0.0f;

  float oscValue = mOscillator.process();
  float envValue = mEnvelope.process();

  if (mEnvelope.isComplete())
    mIsActive = false;

  return oscValue * envValue;
}

bool Voice::isAvailable() const { return !mIsActive; }

void Voice::setOscillatorType(OscillatorType oscType) {
  mOscillator.setOscillatorType(oscType);
}

void Voice::setSampleRate(float sampleRate) {
  mOscillator.setSampleRate(sampleRate);
  mEnvelope.setSampleRate(sampleRate);
}

} // namespace Synth
