#pragma once

namespace dsp::filters {

// State-variable filter (multi-mode)
struct SVFState {
  float lowpass;
  float bandpass;
  float highpass;
};

// NOTE(nico): consider calculating per block instead of per sample
void updateFilterCoefficients(float &f, float &q, float cutoff, float resonance,
                              float sampleRate);

void processSVF(float input, float cutoff, float resonance, SVFState &state,
                float sampleRate);

// Access outputs from SVFState (maybe...)
float getLowpass(const SVFState &state);
float getHighpass(const SVFState &state);
float getBandpass(const SVFState &state);

} // namespace dsp::filters
