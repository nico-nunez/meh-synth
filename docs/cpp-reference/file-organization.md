# File Organization in C++

## Table of Contents
- [The Basics: Headers vs Implementation](#the-basics-headers-vs-implementation)
- [Namespaces - Avoiding Global Namespace](#namespaces---avoiding-global-namespace)
- [Naming Conventions](#naming-conventions)
- [Summary - Common Conventions](#summary---common-conventions)
- [Headers (.h files)](#headers-h-files)
- [Implementation (.cpp files)](#implementation-cpp-files)
- [When to Use Header-Only](#when-to-use-header-only)
- [When to Split Header/Implementation](#when-to-split-headerimplementation)
- [Include Paths](#include-paths)
- [Directory Structure](#directory-structure)
- [Types and Where They Live](#types-and-where-they-live)
- [Forward Declarations](#forward-declarations)
- [Quick Rules](#quick-rules)
- [Common Gotchas](#common-gotchas)
- [Audio Project Example](#audio-project-example)

---

## The Basics: Headers vs Implementation

C++ typically splits code into two files:
- **`.h` (header)**: Declarations - "what exists"
- **`.cpp` (implementation)**: Definitions - "how it works"

```
MyClass.h    // Interface/declaration
MyClass.cpp  // Implementation
```

**Why?** Faster compilation and separation of interface from implementation.

## Namespaces - Avoiding Global Namespace

**Important:** All your code should be in a namespace, not the global namespace.

**Why?** Prevents name collisions with other libraries. If you and another library both have an `Oscillator` class in the global namespace, they'll conflict.

```cpp
// Bad - global namespace
class Oscillator {  // Could conflict with other libraries
  // ...
};

// Good - your own namespace
namespace MySynth {
  class Oscillator {  // No conflicts!
    // ...
  };
}
```

See [core-concepts.md](./core-concepts.md#namespaces) for detailed namespace information.

### Namespace in Headers and Implementation

**Oscillator.h:**
```cpp
#pragma once

namespace MySynth {

class Oscillator {
public:
  Oscillator(float frequency, float sampleRate);
  void setFrequency(float freq);
  float getFrequency() const;

private:
  float m_frequency;
  float m_sampleRate;
  float m_phaseIncrement;

  void updatePhaseIncrement();
};

}  // namespace MySynth
```

**Oscillator.cpp:**
```cpp
#include "Oscillator.h"

namespace MySynth {

Oscillator::Oscillator(float frequency, float sampleRate)
  : m_frequency(frequency),
    m_sampleRate(sampleRate),
    m_phaseIncrement(frequency / sampleRate)
{}

void Oscillator::setFrequency(float freq) {
  m_frequency = freq;
  updatePhaseIncrement();
}

float Oscillator::getFrequency() const {
  return m_frequency;
}

void Voice::updatePhaseIncrement() {
  m_phaseIncrement = m_frequency / m_sampleRate;
}

}  // namespace MySynth
```

**Using namespaced classes:**
```cpp
// main.cpp
#include "Voice.h"

int main() {
  // Option 1: Fully qualified
  MySynth::Voice voice(440.0f, 48000.0f);

  // Option 2: Using declaration (for specific type)
  using MySynth::Voice;
  Voice voice2(220.0f, 48000.0f);

  // Option 3: Using directive (brings in entire namespace)
  using namespace MySynth;  // OK in .cpp files, NEVER in .h files
  Voice voice3(330.0f, 48000.0f);

  return 0;
}
```

**Rule:** Never put `using namespace` in header files - it pollutes all files that include it.

## Naming Conventions

C++ has no enforced naming conventions, but common practices exist. **Be consistent** within your project.

### File Names

**Most common: Match the class name**

```
Voice.h / Voice.cpp         // Class is named 'Voice'
Synthesizer.h / Synthesizer.cpp
AudioBuffer.h / AudioBuffer.cpp
```

**Variations you'll see:**
- `PascalCase.h` - Most common (matches class name)
- `snake_case.h` - Used in some projects (Linux kernel style)
- `kebab-case.h` - Less common in C++

**Pick one style and stick with it.**

### Namespace Names

**Most common: lowercase or PascalCase**

```cpp
// Lowercase (common for single-word namespaces)
namespace mysynth {
  class Voice {};
}

// PascalCase (common for multi-word or company namespaces)
namespace MySynth {
  class Voice {};
}

// Nested for organization
namespace MyCompany {
  namespace Audio {
    class Voice {};
  }
}

// C++17 nested shorthand
namespace MyCompany::Audio {
  class Voice {};
}
```

**Common patterns:**
- Project/library name: `namespace juce`, `namespace MySynth`
- Company + module: `namespace MyCompany::Audio`
- Single word, lowercase: `namespace audio`

**Avoid:**
- `ALLCAPS` - reserved for macros
- `std` or other standard library names

### Class/Struct Names

**PascalCase (most common)**

```cpp
class Voice {};
class AudioBuffer {};
class WaveformGenerator {};
struct Point {};
```

### Member Variables

**Private members with `m_` prefix (very common)**

```cpp
class Voice {
private:
  float m_frequency;      // m_ = member
  float m_sampleRate;
  int m_noteNumber;
};
```

**Alternatives you'll see:**
- `m_memberName` - Most common, clear distinction
- `memberName_` - Trailing underscore (Google style)
- `memberName` - No prefix (works if you're careful with parameter names)
- `_memberName` - Leading underscore (avoid - reserved in some contexts)

### Static Members

**Prefix with `s_` (common)**

```cpp
class Voice {
private:
  static float s_maxFrequency;  // s_ = static
  float m_frequency;             // m_ = member
};
```

### Function/Method Names

**camelCase or PascalCase**

```cpp
class Voice {
public:
  // camelCase (common)
  void setFrequency(float freq);
  float getFrequency() const;
  void processBlock();

  // PascalCase (less common, but used in some projects)
  void SetFrequency(float freq);
  float GetFrequency() const;
};
```

### Constants

**UPPER_SNAKE_CASE or kCamelCase**

```cpp
// Macro constants (old style)
#define MAX_VOICES 16

// const variables
const int MAX_VOICES = 16;
const float SAMPLE_RATE = 48000.0f;

// Or with k prefix (Google style)
const int kMaxVoices = 16;
const float kSampleRate = 48000.0f;

// Modern: constexpr
constexpr int MAX_VOICES = 16;
constexpr float SAMPLE_RATE = 48000.0f;
```

### Local Variables and Parameters

**camelCase or snake_case**

```cpp
void processAudio(float* outputBuffer, int numSamples) {
  float currentSample = 0.0f;
  int bufferIndex = 0;

  // Or snake_case
  float current_sample = 0.0f;
  int buffer_index = 0;
}
```

## Summary - Common Conventions

| Item | Convention | Example |
|------|-----------|---------|
| **Files** | Match class name | `Voice.h`, `Voice.cpp` |
| **Namespaces** | lowercase or PascalCase | `mysynth`, `MySynth` |
| **Classes/Structs** | PascalCase | `Voice`, `AudioBuffer` |
| **Member variables** | `m_camelCase` | `m_frequency`, `m_sampleRate` |
| **Static members** | `s_camelCase` | `s_maxVoices` |
| **Methods** | camelCase | `setFrequency()`, `processBlock()` |
| **Constants** | UPPER_SNAKE_CASE or kCamelCase | `MAX_VOICES`, `kMaxVoices` |
| **Parameters/locals** | camelCase or snake_case | `sampleRate`, `sample_rate` |

**Most important:** Pick a style and be consistent throughout your project.

## Headers (.h files)

Headers declare what exists. They're `#include`d by other files.

**MyClass.h:**
```cpp
#pragma once  // Prevent multiple inclusion

class MyClass {
public:
  MyClass(int value);
  void doSomething();
  int getValue() const;

private:
  int m_value;
};
```

**Key rule**: Always use `#pragma once` or include guards at the top.

### Include Guards (alternative to #pragma once)

```cpp
#ifndef MYCLASS_H
#define MYCLASS_H

// Your code here

#endif
```

`#pragma once` is simpler and widely supported. Use it.

## Implementation (.cpp files)

Implementation files define how things work.

**MyClass.cpp:**
```cpp
#include "MyClass.h"

MyClass::MyClass(int value) : m_value(value) {
  // Constructor implementation
}

void MyClass::doSomething() {
  // Method implementation
}

int MyClass::getValue() const {
  return m_value;
}
```

**The `::` operator**: "Scope resolution" - `MyClass::doSomething()` means "the `doSomething` method that belongs to `MyClass`"

## When to Use Header-Only

Some things MUST stay in headers or are commonly kept there:

### Must Be in Header
- Template definitions
- `inline` functions
- `constexpr` functions

### Commonly Header-Only
- Simple structs
- Short inline methods
- Header-only libraries

**Example - Simple struct (header-only):**
```cpp
#pragma once

struct Point {
  float x = 0.0f;
  float y = 0.0f;

  Point() = default;
  Point(float x_, float y_) : x(x_), y(y_) {}

  // Simple method - keep in header
  float distance() const {
    return std::sqrt(x * x + y * y);
  }
};
```

## When to Split Header/Implementation

Split when:
- Methods have substantial logic (more than 2-3 lines)
- You want faster compilation (changing .cpp doesn't recompile everything)
- Implementation details should be hidden

**Voice.h:**
```cpp
#pragma once

struct Voice {
  float frequency;
  float phaseValue;
  float phaseIncrement;

  Voice(float freq, float sampleRate);
  void reset();
  float getNextSample();  // Complex logic
};
```

**Voice.cpp:**
```cpp
#include "Voice.h"
#include <cmath>

Voice::Voice(float freq, float sampleRate)
  : frequency(freq),
    phaseValue(0.0f),
    phaseIncrement(freq / sampleRate)
{}

void Voice::reset() {
  phaseValue = 0.0f;
}

float Voice::getNextSample() {
  // Complex implementation here
  float sample = std::sin(phaseValue * 2.0f * M_PI);
  phaseValue += phaseIncrement;
  if (phaseValue >= 1.0f) {
    phaseValue -= 1.0f;
  }
  return sample;
}
```

## Include Paths

### Quote includes - Your project files
```cpp
#include "MyClass.h"      // Same directory
#include "utils/Helper.h" // Subdirectory
```

### Angle brackets - System/library files
```cpp
#include <vector>
#include <cmath>
#include <string>
```

## Directory Structure

Where to put headers and source files? Several common patterns exist:

### 1. Separate include/ and src/ (Libraries)

```
project/
  include/
    MyLibrary/
      Voice.h
      Synthesizer.h
      AudioBuffer.h
  src/
    Voice.cpp
    Synthesizer.cpp
    AudioBuffer.cpp
  examples/
    main.cpp
```

**When to use:**
- Building a library others will use
- Need to distribute headers separately from source
- Want clean public API separation

**Include style:**
```cpp
#include <MyLibrary/Voice.h>  // Public header
```

### 2. Headers and Source Together (Small applications)

```
project/
  src/
    Voice.h
    Voice.cpp
    Synthesizer.h
    Synthesizer.cpp
    main.cpp
```

**When to use:**
- Small projects (< 10 files)
- Simple applications
- Quick prototypes

**Downsides:**
- Gets messy with many files
- Hard to see organization at a glance

### 3. src/ with Subfolders (Applications - Recommended)

```
project/
  src/
    main.cpp

    audio/
      Voice.h
      Voice.cpp
      AudioBuffer.h
      AudioBuffer.cpp

    dsp/
      Oscillator.h
      Oscillator.cpp
      Filter.h
      Filter.cpp

    utils/
      WavWriter.h
      WavWriter.cpp
```

**When to use:**
- Applications (not libraries)
- Medium to large projects
- When you want logical grouping
- **Recommended if you dislike flat directories**

**Include style:**
```cpp
#include "audio/Voice.h"
#include "dsp/Oscillator.h"
#include "utils/WavWriter.h"
```

### 4. JUCE Style (Audio plugins/apps)

```
MyPlugin/
  Source/
    PluginProcessor.h
    PluginProcessor.cpp
    PluginEditor.h
    PluginEditor.cpp

    Voice.h
    Voice.cpp

    dsp/
      Oscillator.h
      Oscillator.cpp

  JuceLibraryCode/
  MyPlugin.jucer
```

**When to use:**
- JUCE projects (Projucer expects this)
- Audio plugins
- When you'll use JUCE build tools

## Choosing a Structure

**Recommendations based on project type:**

### Application (like your sine wave generator)

**Option A: src/ with subfolders (Recommended)**
```
sine-wav-generator/
  src/
    main.cpp

    synth/
      Voice.h
      Voice.cpp
      Synthesizer.h
      Synthesizer.cpp

    utils/
      WavWriter.h
      WavWriter.cpp

  docs/
  build/
```

**Why:**
- Scales well as you add features
- Logical grouping (synth code vs utility code)
- Easy to navigate
- Similar to JUCE approach (good practice)
- Avoids flat directory chaos

**As it grows:**
```
src/
  main.cpp

  synth/
    Voice.h
    Voice.cpp
    Synthesizer.h
    Synthesizer.cpp
    VoiceManager.h
    VoiceManager.cpp

  dsp/
    Oscillator.h
    Oscillator.cpp
    Filter.h
    Filter.cpp
    Envelope.h
    Envelope.cpp

  utils/
    WavWriter.h
    WavWriter.cpp
    MathHelpers.h
    MathHelpers.cpp
```

**Option B: Flat src/ (Simple projects only)**
```
sine-wav-generator/
  src/
    main.cpp
    Voice.h
    Voice.cpp
    WavWriter.h
    WavWriter.cpp
```

**When to use:** Very small projects (< 10 files), quick prototypes

### Library (for others to use)

**Recommended: Separate include/ and src/**
```
AudioLibrary/
  include/
    AudioLib/
      Voice.h
      AudioBuffer.h
      Synthesizer.h

  src/
    Voice.cpp
    AudioBuffer.cpp
    Synthesizer.cpp

  examples/
    example1.cpp
    example2.cpp

  tests/
  docs/
  CMakeLists.txt
```

**Why:**
- Clear public API (include/ folder)
- Can distribute headers without source
- Users include with: `#include <AudioLib/Voice.h>`
- Professional library structure

**With submodules:**
```
AudioLibrary/
  include/
    AudioLib/
      Core/
        Voice.h
        AudioBuffer.h
      DSP/
        Oscillator.h
        Filter.h
      Utils/
        Math.h

  src/
    Core/
      Voice.cpp
      AudioBuffer.cpp
    DSP/
      Oscillator.cpp
      Filter.cpp
    Utils/
      Math.cpp
```

### JUCE Plugin

**Projucer-generated structure:**
```
MyPlugin/
  Source/
    PluginProcessor.h
    PluginProcessor.cpp
    PluginEditor.h
    PluginEditor.cpp

    Voice.h
    Voice.cpp

    dsp/
      Oscillator.h
      Oscillator.cpp

  JuceLibraryCode/
  Builds/
  MyPlugin.jucer
```

**Why:** JUCE Projucer expects this, standard for JUCE projects

## Include Paths with Subfolders

When using subfolders, configure your build system to search from `src/`:

**CMakeLists.txt example:**
```cmake
target_include_directories(MyApp PRIVATE src)
```

**Then include from project root:**
```cpp
// In any file
#include "synth/Voice.h"      // Clear where it comes from
#include "dsp/Oscillator.h"
#include "utils/WavWriter.h"
```

**Not:**
```cpp
#include "Voice.h"  // Which Voice? From where?
#include "../synth/Voice.h"  // Relative paths = messy
```

## Types and Where They Live

Types (classes, structs, enums) are typically:

### One type per file (common for complex types)
```
Voice.h / Voice.cpp
Synthesizer.h / Synthesizer.cpp
```

### Multiple related types in one file
```cpp
// AudioTypes.h
#pragma once

struct Voice { /* ... */ };
struct Envelope { /* ... */ };
struct LFO { /* ... */ };
```

### Nested types (when only used internally)
```cpp
class Synthesizer {
public:
  struct Voice {  // Only used by Synthesizer
    float frequency;
    float phase;
  };

private:
  Voice voices[8];
};
```

## Forward Declarations

Sometimes you can avoid `#include` by forward declaring:

**Bad (forces recompile of users when Voice.h changes):**
```cpp
// Synthesizer.h
#include "Voice.h"

namespace MySynth {

class Synthesizer {
  Voice* voices;  // Just using a pointer
};

}
```

**Better:**
```cpp
// Synthesizer.h
namespace MySynth {

class Voice;  // Forward declaration

class Synthesizer {
  Voice* voices;  // Don't need full definition for pointers
};

}  // namespace MySynth
```

**Synthesizer.cpp needs the full definition:**
```cpp
#include "Synthesizer.h"
#include "Voice.h"  // Now we need it

namespace MySynth {

// Implementation...

}  // namespace MySynth
```

**Note:** Forward declarations must be in the same namespace as the actual class.

**When you can forward declare:**
- Pointers or references to the type
- Function parameters (by pointer/reference)
- Return types (by pointer/reference)

**When you CANNOT forward declare:**
- Members by value
- Inheritance
- Using methods/members of the type
- Templates

## Quick Rules

1. **Use namespaces**: All your code should be in a namespace, not global
2. **Headers**: Declarations only (usually)
3. **Implementation**: Definitions and logic
4. **Always `#pragma once`** at top of headers
5. **Include what you use**: If your .cpp uses `std::vector`, include `<vector>`
6. **Quote vs angle brackets**: Quote for your files, angle for system/libraries
7. **Forward declare when possible**: Reduces compilation dependencies
8. **Never `using namespace` in headers**: Pollutes all files that include it

## Common Gotchas

**1. Forgot `#pragma once`** → Multiple definition errors
```cpp
#pragma once  // Don't forget this!
```

**2. Implementation in header without `inline`** → Multiple definition errors
```cpp
// MyClass.h
inline void myFunction() {  // Need 'inline' if defined in header
  // ...
}
```

**3. Circular includes** → Compilation errors
```cpp
// A.h includes B.h
// B.h includes A.h
// Solution: Use forward declarations
```

**4. Missing includes in .cpp** → Compilation errors
```cpp
// MyClass.cpp
#include "MyClass.h"
#include <cmath>  // Don't forget what you use!
```

## Audio Project Example

For a simple synth:

```
sine-wav-generator/
  src/
    main.cpp
    Voice.h          // Class (needs encapsulation)
    Voice.cpp        // Implementation
    Synthesizer.h    // Complex class
    Synthesizer.cpp  // Implementation
```

**Voice.h**:
```cpp
#pragma once

namespace MySynth {

class Voice {
public:
  Voice(float frequency, float sampleRate);

  void setFrequency(float freq);
  void setSampleRate(float sampleRate);
  float getFrequency() const;

  void reset();

private:
  float m_frequency;
  float m_sampleRate;
  float m_phaseValue;
  float m_phaseIncrement;

  void updatePhaseIncrement();
};

}  // namespace MySynth
```

**Voice.cpp**:
```cpp
#include "Voice.h"

namespace MySynth {

Voice::Voice(float frequency, float sampleRate)
  : m_frequency(frequency),
    m_sampleRate(sampleRate),
    m_phaseValue(0.0f),
    m_phaseIncrement(frequency / sampleRate)
{}

void Voice::setFrequency(float freq) {
  m_frequency = freq;
  updatePhaseIncrement();
}

void Voice::setSampleRate(float sampleRate) {
  m_sampleRate = sampleRate;
  updatePhaseIncrement();
}

float Voice::getFrequency() const {
  return m_frequency;
}

void Voice::reset() {
  m_phaseValue = 0.0f;
}

void Voice::updatePhaseIncrement() {
  m_phaseIncrement = m_frequency / m_sampleRate;
}

}  // namespace MySynth
```

**Synthesizer.h**:
```cpp
#pragma once
#include "Voice.h"
#include <vector>

namespace MySynth {

class Synthesizer {
public:
  Synthesizer(float sampleRate);
  void processBlock(float* output, int numSamples);

private:
  float m_sampleRate;
  std::vector<Voice> m_voices;
};

}  // namespace MySynth
```

**Synthesizer.cpp**:
```cpp
#include "Synthesizer.h"
#include <cmath>

namespace MySynth {

Synthesizer::Synthesizer(float sampleRate)
  : m_sampleRate(sampleRate)
{}

void Synthesizer::processBlock(float* output, int numSamples) {
  // Implementation...
}

}  // namespace MySynth
```

**main.cpp**:
```cpp
#include "Synthesizer.h"

int main() {
  using MySynth::Synthesizer;

  Synthesizer synth(48000.0f);
  // Use synth...

  return 0;
}
```
