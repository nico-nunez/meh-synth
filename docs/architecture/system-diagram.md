# System Architecture Diagram

## Table of Contents
- [High-Level Overview](#high-level-overview)
- [Engine: The Conductor](#engine-the-conductor)
- [Voice Pool: The Musicians](#voice-pool-the-musicians)
- [Data Flow: Configs vs State](#data-flow-configs-vs-state)
- [Parameter Update Flow](#parameter-update-flow)
- [Audio Processing Flow](#audio-processing-flow)

## High-Level Overview

```
┌─────────────────────────────────────────────────────────────┐
│                         ENGINE                              │
│  (Conductor - Source of Truth)                              │
│                                                             │
│  ┌───────────────────────────────────────────────────┐      │
│  │ Oscillator Configs (Global Settings)              │      │
│  │                                                   │      │
│  │  Config 1: Saw    + ADSR₁ (attack=1ms,  rel=50ms) │      │
│  │  Config 2: Sine   + ADSR₂ (attack=500ms, rel=2s)  │      │
│  │  Config 3: Square + ADSR₃ (attack=50ms, rel=200ms)│      │
│  └───────────────────────────────────────────────────┘      │
│                                                             │
│  Cached Values:                                             │
│  • mMaxReleaseTime = 2000ms (from Config 2)                 │
│  • Sample rate, polyphony settings, etc.                    │
│                                                             │
│  ┌─────────────────────────────────────────┐                │
│  │          Voice Pool (10 voices)         │                │
│  │                                         │                │
│  │  Voice 0  Voice 1  Voice 2  ...  Voice 9│                │
│  │  [State]  [State]  [State]      [State] │                │
│  └─────────────────────────────────────────┘                │
└─────────────────────────────────────────────────────────────┘
```

## Engine: The Conductor

The Engine is responsible for:

```
┌──────────────────────────────────────────────┐
│              ENGINE RESPONSIBILITIES         │
├──────────────────────────────────────────────┤
│                                              │
│  Configuration Management:                   │
│  • Store oscillator configs (source of truth)│
│  • Calculate derived values (max release)    │
│  • Push updates to all voices                │
│                                              │
│  Voice Management:                           │
│  • Allocate voices for new notes             │
│  • Track voice availability                  │
│  • Manage polyphony limits                   │
│                                              │
│  Audio Processing:                           │
│  • Calculate note timing (noteOff samples)   │
│  • Iterate through voices for rendering      │
│  • Mix voice outputs together                │
│                                              │
└──────────────────────────────────────────────┘
```

### Key Engine Members

```cpp
class Engine {
private:
    // Configuration (source of truth)
    std::vector<OscillatorConfig> mOscillatorConfigs;

    // Cached derived values
    float mMaxReleaseTime;

    // Voice pool
    std::vector<Voice> mVoices;
    int mMaxVoices;

    // Audio settings
    float mSampleRate;

    void updateMaxReleaseTime();  // Recalculates from configs
};
```

## Voice Pool: The Musicians

Each voice is an independent sound generator that follows Engine's configuration:

```
┌────────────────────────────────────────────────────────┐
│                      VOICE                             │
│  (Musician - Follows configuration, maintains state)   │
│                                                        │
│  ┌──────────────────────────────────────────────┐      │
│  │  Oscillator Instances (Runtime State)        │      │
│  │                                              │      │
│  │  ┌────────────────────────────────────┐      │      │
│  │  │ OscillatorModule 1                 │      │      │
│  │  │  • Oscillator (Saw waveform)       │      │      │
│  │  │    - Current phase: 0.347          │      │      │
│  │  │    - Frequency: 261.63 Hz (C4)     │      │      │
│  │  │  • Envelope (ADSR₁ settings)       │      │      │
│  │  │    - Current state: Sustain        │      │      │
│  │  │    - Progress: 0.7                 │      │      │
│  │  └────────────────────────────────────┘      │      │
│  │                                              │      │
│  │  ┌────────────────────────────────────┐      │      │
│  │  │ OscillatorModule 2                 │      │      │
│  │  │  • Oscillator (Sine waveform)      │      │      │
│  │  │    - Current phase: 0.891          │      │      │
│  │  │    - Frequency: 261.63 Hz (C4)     │      │      │
│  │  │  • Envelope (ADSR₂ settings)       │      │      │
│  │  │    - Current state: Attack         │      │      │
│  │  │    - Progress: 0.3                 │      │      │
│  │  └────────────────────────────────────┘      │      │
│  │                                              │      │
│  │  ... (OscillatorModule 3)                    │      │
│  └──────────────────────────────────────────────┘      │
│                                                        │
│  Voice State:                                          │
│  • isActive = true                                     │
│  • currentNote = C4 (261.63 Hz)                        │
└────────────────────────────────────────────────────────┘
```

### Key Voice Members

```cpp
class Voice {
private:
    struct OscillatorModule {
        Oscillator oscillator;  // Waveform generator + phase state
        Envelope envelope;      // ADSR state machine
    };

    std::vector<OscillatorModule> mOscillators;
    bool mIsActive;

public:
    void noteOn(float freq);
    void noteOff();
    float process();
    bool isAvailable() const;
};
```

## Data Flow: Configs vs State

Understanding the difference between configuration and runtime state:

```
┌─────────────────────────────────────────────────────────────┐
│                    CONFIGURATION                            │
│          (Shared across ALL voices - in Engine)             │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  • Oscillator types (Saw, Sine, Square)                     │
│  • Envelope settings (attack, decay, sustain, release)      │
│  • Number of oscillators                                    │
│                                                             │
│  Changes when: User adjusts parameters in UI                │
│  Stored in: Engine.mOscillatorConfigs                       │
│  Applied to: All voices (pushed when changed)               │
└─────────────────────────────────────────────────────────────┘
                              ↓
                    (Pushed to all voices)
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    RUNTIME STATE                            │
│          (Unique per voice - in Voice instances)            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  • Which note is playing (frequency)                        │
│  • Oscillator phase positions                               │
│  • Envelope current state (Attack/Decay/Sustain/Release)    │
│  • Envelope progress (0.0 - 1.0)                            │
│  • Voice availability (active/idle)                         │
│                                                             │
│  Changes: Every sample during audio processing              │
│  Stored in: Each Voice instance independently               │
│  Managed by: Voice itself (autonomous)                      │
└─────────────────────────────────────────────────────────────┘
```

## Parameter Update Flow

When user changes a parameter (e.g., adjusts Osc2 release time):

```
┌──────────┐
│   UI     │  User moves release knob
└────┬─────┘
     │
     ↓
┌────────────────────────────────────────────────────────┐
│  Engine::setOscillatorRelease(oscIndex, newValue)      │
└────────────────────────────────────────────────────────┘
     │
     ├─→ Step 1: Update config (source of truth)
     │   mOscillatorConfigs[oscIndex].envelopeSettings.release = newValue
     │
     ├─→ Step 2: Push to ALL voices
     │   for (auto& voice : mVoices) {
     │       voice.updateOscillatorEnvelope(oscIndex, newSettings)
     │   }
     │
     └─→ Step 3: Recalculate derived values
         updateMaxReleaseTime()  // Finds new longest release


Flow Diagram:

    UI Parameter Change
           ↓
    ┌──────────────┐
    │   Engine     │ (Updates config)
    └──────┬───────┘
           │
           ├───────→ Voice 0  (Receives update)
           ├───────→ Voice 1  (Receives update)
           ├───────→ Voice 2  (Receives update)
           ├───────→   ...
           └───────→ Voice 9  (Receives update)

All voices updated, configs remain synchronized
```

## Audio Processing Flow

How a single note plays from start to finish:

```
1. NOTE TRIGGER
   User presses C4 key
        ↓
   Engine.allocateVoice(C4_frequency)
        ↓
   Find available voice → Voice 3 is idle
        ↓
   Voice 3.noteOn(261.63 Hz)
        ↓
   ┌─────────────────────────────────────────┐
   │ Voice 3 initializes:                    │
   │  • Sets all oscillators to 261.63 Hz    │
   │  • Calls noteOn() on all envelopes      │
   │  • Marks itself as active               │
   └─────────────────────────────────────────┘


2. AUDIO PROCESSING (per sample)
   Engine.process() called 44,100 times/second
        ↓
   For each voice in pool:
        ↓
   ┌──────────────────────────────────────────┐
   │ Voice 3.process()                        │
   │                                          │
   │  For each oscillator instance:           │
   │    sample += osc.process() *             │
   │              envelope.process()          │
   │                                          │
   │  Return mixed sample                     │
   └──────────────────────────────────────────┘
        ↓
   Mix all active voices together
        ↓
   Output to audio buffer


3. NOTE RELEASE
   After N samples, Engine calls noteOff
        ↓
   Voice 3.noteOff()
        ↓
   ┌─────────────────────────────────────────┐
   │ Voice 3:                                │
   │  • Calls noteOff() on all envelopes     │
   │  • Envelopes enter Release state        │
   │  • Voice continues processing           │
   └─────────────────────────────────────────┘
        ↓
   Release phase continues...
        ↓
   Eventually all envelopes reach Idle state
        ↓
   Voice 3.isAvailable() returns true
        ↓
   Voice 3 ready for next note!
```

## Polyphony Example

What happens when multiple keys are pressed:

```
Timeline:

t=0s    User presses C4
        ↓
        Voice 0 allocated → plays C4

        Active: [Voice 0: C4]

t=1s    User presses E4 (while holding C4)
        ↓
        Voice 1 allocated → plays E4

        Active: [Voice 0: C4, Voice 1: E4]

t=2s    User presses G4 (while holding C4, E4)
        ↓
        Voice 2 allocated → plays G4

        Active: [Voice 0: C4, Voice 1: E4, Voice 2: G4]

t=3s    User releases C4
        ↓
        Voice 0.noteOff() called
        Voice 0 enters release phase (2 seconds)

        Active: [Voice 0: C4(releasing), Voice 1: E4, Voice 2: G4]

t=5s    Voice 0 release complete
        ↓
        Voice 0 becomes available

        Active: [Voice 1: E4, Voice 2: G4]
        Available: [Voice 0]


Engine's render loop each sample:
┌─────────────────────────────────────┐
│ float totalSample = 0.0f;           │
│                                     │
│ for (auto& voice : mVoices) {       │
│     totalSample += voice.process(); │
│ }                                   │
│                                     │
│ return totalSample / activeCount;   │
└─────────────────────────────────────┘
```

## Key Architectural Principles

```
┌────────────────────────────────────────────────────────┐
│                  DESIGN PRINCIPLES                     │
├────────────────────────────────────────────────────────┤
│                                                        │
│  1. Single Source of Truth                             │
│     • Engine owns all configuration                    │
│     • Voices receive copies, don't own configs         │
│                                                        │
│  2. Push, Don't Poll                                   │
│     • Engine pushes updates to voices                  │
│     • Voices don't query Engine during processing      │
│                                                        │
│  3. Separation of Concerns                             │
│     • Engine: Configuration & orchestration            │
│     • Voice: Autonomous sound generation               │
│     • Oscillator: Waveform generation only             │
│     • Envelope: Amplitude shaping only                 │
│                                                        │
│  4. Cache Expensive Calculations                       │
│     • Max release time calculated once                 │
│     • Only recalculate when configs change             │
│     • Never in audio processing loop                   │
│                                                        │
│  5. Oscillator-Envelope Pairing                        │
│     • Use OscillatorModule struct                      │
│     • Keeps related data together                      │
│     • Prevents sync issues                             │
│                                                        │
└────────────────────────────────────────────────────────┘
```
