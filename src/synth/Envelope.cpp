#include "synth/Envelope.h"
#include <stdexcept>

namespace Synth {
Envelope::Envelope(float sampleRate) : mSampleRate(sampleRate) {
  setAttack(10.0f);
  setDecay(100.0f);
  setSustain(0.7f);
  setRelease(200.0f);
}

// Attack (Stage 1)
void Envelope::setAttack(float ms) {
  if (ms < 0)
    throw std::runtime_error("Invalid Value: must be positive");

  mAttackMs = ms;
  mAttackSamples = convertMsToSamples(ms);
}
float Envelope::getAttack() const { return mAttackMs; }

// Decay (Stage 2)
void Envelope::setDecay(float ms) {
  if (ms < 0)
    throw std::runtime_error("Invalid Value: must be positive");

  mDecayMs = ms;
  mDecaySamples = convertMsToSamples(ms);
}
float Envelope::getDecay() const { return mDecayMs; }

// Sustain (Stage 3)
void Envelope::setSustain(float value) {
  if (value < 0 || value > 1)
    throw std::runtime_error("Invalid Value: must be >=0.0 and <=1.0");
  mSustainLevel = value;
}
float Envelope::getSustain() const { return mSustainLevel; }

// Release (Stage 4)
void Envelope::setRelease(float ms) {
  if (ms < 0)
    throw std::runtime_error("Invalid Value: must be positive");

  mReleaseMs = ms;
  mReleaseSamples = convertMsToSamples(ms);
}
float Envelope::getRelease() const { return mReleaseMs; }

// Sample Rate
void Envelope::setSampleRate(float sampleRate) {
  if (sampleRate < 0)
    throw std::runtime_error("Invalid Value: must be positive");

  mSampleRate = sampleRate;
  updateSampleCounts();
}
float Envelope::getSampleRate() const { return mSampleRate; }

// Envelope Control Methods
void Envelope::trigger() {
  mSamplesInCurrentState = 0;
  mState = State::Attack;
}

void Envelope::release() {
  mReleaseStartLevel = getCurrentAmplitude();
  mState = State::Release;
  mSamplesInCurrentState = 0;
}

float Envelope::getNextSample() {
  float amplitude = 0.0f;

  switch (mState) {

  case State::Attack:
    amplitude = calculateAttack();
    mSamplesInCurrentState++;

    if (mSamplesInCurrentState >= mAttackSamples) {
      mState = State::Decay;
      mSamplesInCurrentState = 0;
    }
    break;

  case State::Decay:
    amplitude = calculateDecay();
    mSamplesInCurrentState++;

    if (mSamplesInCurrentState >= mDecaySamples) {
      mState = State::Sustain;
      mSamplesInCurrentState = 0;
    }
    break;

  case State::Sustain:
    amplitude = mSustainLevel;
    break;

  case State::Release:
    amplitude = calculateRelease();
    mSamplesInCurrentState++;

    if (mSamplesInCurrentState >= mReleaseSamples) {
      mState = State::Idle;
      mSamplesInCurrentState = 0;
    }
    break;

  case State::Idle:
    amplitude = 0.0f;
    break;
  }

  return amplitude;
}

bool Envelope::isDone() const { return mState == State::Idle; }

// Helper Methods
float Envelope::getCurrentAmplitude() const {
  switch (mState) {
  case State::Attack:
    return calculateAttack();
  case State::Decay:
    return calculateDecay();
  case State::Sustain:
    return mSustainLevel;
  case State::Release:
    return calculateRelease();
  case State::Idle:
    return 0.0f;
  default:
    return 0.0f;
  }
}

int Envelope::convertMsToSamples(float ms) const {
  return static_cast<int>((ms / 1000.f) * mSampleRate);
}

void Envelope::updateSampleCounts() {
  setAttack(mAttackMs);
  setDecay(mDecayMs);
  setRelease(mReleaseMs);
}

// Amplitude Calculations
float Envelope::calculateAttack() const {
  if (mAttackSamples == 0)
    return 1.0f;

  return (float)mSamplesInCurrentState / (float)mAttackSamples;
}

float Envelope::calculateDecay() const {
  if (mDecaySamples == 0)
    return 1.0f;

  float progress = (float)mSamplesInCurrentState / (float)mDecaySamples;
  float decayRange = 1.0f - mSustainLevel;
  return 1.0f - (progress * decayRange);
}

float Envelope::calculateRelease() const {
  if (mReleaseSamples == 0)
    return 1.0f;

  float progress = (float)mSamplesInCurrentState / (float)mReleaseSamples;
  return mReleaseStartLevel * (1.0f - progress);
}

} // namespace Synth
