# Envelope Calculations: Sample-by-Sample Math

## Table of Contents
- [Core Concept](#core-concept)
- [State Transitions and Note-Off Behavior](#state-transitions-and-note-off-behavior)
- [Linear Envelopes](#linear-envelopes)
  - [Attack Phase](#attack-phase)
  - [Decay Phase](#decay-phase)
  - [Sustain Phase](#sustain-phase)
  - [Release Phase](#release-phase)
- [Exponential Envelopes](#exponential-envelopes)
  - [Why Exponential?](#why-exponential)
  - [Attack Phase (Exponential)](#attack-phase-exponential)
  - [Decay Phase (Exponential)](#decay-phase-exponential)
  - [Release Phase (Exponential)](#release-phase-exponential)
- [Key Takeaways](#key-takeaways)

## Core Concept

Envelopes are calculated **per audio sample**, not per millisecond. The basic flow:

1. Convert time parameters (ms) to sample counts
2. Track samples elapsed in current phase
3. Calculate amplitude multiplier for current sample
4. Multiply audio signal by this value

**Sample count conversion:**
```cpp
int attackSamples = (attackTimeMs / 1000.0f) * sampleRate;
// Example: 100ms at 44,100 Hz = 4,410 samples
```

---

## State Transitions and Note-Off Behavior

**Critical concept:** Release can start at **any time**, interrupting whichever phase is currently running.

### How Note-Off Works

When `noteOff()` is called, the envelope must:
1. **Immediately capture the current amplitude** (wherever it is)
2. **Transition to Release phase**
3. **Start release from that captured value**

This means release doesn't always start from sustain level!

### Implementation Pattern

```cpp
enum class EnvelopeState { Idle, Attack, Decay, Sustain, Release };

class Envelope {
    EnvelopeState state;
    float releaseStartLevel;  // Captured on noteOff
    int samplesInCurrentState;

    void noteOff() {
        // Capture current amplitude before switching phases
        releaseStartLevel = getCurrentAmplitude();

        // Transition to release
        state = Release;
        samplesInCurrentState = 0;
    }

    float getCurrentAmplitude() {
        switch(state) {
            case Attack:  return calculateAttackAmplitude();
            case Decay:   return calculateDecayAmplitude();
            case Sustain: return sustainLevel;
            case Release: return calculateReleaseAmplitude();
            case Idle:    return 0.0f;
        }
    }
};
```

### Real-World Scenarios

**Scenario 1: Released during Attack**
- Attack ramping: 0.0 → 0.3 → **0.6** ← noteOff here
- `releaseStartLevel = 0.6`
- Release: 0.6 → 0.4 → 0.2 → 0.0

**Scenario 2: Released during Decay**
- Decay ramping: 1.0 → 0.8 → **0.7** ← noteOff here (targeting sustain 0.5)
- `releaseStartLevel = 0.7`
- Release: 0.7 → 0.467 → 0.233 → 0.0
- Note: Never reached sustain!

**Scenario 3: Released during Sustain**
- Sustain holding: 0.5 → 0.5 → **0.5** ← noteOff here
- `releaseStartLevel = 0.5`
- Release: 0.5 → 0.333 → 0.167 → 0.0
- This is the "typical" case

### Key Implementation Details

1. **Don't assume release starts from sustain** - it's whatever the current amplitude is
2. **Capture amplitude atomically** - get the value at the exact moment of noteOff
3. **Reset sample counter** - `samplesInCurrentState = 0` when entering release
4. **You're interrupting, not continuing** - release replaces the current phase entirely

---

## Linear Envelopes

Your intuition is spot-on! Linear envelopes use simple proportion.

### Attack Phase

**Goal:** Ramp from 0.0 to 1.0 over attack time

```cpp
float progress = (float)samplesElapsed / (float)attackSamples;
float amplitude = progress;  // 0.0 → 1.0
```

**Example:**
- Attack time: 100ms (4,410 samples at 44.1kHz)
- Sample 0: 0 / 4410 = 0.0
- Sample 2205: 2205 / 4410 = 0.5
- Sample 4410: 4410 / 4410 = 1.0

### Decay Phase

**Goal:** Ramp from 1.0 down to sustain level

**Key insight:** Decay is **sustain-dependent**. The sustain level determines where decay ends.

```cpp
float progress = (float)samplesElapsed / (float)decaySamples;
float decayRange = 1.0f - sustainLevel;  // How far to travel
float amplitude = 1.0f - (progress * decayRange);
```

**Breaking it down:**
- `decayRange` = distance from peak (1.0) to sustain level
- `progress * decayRange` = how far we've traveled down
- `1.0f - traveled` = current position

**Example (sustain = 0.5):**
- Decay range: 1.0 - 0.5 = 0.5 (travel halfway down)
- Sample 0 (0% through): `1.0 - (0.0 × 0.5)` = **1.0** (at peak)
- Sample 4410 (50% through): `1.0 - (0.5 × 0.5)` = **0.75** (halfway)
- Sample 8820 (100% through): `1.0 - (1.0 × 0.5)` = **0.5** (at sustain) ✓

**Edge cases:**
- **Sustain = 1.0:** Range is 0, decay does nothing (stays at 1.0)
- **Sustain = 0.0:** Range is 1.0, decay goes all the way to silence

### Sustain Phase

**Goal:** Hold at sustain level (no calculation needed)

```cpp
float amplitude = sustainLevel;  // Just return the value
```

### Release Phase

**Goal:** Ramp from current level to 0.0

**Key insight:** Release is **position-dependent**. It starts from wherever the envelope is when the key is released.

```cpp
float releaseStartLevel;  // Captured when note-off occurs
float progress = (float)samplesElapsed / (float)releaseSamples;
float amplitude = releaseStartLevel * (1.0f - progress);
```

**What is `releaseStartLevel`?**
- If released **during sustain:** `releaseStartLevel = sustainLevel`
- If released **during attack:** `releaseStartLevel = current attack value`
- If released **during decay:** `releaseStartLevel = current decay value`

**Example 1 - Released from sustain (sustain = 0.5):**
- Release starts at: 0.5
- Sample 0 (0% through): `0.5 × (1.0 - 0.0)` = **0.5**
- Sample 6615 (50% through): `0.5 × (1.0 - 0.5)` = **0.25**
- Sample 13230 (100% through): `0.5 × (1.0 - 1.0)` = **0.0** ✓

**Example 2 - Released during decay (at 0.8):**
- Release starts at: 0.8 (wherever decay was at note-off)
- Sample 0: `0.8 × 1.0` = **0.8**
- Sample 6615: `0.8 × 0.5` = **0.4**
- Sample 13230: `0.8 × 0.0` = **0.0** ✓

**This explains the "0 sustain with audible release" behavior:**
- Sustain = 0.0, but key released during decay (say at 0.3)
- Release fades from 0.3 → 0.0 (audible tail, even though sustain is silent!)

---

## Exponential Envelopes

### Why Exponential?

Human perception of volume is **logarithmic**, not linear. Exponential curves sound more natural because:
- Attack feels punchier (slower start, faster finish)
- Release sounds smoother (faster start, slower finish)
- Matches how acoustic instruments behave

### Attack Phase (Exponential)

**Goal:** Curved ramp from 0.0 to 1.0 (slow start, fast finish)

```cpp
float progress = (float)samplesElapsed / (float)attackSamples;
float amplitude = 1.0f - exp(-5.0f * progress);  // Exponential rise
```

**The math:**
- `exp(-5.0f * progress)` creates a decay curve (1.0 → ~0.0)
- `1.0f - decay` flips it to a rise curve (0.0 → 1.0)
- The `-5.0f` constant controls curve steepness (higher = sharper)

**Comparison at 50% progress:**
- Linear: 0.5
- Exponential: ~0.92 (much closer to peak already!)

**Common steepness values:**
- `-3.0` = gentle curve
- `-5.0` = moderate (typical)
- `-7.0` = aggressive

### Decay Phase (Exponential)

**Goal:** Curved ramp from 1.0 to sustain level

**Key insight:** Like linear decay, this is **sustain-dependent**.

```cpp
float progress = (float)samplesElapsed / (float)decaySamples;
float decayRange = 1.0f - sustainLevel;  // Distance to travel
float amplitude = sustainLevel + (decayRange * exp(-5.0f * progress));
```

**The math:**
- `decayRange` = how far from peak (1.0) to sustain level
- `exp(-5.0f * progress)` = exponential decay multiplier (1.0 → ~0.0)
- `decayRange * exp(...)` = apply curve to our specific range
- `sustainLevel + curve` = offset so we end at sustain, not 0

**Example (sustain = 0.5, 50% through decay):**
- Decay range: 1.0 - 0.5 = 0.5
- At 50%: `0.5 + (0.5 × exp(-2.5))` = `0.5 + (0.5 × 0.082)` = **~0.54**
- Linear would be: **0.75** (exponential drops faster initially)

**Edge cases:**
- **Sustain = 1.0:** Range is 0, stays at 1.0 (like linear)
- **Sustain = 0.0:** Range is 1.0, full exponential curve to silence

### Release Phase (Exponential)

**Goal:** Curved ramp from current level to 0.0

**Key insight:** Like linear release, this is **position-dependent** - starts from current envelope level.

```cpp
float releaseStartLevel;  // Captured when note-off occurs
float progress = (float)samplesElapsed / (float)releaseSamples;
float amplitude = releaseStartLevel * exp(-5.0f * progress);
```

**The math:**
- `releaseStartLevel` = wherever envelope is at note-off (sustain level, or mid-attack/decay)
- `exp(-5.0f * progress)` = exponential decay multiplier (1.0 → ~0.0)
- Multiply by start level to scale the curve

**Example (released from sustain = 0.5):**
- Sample 0: `0.5 × exp(0)` = `0.5 × 1.0` = **0.5**
- Sample 6615 (50%): `0.5 × exp(-2.5)` = `0.5 × 0.082` = **~0.041**
- Linear would be: **0.25** (exponential drops much faster)

**Why this sounds better:**
- Drops quickly at first (removes initial "pop")
- Tail fades smoothly and gradually (sounds natural)
- Never fully reaches 0.0 mathematically (but becomes inaudible)

---

## Key Takeaways

**Linear vs Exponential:**
- **Linear** = mathematically simple, sounds mechanical
- **Exponential** = slightly more complex, sounds natural
- Start with linear to learn, upgrade to exponential for quality

**Common Gotchas:**
1. **Exponential never reaches target exactly** - stop when close enough (`< 0.001`)
2. **Release starts from current level** - not always sustain
3. **Sample rate matters** - always convert ms to samples
4. **Steepness constant is tunable** - `-5.0f` is just a starting point

**Formula Pattern:**
```cpp
// All phases follow:
float progress = samplesElapsed / totalSamplesForPhase;  // 0.0 → 1.0
// Then apply linear or exponential curve to progress
```

**Next Steps:**
- Implement linear first (easier to debug)
- Test with extreme values (0ms attack, 0.0 sustain, etc.)
- Add exponential once linear works perfectly
- Consider making curve steepness a parameter
