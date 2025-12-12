#include "Oscillator.h"
#include <cmath>

namespace {
// Create constant for phase wrapping comparison
// Avoids multiplication operation on every increment
constexpr float TWO_PI = 2 * static_cast<float>(M_PI);
} // namespace

namespace Synth {

Oscillator::Oscillator(float freq, float sampleRate)
    : m_frequency(freq), m_sampleRate(sampleRate) {
  calculatePhaseIncrement();
}

// Change in frequency requires recalculation of phase increment
// Thus member is private and updated via method
void Oscillator::setFrequency(float freq) {
  m_frequency = freq;
  calculatePhaseIncrement();
}
float Oscillator::getFrequency() const { return m_frequency; }

// Change in sample rate requires recalculation of phase increment
// Thus member is private and updated via method
void Oscillator::setSampleRate(float sampleRate) {
  m_sampleRate = sampleRate;
  calculatePhaseIncrement();
}
float Oscillator::getSampleRate() const { return m_sampleRate; }

// Calculate in advance in order to increment the phase each step.
// More efficient than time-based calculation on every increment.
// Introduces (neglibile) drift due to continous add operations using float
void Oscillator::calculatePhaseIncrement() {
  m_phaseIncrement = TWO_PI * m_frequency / m_sampleRate;
}

// Increament after each sample
void Oscillator::incrementPhase() {
  m_phase += m_phaseIncrement;

  // Wrap phase to prevent float precision issues
  // due to limited number of significant digits (~7)
  if (m_phase >= TWO_PI)
    m_phase -= TWO_PI;
}

float Oscillator::getNextSampleValue() {
  float sample{std::sin(m_phase)};
  incrementPhase();
  return sample;
}

} // namespace Synth
