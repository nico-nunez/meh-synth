#include "_synth_old/Engine.h"
#include "_synth_old/Oscillator.h"

#include "app/KeyProcessor.h"
#include "dsp/Waveforms.h"
#include "synth_io/Events.h"
#include "synth_io/SynthIO.h"

#include "synth/Engine.h"
#include "synth/Oscillator.h"
#include "synth/VoicePool.h"

#include <audio_io/AudioIO.h>
#include <csignal>

static void processParamEvent(synth_io::ParamEvent event, void *myContext) {
#if OLD
  auto engine = static_cast<Synth::Engine *>(myContext);
#else
  auto engine = static_cast<synth::Engine *>(myContext);
  engine->processParamEvent(event);
#endif
}

static void processNoteEvent(synth_io::NoteEvent event, void *myContext) {
#if OLD
  auto engine = static_cast<Synth::Engine *>(myContext);
#else
  auto engine = static_cast<synth::Engine *>(myContext);
#endif

  engine->processNoteEvent(event);
}
static void processAudioBlock(float **outputBuffer, size_t numChannels,
                              size_t numFrames, void *myContext) {
#if OLD
  auto engine = static_cast<Synth::Engine *>(myContext);
#else
  auto engine = static_cast<synth::Engine *>(myContext);
#endif
  engine->processAudioBlock(outputBuffer, numChannels, numFrames);
}

int main() {
  constexpr float SAMPLE_RATE = 48000.0f;

  // 1. Setup synth engine
#if OLD
  Synth::Engine engine{SAMPLE_RATE, Synth::OscillatorType::Square};
#else
  using Engine = synth::Engine;
  using EngineConfig = synth::EngineConfig;

  EngineConfig engineConfig{};
  engineConfig.sampleRate = SAMPLE_RATE;
  engineConfig.osc1.waveform = synth::WaveformType::Saw;
  engineConfig.osc1.detuneAmount = 10.0f;
  engineConfig.osc2 = {synth::WaveformType::Saw, 0.5f, -1, -10.0f, true};
  engineConfig.subOsc.mixLevel = 0.7f;

  Engine engine = synth::createEngine(engineConfig);
#endif

  // 2. Setup audio_io
  synth_io::SessionConfig sessionConfig{};
  sessionConfig.sampleRate = static_cast<uint32_t>(SAMPLE_RATE);

  synth_io::SynthCallbacks sessionCallbacks{};
  sessionCallbacks.processAudioBlock = processAudioBlock;
  sessionCallbacks.processNoteEvent = processNoteEvent;
  sessionCallbacks.processParamEvent = processParamEvent;

  synth_io::hSynthSession session =
      synth_io::initSession(sessionConfig, sessionCallbacks, &engine);

  synth_io::startSession(session);

  app_input::startKeyInputCapture(session);

  synth_io::stopSession(session);
  synth_io::disposeSession(session);

  return 0;
}
