# Voice Stealing & Voice Allocation

## Table of Contents
- [Overview](#overview)
- [The Problem](#the-problem)
- [Voice Allocation Strategies](#voice-allocation-strategies)
- [Implementation Considerations](#implementation-considerations)
- [Code Examples](#code-examples)

## Overview

**Voice stealing** is the process of deciding which active voice to interrupt when all voices are busy and a new note needs to play. This is essential in polyphonic synthesizers with limited voice counts.

**Key concept:** Most synthesizers have a fixed number of voices (e.g., 8, 16, 32). When all voices are playing and a new note arrives, you must "steal" one of the active voices.

## The Problem

```
Synth has 8 voices
User plays 10-note chord

What happens to notes 9 and 10?
- Don't play them? ❌ Bad UX
- Wait for a voice? ❌ Delayed/missed notes
- Steal a voice? ✅ Standard practice
```

## Voice Allocation Strategies

### 1. Oldest Note First (Most Common)
Steal the voice that has been playing the longest.

**Pros:**
- Most natural/musical behavior
- Preserves recently-played notes (what user is focused on)

**Cons:**
- Requires tracking note-on timestamps

```cpp
Voice* allocateVoice() {
  // Try available first
  for (auto& voice : mVoices) {
    if (voice.isAvailable()) return &voice;
  }

  // Steal oldest
  Voice* oldest = &mVoices[0];
  for (auto& voice : mVoices) {
    if (voice.getStartTime() < oldest->getStartTime()) {
      oldest = &voice;
    }
  }
  return oldest;
}
```

### 2. Priority-Based (Envelope Stage)
Steal voices in this order: Release > Sustain > Decay > Attack

**Pros:**
- Minimizes audible artifacts
- Rarely interrupts attack (most noticeable)

**Cons:**
- More complex logic
- Need to track envelope state per voice

**Priority order:**
1. Release stage (already fading out)
2. Sustain stage (constant level, less noticeable)
3. Decay stage (transitioning)
4. Attack stage (AVOID if possible - most audible)

### 3. Quietest Voice
Steal the voice with the lowest current amplitude.

**Pros:**
- Minimizes audible interruption
- Works well for dynamic content

**Cons:**
- Requires calculating current amplitude
- Can steal important quiet notes

### 4. Round-Robin (Simplest)
Cycle through voices in order.

**Pros:**
- Dead simple
- Predictable
- No state tracking needed

**Cons:**
- Can steal recent/important notes
- Not musically optimal

```cpp
Voice* allocateVoice() {
  Voice* voice = &mVoices[mNextVoiceIndex];
  mNextVoiceIndex = (mNextVoiceIndex + 1) % mVoices.size();
  return voice;
}
```

### 5. No Stealing (Sequential/Step Sequencer)
Always restart the same voices - no stealing logic needed.

**Use case:** Step sequencers, arpeggiators, preset patterns

**Pros:**
- Simple, predictable
- Clean step transitions

**Cons:**
- No voice overlap
- Not suitable for real-time performance

```cpp
// Just restart voices directly
for (size_t i = 0; i < notes.size() && i < mVoices.size(); i++) {
  mVoices[i].noteOn(notes[i].frequency);
}
```

## Implementation Considerations

### When to Check Availability

**Real-time performance:** Always allocate, steal if needed
```cpp
void playNote(float frequency) {
  Voice* voice = allocateVoice();  // Never fails
  voice->noteOn(frequency);
}
```

**Pre-rendered/sequenced:** Can choose to skip or steal
```cpp
void playNote(float frequency) {
  Voice* voice = findAvailableVoice();
  if (voice == nullptr) {
    // Skip note, or steal based on strategy
    return;
  }
  voice->noteOn(frequency);
}
```

### Voice State Management

Voices need to track state for smart stealing:
```cpp
class Voice {
  uint64_t mNoteOnTime;      // For "oldest first"
  float mCurrentAmplitude;   // For "quietest first"
  Envelope::State mEnvState; // For "priority-based"
};
```

### Tracking Active Notes

For MIDI note-off matching, track which voice plays which note:
```cpp
std::unordered_map<int, Voice*> mActiveNotes;  // MIDI note -> Voice

void noteOn(int midiNote, float velocity) {
  Voice* voice = allocateVoice();
  voice->noteOn(midiNoteToFreq(midiNote));
  mActiveNotes[midiNote] = voice;
}

void noteOff(int midiNote) {
  if (mActiveNotes.count(midiNote)) {
    mActiveNotes[midiNote]->noteOff();
    mActiveNotes.erase(midiNote);
  }
}
```

## Code Examples

### Complete Oldest-First Implementation

```cpp
class Engine {
private:
  std::vector<Voice> mVoices;
  uint64_t mSampleCounter = 0;

public:
  Voice* allocateVoice() {
    // First pass: find available voice
    for (auto& voice : mVoices) {
      if (voice.isAvailable()) {
        return &voice;
      }
    }

    // Second pass: steal oldest
    Voice* oldest = &mVoices[0];
    for (auto& voice : mVoices) {
      if (voice.getNoteOnTime() < oldest->getNoteOnTime()) {
        oldest = &voice;
      }
    }

    return oldest;
  }

  void playNote(float frequency) {
    Voice* voice = allocateVoice();
    voice->noteOn(frequency, mSampleCounter);
    mSampleCounter++;
  }
};

class Voice {
private:
  uint64_t mNoteOnTime;
  bool mIsActive;

public:
  void noteOn(float frequency, uint64_t timestamp) {
    mNoteOnTime = timestamp;
    mIsActive = true;
    // ... trigger envelope, set frequency
  }

  uint64_t getNoteOnTime() const { return mNoteOnTime; }
  bool isAvailable() const { return !mIsActive; }
};
```

### Priority-Based (Envelope Stage)

```cpp
Voice* allocateVoiceByPriority() {
  // Try available first
  for (auto& voice : mVoices) {
    if (voice.isAvailable()) return &voice;
  }

  // Find lowest priority (highest enum value = lowest priority)
  Voice* lowestPriority = &mVoices[0];
  for (auto& voice : mVoices) {
    if (voice.getEnvelopePriority() > lowestPriority->getEnvelopePriority()) {
      lowestPriority = &voice;
    }
  }

  return lowestPriority;
}

// In Voice class:
int getEnvelopePriority() const {
  // Lower number = higher priority (don't steal)
  switch (mEnvelope.getState()) {
    case Envelope::State::Attack:  return 0;  // Highest priority
    case Envelope::State::Decay:   return 1;
    case Envelope::State::Sustain: return 2;
    case Envelope::State::Release: return 3;  // Lowest priority
    case Envelope::State::Idle:    return 4;
    default: return 4;
  }
}
```

## Key Takeaways

- **Voice stealing is necessary** in polyphonic synths with limited voices
- **"Oldest first" is the industry standard** for real-time performance
- **Simple strategies work fine** for learning/prototyping
- **Choose strategy based on use case:**
  - Real-time performance → Oldest first or Priority-based
  - Step sequencer → No stealing (restart voices)
  - CPU-limited → Round-robin (cheapest)
- **Always allocate successfully** - never fail to play a note due to busy voices
