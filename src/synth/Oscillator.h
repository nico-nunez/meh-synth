#ifndef SYNTH_OSCILLATOR_H
#define SYNTH_OSCILLATOR_H

#include "utils/Waveform.h"
#include <vector>

namespace Synth {
constexpr float DEFAULT_SAMPLE_RATE = 44100.0f;

class Oscillator {
public:
  Oscillator(float freq, float sampleRate = DEFAULT_SAMPLE_RATE,
             Waveforms::WaveformFunc waveformFunc = Waveforms::sine);

  virtual ~Oscillator() = default;

  void setFrequency(float freq);
  float getFrequency() const;

  void setSampleRate(float sampleRate);
  float getSampleRate() const;

  float getNextSampleValue();

private:
  float m_frequency;
  float m_phase = 0.0f;
  float m_phaseIncrement;
  float m_sampleRate;

  Waveforms::WaveformFunc m_waveformFunc = nullptr;

  void incrementPhase();
  void calculatePhaseIncrement();
};

// ==== Begin: Waveform Oscillators ====

class SineOsc : public Oscillator {
public:
  SineOsc(float freq, float sampleRate = DEFAULT_SAMPLE_RATE)
      : Oscillator(freq, sampleRate, Waveforms::sine) {}
};

class SawOsc : public Oscillator {
public:
  SawOsc(float freq, float sampleRate = DEFAULT_SAMPLE_RATE)
      : Oscillator(freq, sampleRate, Waveforms::saw) {}
};

class SquareOsc : public Oscillator {
public:
  SquareOsc(float freq, float sampleRate = DEFAULT_SAMPLE_RATE)
      : Oscillator(freq, sampleRate, Waveforms::square) {}
};

class TriangleOsc : public Oscillator {
public:
  TriangleOsc(float freq, float sampleRate = DEFAULT_SAMPLE_RATE)
      : Oscillator(freq, sampleRate, Waveforms::triangle) {}
};

// ==== End: Wavefrom Oscillators ====

using OscillatorPtr = std::unique_ptr<Oscillator>;
using OscillatorGroup = std::vector<OscillatorPtr>;

// Generic factory for creating specified waveform Oscillator
template <typename T>
OscillatorPtr createOscByClass(float freq,
                               float sampleRate = DEFAULT_SAMPLE_RATE) {
  static_assert(std::is_base_of<Oscillator, T>::value,
                "Invalid type: must be derived from Oscillator");
  return std::make_unique<T>(freq, sampleRate);
}

enum class OscType {
  Sine,
  Saw,
  Square,
  Triangle,
};

OscillatorPtr createOsc(OscType oscType, float freq,
                        float sampleRate = DEFAULT_SAMPLE_RATE);

} // namespace Synth

#endif
