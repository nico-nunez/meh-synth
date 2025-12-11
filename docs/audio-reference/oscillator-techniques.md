# Oscillator Techniques: Time-based vs Phase Increment

Two common approaches for generating periodic waveforms (sine, saw, square, etc.)

## Time-based Calculation

```cpp
// i = current sample number (0 to 44100 for 1 second at 44.1kHz)
double time = static_cast<double>(i) / sampleRate;
double value = std::sin(2.0 * M_PI * frequency * time);
```

**When to use:**
- Offline rendering (generating WAV files)
- When precision matters most
- Educational/learning projects

**Pros:**
- Mathematically precise (no accumulated error)
- Simpler to understand
- No phase wrapping needed

**Cons:**
- Slower (multiplication every sample)
- Not practical for real-time

## Phase Increment (Industry Standard for Real-time)

```cpp
// Setup (once per note):
float phase = 0.0f;
float phaseIncrement = (2.0f * M_PI * frequency) / sampleRate;

// Per sample:
float value = std::sin(phase);
phase += phaseIncrement;

// Wrap phase to prevent overflow:
if (phase >= 2.0f * M_PI) {
    phase -= 2.0f * M_PI;
}
```

**When to use:**
- Real-time audio (VST/AU plugins, synths)
- Performance-critical code
- Professional audio software

**Pros:**
- Fast (just addition per sample)
- Industry standard approach
- Scales well with multiple voices

**Cons:**
- Can accumulate floating-point error over long periods
- Requires phase wrapping
- Slightly more complex

## Key Takeaway

For real-time audio synthesis, **phase increment is the standard**. The performance difference matters when processing hundreds of voices in <10ms buffer windows.
