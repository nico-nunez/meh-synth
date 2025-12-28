# Audio DSP Reference Guide

Concepts and "aha moments" for digital audio and signal processing.

## Fundamentals
- **[Sampling and Nyquist](sampling-and-nyquist.md)** - Why sample rate matters, aliasing, the Nyquist limit

## Wave Generation
- **[Oscillator Techniques](oscillator-techniques.md)** - Time-based vs phase increment approaches
- **[Phase Increment Deep Dive](phase-increment-deep-dive.md)** - How phase increment works, performance, precision trade-offs
- **[Polyphonic Synthesis](polyphonic-synthesis.md)** - How multiple notes become one waveform, voice management
- **[Voice Stealing](voice-stealing.md)** - Voice allocation strategies, when to steal, oldest-first vs priority-based

## Audio File Formats
- **[WAV File Format](wav-file-format.md)** - File structure, headers, chunks, why you need metadata
- Coming soon: PCM encoding, bit depth, other formats

## Envelopes
- **[Envelope Calculations](envelope-calculations.md)** - Sample-by-sample ADSR math, linear vs exponential curves

## Effects and Processing
- Coming soon: filters, modulation, dynamics

---

**Note:** These docs capture the key insights and "why it works" moments, not exhaustive theory.
