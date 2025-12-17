#include "SynthUtils.h"
#include "synth/Oscillator.h"
#include "utils/AudioUtils.h"

#include <iostream>
#include <utility>
#include <vector>

namespace SynthUtils {
void generateSineValues(std::vector<int16_t> &samples, double frequency,
                        const int duration, const int sampleRate) {
  // Volume (0.0 to 1.0, keeping it at 50% to avoid clipping)
  const double AMPLITUDE = 0.5;
  const int32_t numSamples = sampleRate * duration;

  std::cout << "Generating " << duration << " seconds of " << frequency
            << "HZ sine wave...\n";

  for (int32_t i = 0; i < numSamples; ++i) {
    // The sine wave formula: sin(2 * π * frequency * time)
    // time = i / SAMPLE_RATE (current sample number divided by samples per
    // second)
    double time = static_cast<double>(i) / sampleRate;
    double value = AMPLITUDE * std::sin(2.0 * M_PI * frequency * time);

    // Convert from floating point (-1.0 to 1.0) to 16-bit integer (-32768 to
    // 32767) This is how audio is stored in WAV files
    int16_t sample = static_cast<int16_t>(value * 32767.0);
    samples.push_back(sample);
  }
}

/* Semitone ratio is 2^(1/12). This is multiplied by the number
 * of semitones from A4 for the note that's to be calculated.
 * This value (ratio * semitones) is multiplied by 440hz (A4)
 * to get the value of the desired note in hertz.
 *
 * Example: C4 = 440hz * (2^((1/12) * -9)), where C4 is -9 semitones from A4
 */
float getHertzFromSemitoneOffset(int semitones) {
  return 440.0f * static_cast<float>(std::pow(2.0, (1.0 / 12.0) * semitones));
}

AudioUtils::Sequence
getSequenceFromFreqs(AudioUtils::FreqSequence &freqSequence,
                     Synth::OscType oscType, float sampleRate) {
  AudioUtils::Sequence sequence{};

  for (auto &freqGroup : freqSequence) {
    Synth::OscillatorGroup oscGroup{};
    for (auto &freq : freqGroup) {
      oscGroup.push_back(Synth::createOsc(oscType, freq, sampleRate));
    }
    sequence.push_back(std::move(oscGroup));
  }

  return sequence;
}

} // namespace SynthUtils
