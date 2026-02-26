#pragma once

#include "dsp/WaveTable.h"

namespace synth::wavetable::banks {
using WavetableBank = dsp::wavetable::WavetableBank;

inline constexpr int MAX_REGISTRY_BANKS = 32;

void registerBank(WavetableBank *bank);

// Case-sensitive
WavetableBank *getBankByName(const char *name);

} // namespace synth::wavetable::banks
