# Constants and Scope

How to define constants and control their visibility in C++.

## Table of Contents
- [constexpr - Compile-Time Constants](#constexpr---compile-time-constants)
- [Anonymous Namespaces](#anonymous-namespaces)
- [Common Patterns](#common-patterns)
- [Quick Rules](#quick-rules)

## constexpr - Compile-Time Constants

`constexpr` tells the compiler to compute a value at **compile-time**, not runtime.

```cpp
constexpr float TWO_PI = 2.0f * M_PI;  // Computed when code is compiled
constexpr int BUFFER_SIZE = 512;
constexpr float SAMPLE_RATE = 44100.0f;
```

### Why Use constexpr?

**Zero runtime cost:**
```cpp
// Without constexpr
float computeIncrement() {
  float twoPi = 2.0f * M_PI;  // Computed every time function is called
  return twoPi * freq / sampleRate;
}

// With constexpr
constexpr float TWO_PI = 2.0f * M_PI;  // Computed once at compile-time
float computeIncrement() {
  return TWO_PI * freq / sampleRate;  // TWO_PI is already a value
}
```

**Type safety:**
```cpp
#define TWO_PI 6.28318f  // Macro - no type checking, error-prone

constexpr float TWO_PI = 6.28318f;  // Typed constant - compiler checks usage
```

### constexpr vs const

```cpp
const float x = 5.0f;        // Runtime constant - value known at runtime
constexpr float y = 5.0f;    // Compile-time constant - value baked into binary

const float a = getValue();      // ✓ OK - can be runtime value
constexpr float b = getValue();  // ✗ ERROR - must be computable at compile-time
```

**Use `constexpr` when:**
- Value is known at compile-time
- You want guaranteed compile-time evaluation
- Defining mathematical constants (π, e, etc.)

**Use `const` when:**
- Value might come from runtime (user input, file, calculation)
- Working with function parameters or variables that don't change

## Anonymous Namespaces

An **anonymous namespace** limits visibility to the current file only:

```cpp
// Oscillator.cpp
namespace {
  // These are ONLY visible in Oscillator.cpp
  constexpr float TWO_PI = 2.0f * M_PI;
  constexpr int MAX_OSCILLATORS = 128;

  float helperFunction(float x) {
    return x * TWO_PI;
  }
}

namespace Synth {
  void Oscillator::incrementPhase() {
    m_phase += m_phaseIncrement;
    if (m_phase >= TWO_PI) {  // Can use TWO_PI here
      m_phase -= TWO_PI;
    }
  }
}
```

### Why Use Anonymous Namespaces?

**1. Avoid name collisions:**
```cpp
// Oscillator.cpp
namespace {
  constexpr float TWO_PI = 6.28318f;  // Only in Oscillator.cpp
}

// Filter.cpp
namespace {
  constexpr float TWO_PI = 6.28318f;  // Only in Filter.cpp
}

// No collision! Each file has its own TWO_PI
```

**2. Better than global variables:**
```cpp
// Bad - pollutes global namespace
constexpr float TWO_PI = 6.28318f;  // Visible everywhere if header is included

// Good - file-local only
namespace {
  constexpr float TWO_PI = 6.28318f;  // Only visible in this .cpp file
}
```

**3. Replaces `static` keyword (old C style):**
```cpp
// Old C style (still works)
static constexpr float TWO_PI = 6.28318f;

// Modern C++ style (preferred)
namespace {
  constexpr float TWO_PI = 6.28318f;
}
```

### When NOT to Use Anonymous Namespaces

**Never in header files:**
```cpp
// Oscillator.h - DON'T DO THIS
namespace {
  constexpr float TWO_PI = 6.28318f;  // ✗ Each .cpp that includes this gets its own copy!
}
```

If you need a constant in a header, use:
```cpp
// Oscillator.h - Do this instead
namespace Synth {
  inline constexpr float TWO_PI = 6.28318f;  // Shared across all files
}
```

## Common Patterns

### Pattern 1: File-Local Constants
```cpp
// Oscillator.cpp
namespace {
  constexpr float TWO_PI = 6.28318530717958647692f;
  constexpr float MIN_FREQUENCY = 20.0f;
  constexpr float MAX_FREQUENCY = 20000.0f;
}

namespace Synth {
  void Oscillator::setFrequency(float freq) {
    if (freq >= MIN_FREQUENCY && freq <= MAX_FREQUENCY) {
      m_frequency = freq;
    }
  }
}
```

### Pattern 2: File-Local Helper Functions
```cpp
// Synthesizer.cpp
namespace {
  float midiToFrequency(int midiNote) {
    return 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
  }

  float decibelToLinear(float db) {
    return std::pow(10.0f, db / 20.0f);
  }
}

namespace Synth {
  void Synthesizer::playNote(int midiNote) {
    float freq = midiToFrequency(midiNote);  // Helper only visible here
    m_voice.setFrequency(freq);
  }
}
```

### Pattern 3: Shared Constants (in header)
```cpp
// Constants.h
#pragma once

namespace Audio {
  // Use inline constexpr for header constants
  inline constexpr float TWO_PI = 6.28318530717958647692f;
  inline constexpr int DEFAULT_SAMPLE_RATE = 44100;
  inline constexpr int DEFAULT_BUFFER_SIZE = 512;
}

// Usage in multiple .cpp files:
#include "Constants.h"
float value = Audio::TWO_PI * frequency;
```

## Quick Rules

1. **Use `constexpr` for compile-time constants** - zero runtime cost
2. **Use anonymous namespaces in `.cpp` files** - keeps implementation details private
3. **Never use anonymous namespaces in headers** - each includer gets a copy
4. **Prefer `constexpr` over `#define`** - type-safe and scoped
5. **Use `inline constexpr` for constants in headers** - shared across translation units
6. **Anonymous namespaces replace C's `static` keyword** - more C++-idiomatic

### Scope Summary

```cpp
// Global (avoid)
constexpr float TWO_PI = 6.28318f;  // Visible everywhere

// Namespace (good for shared constants)
namespace Math {
  constexpr float TWO_PI = 6.28318f;  // Math::TWO_PI
}

// Anonymous namespace (best for implementation details)
namespace {
  constexpr float TWO_PI = 6.28318f;  // Only this file
}

// Class member (when it belongs to the class)
class Oscillator {
  static constexpr float TWO_PI = 6.28318f;  // Oscillator::TWO_PI
};
```

Choose based on who needs access to the constant!
