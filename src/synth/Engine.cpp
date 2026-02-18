#include "Engine.h"
#include "ParamBindings.h"
#include "VoicePool.h"

#include "synth_io/Events.h"

#include <cassert>
#include <cstdio>

namespace synth {
using NoteEvent = synth_io::NoteEvent;
using ParamEvent = synth_io::ParamEvent;

Engine createEngine(const EngineConfig &config) {
  Engine engine{};

  voices::updateVoicePoolConfig(engine.voicePool, config);

  param_bindings::initParamBindings(engine);

  return engine;
}

void Engine::processParamEvent(const ParamEvent &event) {
  param_bindings::setParamValueByID(*this, static_cast<ParamID>(event.id),
                                    event.value);
}

void Engine::processNoteEvent(const synth_io::NoteEvent &event) {
  if (!event.midiNote)
    return;

  if (event.type == synth_io::NoteEventType::NoteOff) {
    voices::releaseVoice(voicePool, event.midiNote);
  } else {
    voices::handleNoteOn(voicePool, event.midiNote, event.velocity, noteCount++,
                         sampleRate);
  }
}

void Engine::processAudioBlock(float **outputBuffer, size_t numChannels,
                               size_t numFrames) {
  voices::processVoices(voicePool, poolBuffer, numFrames);

  for (size_t frame = 0; frame < numFrames; frame++) {
    for (size_t ch = 0; ch < numChannels; ch++) {
      outputBuffer[ch][frame] = poolBuffer[frame];
    }
  }
}

} // namespace synth
