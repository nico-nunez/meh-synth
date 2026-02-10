#include "Filters.h"
#include <algorithm>
#include <cmath>

namespace dsp::filters {

// NOTE(nico): consider calculating per block instead of per sample
void updateFilterCoefficients(float &f, float &q, float cutoff, float resonance,
                              float sampleRate) {
  // Clamp parameters to stable range
  cutoff = std::clamp(cutoff, 20.0f, sampleRate * 0.45f);
  resonance = std::clamp(resonance, 0.0f, 0.99f);

  // Calculate filter coefficients
  f = 2.0f * std::sin(static_cast<float>(M_PI) * cutoff / sampleRate);
  q = 1.0f - resonance;
}

void processSVF(float sample, float f, float q, SVFState &state) {
  // Process state-variable filter
  state.lowpass += f * state.bandpass;
  state.highpass = sample - state.lowpass - q * state.bandpass;
  state.bandpass += f * state.highpass;
}

float getLowpass(const SVFState &state) { return state.lowpass; }

float getHighpass(const SVFState &state) { return state.highpass; }

float getBandpass(const SVFState &state) { return state.bandpass; }

} // namespace dsp::filters
