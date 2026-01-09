# Voice Architecture

## Table of Contents
- [What is a Voice?](#what-is-a-voice)
- [Why Not Just Oscillators?](#why-not-just-oscillators)
- [Voice Components](#voice-components)
- [One Oscillator Per Voice](#one-oscillator-per-voice)
- [Code Example](#code-example)
- [Mental Model](#mental-model)
- [Target Architecture: Multiple Oscillators with Per-Oscillator Envelopes](#target-architecture-multiple-oscillators-with-per-oscillator-envelopes)

## What is a Voice?

A **Voice** represents a single note being played. It's a container class that bundles together all the components needed to generate and shape a musical note.

## Why Not Just Oscillators?

Initially, it's tempting to conflate "oscillator" and "voice" - but there's more to a sound than just the waveform:

- **Oscillator** = Raw waveform generation (the sound source)
- **Voice** = Complete signal chain that shapes that raw waveform into a musical note

Things that shape a voice:
- ADSR envelopes (amplitude over time)
- Filters (tonal shaping)
- Effects (character, stereo positioning)
- Modulation (vibrato, tremolo)

## Voice Components

A typical voice contains:

```
Oscillator(s) → Envelope(s) → Filter(s) → Effects → Output
     ↓              ↓            ↓           ↓
  Raw wave    Shape volume   Shape tone   Add character
```

### Essential Components
- **Oscillator(s)** - Generate raw waveforms
- **ADSR Envelope** - Control amplitude over time
- **Note state** - Which note, velocity, on/off status

### Later Additions
- **Filter** - Shape frequency content (brightness/darkness)
- **Filter Envelope** - Modulate filter over time
- **LFO** - Add vibrato, tremolo, etc.
- **Pan/Effects** - Stereo positioning, distortion, etc.

## One Oscillator Per Voice

Yes! Each voice gets its own oscillator instance because each voice needs to:
- Generate its own independent waveform
- Play at its own pitch (frequency)
- Be at its own phase position

### Why Not Share Oscillators?

Without separate oscillators, you'd need parallel arrays (messy and error-prone):

```cpp
// DON'T DO THIS - hard to keep in sync
std::vector<SineWaveOscillator> oscillators;
std::vector<ADSREnvelope> envelopes;
std::vector<int> activeNotes;
std::vector<float> velocities;
```

With Voice class (much cleaner):

```cpp
// Better approach
class Voice {
    SineWaveOscillator oscillator;  // This voice's oscillator
    ADSREnvelope envelope;
    int midiNote;
    float velocity;
    bool isActive;
};

std::vector<Voice> voices;  // Clean and organized
```

## Code Example

```cpp
class Voice {
    SineWaveOscillator oscillator;
    ADSREnvelope envelope;
    int midiNote = -1;
    float velocity = 0.0f;
    bool isActive = false;

public:
    void noteOn(int note, float vel, float sampleRate) {
        midiNote = note;
        velocity = vel;
        isActive = true;

        // Set THIS voice's oscillator to THIS note's frequency
        float freq = midiNoteToFrequency(note);
        oscillator.setFrequency(freq, sampleRate);

        envelope.noteOn();
    }

    void noteOff() {
        envelope.noteOff();
    }

    float process() {
        if (!isActive) return 0.0f;

        float sample = oscillator.process();
        float envValue = envelope.process();

        // Check if envelope finished
        if (envelope.isComplete()) {
            isActive = false;
        }

        return sample * envValue;
    }

    bool getIsActive() const { return isActive; }
    int getMidiNote() const { return midiNote; }
};
```

## Mental Model

Think of a piano:
- **Voice** = One string being struck
- **Oscillator** = That string vibrating at a specific frequency
- **ADSR** = How that string's vibration decays over time

When you press 3 keys simultaneously:
- 3 voices active
- 3 oscillators generating different frequencies
- 3 envelopes at different stages (attack, sustain, release)
- Final audio output = sum of all active voices

## Target Architecture: Multiple Oscillators with Per-Oscillator Envelopes

**Decision:** Each voice contains multiple oscillators, and **each oscillator has its own envelope instance**. This enables complex sound design while keeping all voices configured identically.

### Architecture Overview

**Global/Patch Level (same for all voices):**
```cpp
struct OscillatorConfig {
  OscillatorType type;              // Saw, Sine, Square, etc.
  Envelope::Settings envelopeSettings;  // ADSR settings for this oscillator
};

// Example patch configuration
std::vector<OscillatorConfig> configs = {
  {OscillatorType::Saw,   {attack: 1ms,   release: 50ms}},   // Punchy
  {OscillatorType::Sine,  {attack: 500ms, release: 2000ms}}, // Pad
  {OscillatorType::Square, {attack: 50ms, release: 200ms}}   // Mid
};
```

**Per-Voice Instances:**
```cpp
class Voice {
    struct OscillatorModule {
        Oscillator oscillator;      // Generates waveform
        Envelope envelope;          // Tracks ADSR state for THIS instance
    };

    std::vector<OscillatorModule> mOscillators;  // One per config

    void noteOn(float freq) {
        for (auto& osc : mOscillators) {
            osc.oscillator.setFrequency(freq);
            osc.envelope.noteOn();  // Independent envelope state
        }
    }

    float process() {
        float sample = 0.0f;
        for (auto& osc : mOscillators) {
            float oscValue = osc.oscillator.process();
            float envValue = osc.envelope.process();
            sample += oscValue * envValue;
        }
        return sample / mOscillators.size();  // Mix down
    }
};
```

### Why Per-Oscillator Envelopes?

**Sound Design Power:**
- Fast attack oscillator + slow attack oscillator = evolving, layered textures
- Short release + long release = percussive attack with sustained tail
- Common in modern synths: Serum, Vital, Massive

**Example Sound:**
```
Osc1 (Saw):  Attack=1ms,  Release=50ms  → Sharp transient
Osc2 (Sine): Attack=500ms, Release=2s   → Smooth pad that swells
Result: Punchy pluck with evolving pad underneath
```

### Key Architectural Points

1. **All voices share same oscillator configs** - set once in the patch/UI
2. **Each voice has independent envelope states** - for polyphony
3. **Each oscillator within a voice has its own envelope** - for sound design
4. **Voice represents one keypress** - but generates multiple simultaneous sounds

### Implementation Notes

**noteOff Timing:**
Use the **longest release time** across all oscillators to ensure none get cut off:

```cpp
float maxRelease = 0.0f;
for (const auto& config : mOscillatorConfigs) {
    maxRelease = std::max(maxRelease, config.envelopeSettings.release);
}
int releaseSamples = static_cast<int>((maxRelease / 1000.0f) * mSampleRate);
int lastSampleIndex = samplesPerStep - 1;
int noteOffSample = lastSampleIndex - releaseSamples;
```

**Voice Availability:**
A voice is available when **all** of its oscillator envelopes have completed (all idle).

## Key Takeaway

**Voice** is the fundamental unit of sound generation, containing multiple oscillators each with their own envelope for maximum sound design flexibility. This architecture matches modern production synthesizers while maintaining clean polyphony management.
