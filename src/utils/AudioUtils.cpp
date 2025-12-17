#include "AudioUtils.h"
#include "synth/Oscillator.h"

#include <cassert>
#include <climits>
#include <cstdint>
#include <vector>

namespace AudioUtils {

/* Used to play/render sequence:
 * A "Sequence" is a vector of "OscillatorGroups"
 *"OscillatorGroup" is a vector of "Oscillators"
 * sequence[i] == WHEN to play/render (order)
 * sequence[i][j] == WHAT to play/render (oscillators)
 */
void renderSequence(std::vector<int16_t> &samples, Sequence &sequence,
                    int duration, float amplitude) {
  for (Synth::OscillatorGroup &oscillators : sequence) {

    // Ensure it's not empty and not too large to be converted to int
    // TODO: add runtime checks
    assert(oscillators.size() > 0);
    assert(oscillators.size() < INT_MAX);

    int numOscillators = static_cast<int>(oscillators.size());

    // Making assumption that all oscillators have the same sample rate
    const int32_t totalSamples =
        static_cast<int>(oscillators[0]->getSampleRate()) * duration;

    // Reduce the amplitude based on number of oscillators. Example:
    // 3 oscillators at N amplitude is louder than 1 oscillator at N amplitude
    const float relativeAmplitude = amplitude * (1.0f / numOscillators);

    for (int32_t i = 0; i < totalSamples; ++i) {
      float value{0.0f};
      for (auto &osc : oscillators) {
        value += osc->getNextSampleValue();
      }

      // Convert from floating point (-1.0 to 1.0) to 16-bit integer (-32768
      // to 32767) This is how audio is stored in WAV files
      int16_t sample = static_cast<int16_t>(relativeAmplitude * value *
                                            static_cast<float>(INT16_MAX));
      samples.push_back(sample);
    }
  }
}

} // namespace AudioUtils
