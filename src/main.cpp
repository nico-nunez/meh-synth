#include "_synth_/Engine.h"
#include "dsp/Waveforms.h"
#include "platform_io/AudioProcessor.h"
#include "platform_io/NoteEventQueue.h"
#include "synth/Engine.h"
#include "synth/Oscillator.h"

#include <audio_io/AudioIO.h>
#include <csignal>

static void processEvent(platform_io::NoteEvent event, void *myContext) {
#if OLD
  auto engine = static_cast<Synth::Engine *>(myContext);
#else
  auto engine = static_cast<synth::Engine *>(myContext);
#endif

  engine->processEvent(event);
}

static void processBlock(float **outputBuffer, size_t numChannels,
                         size_t numFrames, void *myContext) {
#if OLD
  auto engine = static_cast<Synth::Engine *>(myContext);
#else
  auto engine = static_cast<synth::Engine *>(myContext);
#endif
  engine->processBlock(outputBuffer, numChannels, numFrames);
}

int main() {
  constexpr float SAMPLE_RATE = 48000.0f;

  // 1. Setup synth engine
#if OLD
  Synth::Engine engine{SAMPLE_RATE, Synth::OscillatorType::Square};
#else
  synth::Engine engine{};
  engine.sampleRate = SAMPLE_RATE;
#endif

  // 2. Setup audio_io
  platform_io::AudioConfig config{};
  config.sampleRate = static_cast<uint32_t>(SAMPLE_RATE);

  platform_io::setupAudioProcess(config, processEvent, processBlock, &engine);
  return 0;
}
