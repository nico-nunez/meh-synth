# Parameter Updates & Configuration Management

## Table of Contents
- [The Core Question](#the-core-question)
- [Storage Strategy: Copy vs Reference](#storage-strategy-copy-vs-reference)
- [Update Strategy: Targeted vs Batch](#update-strategy-targeted-vs-batch)
- [Thread Safety: The Critical Issue](#thread-safety-the-critical-issue)
- [Recommended Implementation Phases](#recommended-implementation-phases)
- [Key Takeaways](#key-takeaways)

## The Core Question

How do we share configuration between Engine (source of truth) and Voices (need the settings to operate)?

Two sub-questions:
1. **Storage:** Does each Voice store a copy of config, or reference Engine's config?
2. **Updates:** When config changes (e.g., Osc2 waveform sine→square), how do we propagate that?

## Storage Strategy: Copy vs Reference

### Option A: Store Full Copy (Recommended)

```cpp
class Envelope {
    EnvelopeSettings settings;  // Local copy (16 bytes)
    EnvelopeState state;        // Runtime state
    float currentLevel;         // Runtime state
};
```

**Pros:**
- **Cache locality:** Everything needed to process a voice is contiguous in memory
- **No pointer dereferencing:** Audio thread never chases pointers (cache misses expensive at 44.1kHz)
- **Thread-safe reads:** Audio thread has its own copy, UI thread can't modify it mid-read
- **Simple code:** No lifetime/ownership concerns

**Cons:**
- Memory duplication: ~16 bytes × 3 oscs × 10 voices = 480 bytes (negligible on modern hardware)
- Must push updates to all voices when config changes

### Option B: Store Pointer/Reference

```cpp
class Envelope {
    const EnvelopeSettings* settings;  // Points to Engine's config
    EnvelopeState state;
    float currentLevel;
};
```

**Pros:**
- Single source of truth (Engine owns one copy)
- Zero memory duplication
- Updates automatically visible to all voices

**Cons:**
- **Pointer dereference every sample:** Cache miss risk (audio processing happens 44,100+ times/second)
- **Thread safety nightmare:** UI thread modifies while audio thread reads → race conditions
- **Requires atomics or locks:** Kills real-time guarantees

### Option C: Hybrid (Advanced)

```cpp
class Envelope {
    std::atomic<const EnvelopeSettings*> settings;  // Atomic pointer swap
    EnvelopeState state;
    float currentLevel;
};
```

**Pros:**
- Can atomically swap entire config structure
- Good for complex multi-parameter updates

**Cons:**
- Still pointer dereferencing in audio loop
- More complex than needed for learning project
- Atomic operations have overhead

### Verdict: Store Copy

For production-quality synths, **Option A (store copy)** wins:
- 480 bytes is nothing (modern CPU L1 cache is 32KB+)
- Cache locality matters more than memory savings
- Simpler code with fewer bugs
- Thread-safe by default

## Update Strategy: Targeted vs Batch

### Targeted Update (Efficient)

Update only the specific parameter that changed:

```cpp
// Engine interface - fine-grained control
void Engine::setOscillatorWaveform(int oscIndex, WaveformType type) {
    // 1. Update source of truth
    mOscillatorConfigs[oscIndex].type = type;

    // 2. Push to all voices - ONLY update waveform
    for (auto& voice : mVoices) {
        voice.setOscillatorWaveform(oscIndex, type);
    }
}

void Engine::setOscillatorAttack(int oscIndex, float attack) {
    mOscillatorConfigs[oscIndex].envelopeSettings.attack = attack;

    for (auto& voice : mVoices) {
        voice.setOscillatorAttack(oscIndex, attack);
    }
}

// Voice interface - many small methods
void Voice::setOscillatorWaveform(int oscIndex, WaveformType type) {
    mOscillators[oscIndex].oscillator.setWaveform(type);
}

void Voice::setOscillatorAttack(int oscIndex, float attack) {
    mOscillators[oscIndex].envelope.setAttack(attack);
}
```

**Pros:**
- Minimal work (only update what changed)
- Clear intent in code

**Cons:**
- Many methods to maintain
- More API surface area
- Marginal performance benefit (looping 10 voices is trivial)

### Batch Update (Simpler API)

Update entire config structure at once:

```cpp
// Engine interface - coarse-grained
void Engine::updateOscillatorConfig(int oscIndex, const OscillatorConfig& config) {
    // 1. Update source of truth
    mOscillatorConfigs[oscIndex] = config;

    // 2. Push entire config to all voices
    for (auto& voice : mVoices) {
        voice.updateOscillatorConfig(oscIndex, config);
    }
}

// Voice interface - one method handles everything
void Voice::updateOscillatorConfig(int oscIndex, const OscillatorConfig& config) {
    auto& oscModule = mOscillators[oscIndex];

    // Update all settings
    oscModule.oscillator.setWaveform(config.type);
    oscModule.envelope.setSettings(config.envelopeSettings);
}
```

**Pros:**
- Simple API (one method instead of many)
- Easy to add new parameters (just extend struct)
- Consistent update mechanism

**Cons:**
- Updates values that didn't change (usually irrelevant)

### Verdict: Start Batch, Optimize Later

Use **batch updates** initially:
- Simpler code
- Easier to maintain
- Performance difference is negligible (10 voices × 3 oscs = 30 updates, microseconds)
- Profile first, optimize if needed

## Thread Safety: The Critical Issue

⚠️ **Audio software runs on multiple threads:**

```
UI Thread                              Audio Thread
(User interaction)                     (Real-time processing)
────────────────────────────────────────────────────────────────
User turns attack knob         │       Voice.process() running
  ↓                            │         ↓
Engine.setAttack(50.0f)        │       envelope.process()
  ↓                            │         ↓
voice.updateSettings(...)  ←───┼───────  Reads envelope.settings
                               │       ← RACE CONDITION!
```

**The Problem:**
- UI thread modifies `envelope.settings.attack`
- Audio thread reads `envelope.settings.attack` at the same time
- Result: Undefined behavior (crackling, glitches, crashes)

### Safe Update Strategies

#### Strategy 1: Atomic Individual Values

For simple scalar types:

```cpp
class Envelope {
    std::atomic<float> attack;
    std::atomic<float> decay;
    std::atomic<float> sustain;
    std::atomic<float> release;

    // UI thread (safe)
    void setAttack(float value) {
        attack.store(value, std::memory_order_relaxed);
    }

    // Audio thread (safe)
    float process() {
        float attackTime = attack.load(std::memory_order_relaxed);
        // Use attackTime...
    }
};
```

**Pros:**
- Simple for individual parameters
- Lock-free and real-time safe

**Cons:**
- Only works for atomically-sized types (float, int)
- Can't atomically update multiple related values
- Awkward API (`.load()` and `.store()` everywhere)

#### Strategy 2: Lock-Free Queue (Industry Standard)

Queue parameter changes, apply them in audio thread:

```cpp
// In Engine
struct ParameterChange {
    int voiceIndex;
    int oscIndex;
    OscillatorConfig newConfig;
};

// Lock-free queue (use library like readerwriterqueue)
LockFreeQueue<ParameterChange> mParamQueue;

// UI thread - queue changes (non-blocking)
void Engine::updateOscillatorConfig(int osc, const OscillatorConfig& config) {
    // Update Engine's source of truth
    mOscillatorConfigs[osc] = config;

    // Queue updates for all voices
    for (int v = 0; v < mVoices.size(); v++) {
        mParamQueue.push({v, osc, config});
    }
}

// Audio thread - apply queued changes at block start
void Engine::process(float* buffer, int numSamples) {
    // Process all pending parameter changes
    ParameterChange change;
    while (mParamQueue.try_pop(change)) {
        mVoices[change.voiceIndex].updateOscillatorConfig(
            change.oscIndex,
            change.newConfig
        );
    }

    // NOW process audio (with stable parameters)
    for (int i = 0; i < numSamples; i++) {
        for (auto& voice : mVoices) {
            buffer[i] += voice.process();
        }
    }
}
```

**Pros:**
- Fully thread-safe without locks
- Updates applied at safe points (not mid-sample)
- Can batch multiple parameter changes
- Industry standard approach

**Cons:**
- Requires lock-free queue library
- Slightly more complex than direct updates
- Small latency (changes applied at next audio block, not immediately)

#### Strategy 3: Double Buffering

Swap between two config copies:

```cpp
class Envelope {
    EnvelopeSettings settings[2];      // Two copies
    std::atomic<int> readIndex{0};     // Which copy audio thread uses

    // UI thread
    void updateSettings(const EnvelopeSettings& newSettings) {
        int writeIndex = 1 - readIndex.load();  // Get inactive buffer
        settings[writeIndex] = newSettings;      // Write to inactive
        readIndex.store(writeIndex);             // Atomic swap
    }

    // Audio thread
    float process() {
        int idx = readIndex.load();
        // Use settings[idx] - guaranteed stable for this block
    }
};
```

**Pros:**
- Clean separation of read/write
- Atomic pointer swap is fast

**Cons:**
- Doubles memory usage (2x config per voice)
- More complex than needed for simple case

### Verdict: Start Simple, Add Queue Later

**Phase 1:** Ignore threading (single-threaded testing)
**Phase 2:** Add lock-free queue when integrating with real audio I/O
**Phase 3:** Consider double-buffering only if profiling shows queue overhead

## Recommended Implementation Phases

### Phase 1: Simple & Correct (Start Here)

```cpp
class Engine {
    std::vector<OscillatorConfig> mOscillatorConfigs;  // Source of truth
    std::vector<Voice> mVoices;

public:
    // Batch update API
    void updateOscillatorConfig(int oscIndex, const OscillatorConfig& config) {
        // Update source of truth
        mOscillatorConfigs[oscIndex] = config;

        // Push to all voices (direct, not thread-safe yet)
        for (auto& voice : mVoices) {
            voice.updateOscillatorConfig(oscIndex, config);
        }
    }
};

class Voice {
    struct OscillatorModule {
        Oscillator oscillator;
        Envelope envelope;
    };
    std::vector<OscillatorModule> mOscillators;

public:
    void updateOscillatorConfig(int oscIndex, const OscillatorConfig& config) {
        auto& osc = mOscillators[oscIndex];
        osc.oscillator.setWaveform(config.type);
        osc.envelope.setSettings(config.envelopeSettings);
    }
};

class Envelope {
    EnvelopeSettings settings;  // Local copy
    // ... state variables

public:
    void setSettings(const EnvelopeSettings& newSettings) {
        settings = newSettings;  // Direct assignment (not thread-safe yet)
    }
};
```

**Use this for:**
- Initial implementation
- Unit testing
- Algorithm development
- Single-threaded testing

### Phase 2: Thread-Safe (Production)

Add lock-free queue:

```cpp
class Engine {
    std::vector<OscillatorConfig> mOscillatorConfigs;
    std::vector<Voice> mVoices;
    LockFreeQueue<ParameterChange> mParamQueue;  // NEW

public:
    // UI thread calls this
    void updateOscillatorConfig(int oscIndex, const OscillatorConfig& config) {
        mOscillatorConfigs[oscIndex] = config;

        // Queue updates instead of direct push
        for (int v = 0; v < mVoices.size(); v++) {
            mParamQueue.push({v, oscIndex, config});
        }
    }

    // Audio thread calls this
    void process(float* buffer, int numSamples) {
        // Apply queued parameter changes
        ParameterChange change;
        while (mParamQueue.try_pop(change)) {
            mVoices[change.voiceIndex].updateOscillatorConfig(
                change.oscIndex,
                change.newConfig
            );
        }

        // Process audio with stable parameters
        // ...
    }
};
```

**Use this when:**
- Integrating with real audio I/O (JUCE, RtAudio, etc.)
- Building actual plugin/standalone app
- Need real-time guarantees

### Phase 3: Optimize (If Needed)

Profile first! Only optimize if you find bottlenecks:

```cpp
// Example: Targeted updates if batch updates show up in profiler
void Engine::setOscillatorWaveform(int osc, WaveformType type) {
    mOscillatorConfigs[osc].type = type;

    for (int v = 0; v < mVoices.size(); v++) {
        mParamQueue.push({
            .type = ParamChangeType::Waveform,
            .voiceIndex = v,
            .oscIndex = osc,
            .waveform = type
        });
    }
}
```

**Only needed if:**
- Profiling shows parameter updates are a bottleneck (unlikely)
- You have hundreds of voices (not typical for synth)

## Key Takeaways

1. **Store config copies in each voice** - cache locality and thread safety beat memory savings
2. **Start with batch updates** - simpler API, negligible performance difference
3. **Thread safety matters** - use lock-free queue when integrating with real audio I/O
4. **Optimize later** - get it working first, profile, then optimize if needed

### Mental Model

```
Configuration Flow:

UI Thread:
  User adjusts knob
       ↓
  Engine.updateConfig(...)
       ↓
  Update Engine's source of truth
       ↓
  Queue change for audio thread

Audio Thread:
  Start of audio block
       ↓
  Pull all queued changes
       ↓
  Apply to voice copies
       ↓
  Process audio (stable parameters)
```

### Real-World Examples

**Modern synths using copy + queue approach:**
- Serum (Xfer Records)
- Vital (Matt Tytel)
- Surge XT (open source - good reference!)

All use variants of "store copy, update via queue" because it's the sweet spot of simplicity, performance, and thread safety.
