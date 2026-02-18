#pragma once

#include "synth/Oscillator.h"

namespace synth {
struct Engine;
}

namespace synth::param_bindings {

using WaveformType = oscillator::WaveformType;

enum ParamID {
  OSC1_WAVEFORM,
  OSC1_MIX_LEVEL,
  OSC1_DETUNE_AMOUNT,
  OSC1_OCTAVE_OFFSET,
  OSC1_ENABLED,
  OSC2_WAVEFORM,
  OSC2_MIX_LEVEL,
  OSC2_DETUNE_AMOUNT,
  OSC2_OCTAVE_OFFSET,
  OSC2_ENABLED,
  OSC3_WAVEFORM,
  OSC3_MIX_LEVEL,
  OSC3_DETUNE_AMOUNT,
  OSC3_OCTAVE_OFFSET,
  OSC3_ENABLED,
  SUB_OSC_WAVEFORM,
  SUB_OSC_MIX_LEVEL,
  SUB_OSC_DETUNE_AMOUNT,
  SUB_OSC_OCTAVE_OFFSET,
  SUB_OSC_ENABLED,
  AMP_ENV_ATTACK,
  AMP_ENV_DECAY,
  AMP_ENV_SUSTAIN_LEVEL,
  AMP_ENV_RELEASE,
  MASTER_GAIN,
  PARAM_COUNT,
};

enum ParamValueFormat {
  NORMALIZED,
  DENORMALIZED,
};

enum ParamStorageType {
  FLOAT,
  INT8,
  BOOL,
  WAVEFORM // For WaveformType enum
};

struct ParamBinding {
  union {
    float *floatPtr;
    int8_t *int8Ptr;
    bool *boolPtr;
    WaveformType *waveformPtr;
  };
  ParamStorageType type;
  float min, max;
};

void initParamBindings(synth::Engine &engine);

void getParamValueByID(
    Engine &engine, ParamID id,
    ParamValueFormat valueFormat = ParamValueFormat::DENORMALIZED);

void setParamValueByID(
    Engine &engine, ParamID id, float value,
    ParamValueFormat valueFormat = ParamValueFormat::DENORMALIZED);

} // namespace synth::param_bindings
