# Phase Increment Deep Dive

Understanding how phase increment works and why it's the standard for real-time audio.

## What is Phase?

**Phase** = your current position in the wave cycle (0 to 2π radians = one complete cycle)

Think of it as "where you are" in the sine wave at any given moment.

## What is Phase Increment?

**Phase Increment** = how much you advance through the wave with each sample

```cpp
phaseIncrement = (2.0 * M_PI * frequency) / sampleRate;
```

This tells you: "how many radians do I move forward per sample?"

- **Higher frequency** → bigger jumps → larger phaseIncrement
- **Lower frequency** → smaller jumps → smaller phaseIncrement

### Example
440Hz at 44,100 samples/sec:
```
phaseIncrement = (2π * 440) / 44100 ≈ 0.0628 radians per sample
```

After ~100 samples, you've completed one full 2π cycle (one wavelength).

## Performance Comparison

### Time-based (offline rendering)
```cpp
// Every sample:
double time = i / sampleRate;                    // 1 division
double value = sin(2π * frequency * time);       // 2 multiplications + sin()
```
**Cost: 1 division + 2 multiplications + sin() per sample**

### Phase Increment (real-time)
```cpp
// Once per note:
phaseIncrement = (2π * frequency) / sampleRate;

// Every sample:
value = sin(phase);        // just sin()
phase += phaseIncrement;   // just addition
```
**Cost: 1 addition + sin() per sample**

## Key Insight

Both approaches still call `sin()` each sample - that's unavoidable. The win comes from **replacing expensive multiplications with cheap addition**.

When running 100+ voices in real-time, those saved operations matter significantly.

## The Trade-off

**Time-based:**
- ✓ Mathematically precise (no accumulated error)
- ✗ Slower - division (~40 cycles) and multiplication (~5 cycles) are expensive

**Phase increment:**
- ✓ Fast (just addition)
- ✗ Accumulated floating-point error over long periods

### Understanding Phase Drift

**Key concept:** Phase drift affects the **radian position**, not the frequency.

- Frequency stays perfectly consistent (same increment each sample)
- But the radian value drifts from the mathematically exact position
- You generate sine values at **slightly wrong radian positions**

Example after 100,000 samples:
```
Time-based:  sin(6283.185)  ← exact radian
Phase drift: sin(6283.187)  ← off by ~0.002 radians
```

Both still oscillate at 440Hz, but phase increment uses a slightly incorrect radian. This drift is inaudible - humans can't perceive absolute phase positions, only frequency and amplitude.

**What if drift gets extreme?**

Think of phase as a position on a circle (0 to 2π radians):
- At **0 drift:** perfectly in sync
- At **π drift (180°):** maximum "wrongness" - waveform is inverted
- At **2π drift (360°):** back in sync (full rotation)

Even at maximum drift (π radians), you're still generating a perfect 440Hz sine wave, just phase-inverted. This is only audible when mixing with other oscillators (destructive interference). A single drifting oscillator remains completely inaudible.

**In practice:** With `float`, drift accumulates slowly (~0.0001 radians/minute). Musical notes restart long before significant drift occurs.

## Offline vs Real-time

### Offline Rendering (WAV files)
```cpp
const int32_t numSamples = sampleRate * duration;  // e.g., 44100 * 2

for (int32_t i = 0; i < numSamples; ++i) {
    value = sin(phase);
    phase += phaseIncrement;
    // ... write to file
}
```

Loop runs for total duration, then you're done.

### Real-time Audio
```cpp
// Called repeatedly by audio system:
void processAudio(float* buffer, int bufferSize) {  // bufferSize typically 512
    for (int i = 0; i < bufferSize; ++i) {
        buffer[i] = sin(phase);
        phase += phaseIncrement;
    }
    // phase persists for next buffer callback!
}
```

**Critical difference:** No `duration` - phase is **persistent state** that lives as long as the note plays, spanning many buffer callbacks.

## Phase Wrapping

Since phase accumulates indefinitely, wrap it to prevent overflow:

```cpp
phase += phaseIncrement;
if (phase >= 2.0 * M_PI) {
    phase -= 2.0 * M_PI;
}

// Or using fmod (slightly slower):
phase = fmod(phase + phaseIncrement, 2.0 * M_PI);
```

## Float vs Double Precision

### For Real-time Audio: Use `float`

```cpp
float phase = 0.0f;
float phaseIncrement = (2.0f * M_PI * frequency) / sampleRate;
```

**Why `float` is standard:**
- Half the memory (critical with 100+ voices)
- Faster processing (SIMD operations handle more floats at once)
- Industry standard in audio plugins
- Precision is sufficient (24-bit audio ≈ 7 decimal digits)
- Note the `f` suffix on literals to avoid implicit `double` conversions

**When to use `double`:**
- Offline rendering where precision matters most
- Very long-running oscillators (hours)
- Scientific/research applications

**Bottom line:** Phase drift from `float` is negligible for musical applications. Professional audio codebases use `float` exclusively for real-time synthesis.

## Bottom Line

Phase increment is the industry standard for real-time audio synthesis because **speed matters** when processing hundreds of voices in <10ms buffer windows.
