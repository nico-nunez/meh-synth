# Sine Wave Generator

A learning project for exploring C++ and audio DSP fundamentals.

## About This Project

This is a **learning-focused project** created by a TypeScript developer learning C++ for audio software and plugin development. The goal is to understand core concepts, not just build working software.

**Key learning areas:**
- C++ fundamentals (classes, memory management, build systems)
- Audio DSP basics (oscillators, phase, sample rate, WAV format)
- Foundation skills for future JUCE/audio plugin development

## What This Program Does

Generates a pure sine wave tone and saves it as a WAV file, written from scratch without audio libraries to understand exactly how digital audio works.

## Building

```bash
make              # Build debug version (default)
make debug        # Build with debug symbols
make release      # Build optimized release version
make clean        # Remove built files
```

## Project Structure

```
sine-wav-generator/
  src/
    main.cpp
    synth/           # Synthesis components (Voice, Synthesizer)
    utils/           # WAV file writing, helpers

  docs/
    cpp-reference/   # C++ learning notes
    audio-reference/ # Audio DSP learning notes

  Makefile           # Build configuration
  CLAUDE.md          # Project guidelines
```

## Documentation

Extensive learning documentation covering both C++ and audio DSP:

### C++ Reference
- [Core Concepts](docs/cpp-reference/core-concepts.md) - Namespaces, parameters, formatting
- [Structs and Constructors](docs/cpp-reference/structs-and-constructors.md) - Creating types
- [Classes](docs/cpp-reference/classes.md) - Encapsulation, public/private
- [File Organization](docs/cpp-reference/file-organization.md) - Headers, namespaces, directory structure
- [Makefiles](docs/cpp-reference/makefiles.md) - Build system automation
- [Integer Types](docs/cpp-reference/integer-types.md) - uint8_t, int32_t, etc.
- [Headers and Includes](docs/cpp-reference/headers-and-includes.md) - Include system
- [Memory Management](docs/cpp-reference/memory-management.md) - Pointers, references, RAII
- [Arrays and Containers](docs/cpp-reference/arrays-and-containers.md) - std::vector, arrays
- [Common Pitfalls](docs/cpp-reference/common-pitfalls.md) - Gotchas from TypeScript

### Audio DSP Reference
- [Sampling and Nyquist](docs/audio-reference/sampling-and-nyquist.md) - Digital audio basics
- [WAV File Format](docs/audio-reference/wav-file-format.md) - Binary file structure
- [Oscillator Techniques](docs/audio-reference/oscillator-techniques.md) - Generating waveforms
- [Phase Increment Deep Dive](docs/audio-reference/phase-increment-deep-dive.md) - Frequency to phase math

## Key Concepts

### Digital Audio Basics
- Audio is an array of numbers representing air pressure over time
- Sample rate (48,000 Hz) = 48,000 numbers per second
- Each number represents speaker position at that instant

### The Sine Wave Formula
```cpp
value = sin(2 * π * phase)
phase += frequency / sampleRate
```
- `frequency`: How many cycles per second (440 Hz = A4 note)
- `phase`: Current position in the waveform (0.0 to 1.0)
- Result: A value between -1.0 and 1.0

### Data Conversion
- Math gives us floating point values (-1.0 to 1.0)
- WAV files use 16-bit integers (-32,768 to 32,767)
- Multiply by 32,767 to convert

## Experiments to Try

### Easy
1. **Change frequency**: Try 220 (A3), 880 (A5), or 100 (low rumble)
2. **Change duration**: Make it longer or shorter
3. **Change amplitude**: Try 0.2 (quieter) or 0.8 (louder), don't exceed 1.0!

### Medium
4. **Add two frequencies**: Generate two sine waves and add them (creates a chord)
5. **Fade in/out**: Multiply amplitude by a value that changes over time
6. **Multiple voices**: Create polyphony with voice management

### Advanced
7. **Stereo**: Different samples for left/right channels
8. **Harmonics**: Mix multiple frequencies (fundamental + overtones)
9. **Tremolo**: Modulate amplitude with a slow sine wave
10. **ADSR envelope**: Attack, Decay, Sustain, Release

## Learning Philosophy

This project follows a specific approach:
- **Explanations over automatic fixes** - Understanding why, not just what works
- **Documentation as learning** - Notes are concise, scannable, and practical
- **Incremental complexity** - Build foundational knowledge step by step
- **Code changes by request** - This is a learning project, not a "get it working" project

See [CLAUDE.md](CLAUDE.md) for detailed project guidelines.

## Tools & Environment

- **Compiler:** Clang++ with C++17
- **Build System:** Make (learning Make before CMake for JUCE)
- **Platform:** macOS (concepts apply cross-platform)
- **Code Style:** PascalCase classes, m_ prefix for members (JUCE conventions)

## What You Learn

Even though this is "just" a sine wave generator, you'll understand:
- How digital audio is represented (samples and sample rates)
- How frequencies relate to pitch
- How audio effects are math operations on arrays
- WAV file format structure
- Binary file I/O in C++
- C++ classes, encapsulation, and project organization
- Build systems and compilation

**This is the foundation.** Every audio effect, synthesizer, or plugin ultimately does the same thing: reading samples, doing math on them, and writing samples.

## Next Steps

Once comfortable with this:
1. Read an existing WAV file and process it (effects, filters)
2. Implement simple DSP (low-pass filter, distortion)
3. Move to JUCE for real-time processing
4. Build audio plugins (VST/AU)

The concepts remain the same, but real-time audio adds constraints: process buffers within milliseconds, no memory allocations in audio thread, thread safety, etc.

## Resources

Documentation is written as a learning resource for developers coming from high-level languages (JavaScript/TypeScript) to C++ audio development.
