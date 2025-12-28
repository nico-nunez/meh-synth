#ifndef SYNTH_ENGINE_H
#define SYNTH_ENGINE_H

#include "synth/Oscillator.h"
#include "synth/Voice.h"
#include <vector>

namespace Synth {

struct NoteEvent {
  float frequency;
};

using NoteEventGroup = std::vector<NoteEvent>;
using Sequence = std::vector<NoteEventGroup>;

class Engine {
public:
  Engine(const float sampleRate = DEFAULT_SAMPLE_RATE,
         const OscillatorType oscType = OscillatorType::Sine);

  void setOscillatorType(const OscillatorType oscType);
  void getOscillatorType() const;

  std::vector<float> process(const Sequence &sequence, float totalDuration);

private:
  int mMaxVoices{3};
  float mSampleRate;
  OscillatorType mOscillatorType;
  std::vector<Voice> mVoices{};

  void setupVoices();
};

} // namespace Synth

#endif
