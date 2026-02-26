#include "WavetableBanks.h"
#include <cstring>

namespace synth::wavetable::banks {

static WavetableBank *s_registry[MAX_REGISTRY_BANKS] = {};
static int s_registryCount = 0;

void registerBank(WavetableBank *bank) {
  if (s_registryCount < MAX_REGISTRY_BANKS)
    s_registry[s_registryCount++] = bank;
  // else: silently drop — only reachable if you add more than 32 banks
}

WavetableBank *getBankByName(const char *name) {
  for (int i = 0; i < s_registryCount; i++) {
    if (std::strcmp(s_registry[i]->name, name) == 0)
      return s_registry[i];
  }
  return nullptr;
}

} // namespace synth::wavetable::banks
