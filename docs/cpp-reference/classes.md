# Classes in C++

## class vs struct

The **only** difference: default access level
- `class`: members are **private** by default
- `struct`: members are **public** by default

Everything else is identical. Use `class` when you need encapsulation.

## Basic Class

```cpp
class Synthesizer {
public:
  Synthesizer(float sampleRate);
  void processBlock(float* output, int numSamples);
  void setFrequency(float freq);

private:
  float m_sampleRate;
  float m_frequency;
  float m_phase;
};
```

## Access Specifiers

Control who can access members:

```cpp
class MyClass {
public:
  // Accessible from anywhere
  void publicMethod();
  int publicData;

private:
  // Only accessible inside this class
  void privateMethod();
  int m_privateData;

protected:
  // Accessible in this class and derived classes
  void protectedMethod();
  int m_protectedData;
};
```

### When to Use Each

**public** - The interface
- Constructors
- Methods that users of your class need to call
- Rarely: data members (usually provide methods instead)

**private** - Implementation details (default here!)
- Internal state/data members
- Helper methods users don't need to know about
- Anything you might want to change later

**protected** - Inheritance
- Rarely used (only needed for inheritance)
- Members that derived classes need but users don't

### Why Use private?

**1. Encapsulation** - Hide implementation so you can change it later:

```cpp
class AudioBuffer {
public:
  float getSample(int index);  // Interface stays the same

private:
  float* m_data;  // Could change to std::vector later without breaking user code
};
```

**2. Validation** - Control how data is set:

```cpp
class Oscillator {
public:
  void setFrequency(float freq) {
    if (freq > 0.0f && freq < 20000.0f) {  // Validate!
      m_frequency = freq;
    }
  }

private:
  float m_frequency;  // Can't be set to invalid values from outside
};
```

**3. Maintain Invariants** - Keep object in valid state:

```cpp
class Voice {
public:
  void updatePhase() {
    m_phase += m_phaseIncrement;
    if (m_phase >= 1.0f) {
      m_phase -= 1.0f;  // Always keep phase 0.0-1.0
    }
  }

private:
  float m_phase;           // If public, user could break invariant
  float m_phaseIncrement;
};
```

**4. Clear Interface** - Users only see what matters:

```cpp
class Synthesizer {
public:
  // Clean, simple interface
  void processBlock(float* output, int numSamples);
  void setFrequency(float freq);

private:
  // Users don't need to see this complexity
  float m_sampleRate;
  float m_phase;
  float m_frequency;

  void updatePhase();
  void calculatePhaseIncrement();
  float generateSample();
};
```

### Bad vs Good Examples

**Bad - Everything public:**

```cpp
struct Synthesizer {
  float phase;
  float frequency;
  float sampleRate;
};

Synthesizer synth;
synth.phase = 999.0f;  // Oops! Invalid value
synth.frequency = -100.0f;  // Oops! Negative frequency
```

**Good - Private with controlled access:**

```cpp
class Synthesizer {
public:
  Synthesizer(float sampleRate) : m_sampleRate(sampleRate), m_phase(0.0f) {}

  void setFrequency(float freq) {
    if (freq > 0.0f) {  // Validation
      m_frequency = freq;
    }
  }

  void reset() {
    m_phase = 0.0f;  // Always valid
  }

private:
  float m_sampleRate;
  float m_frequency;
  float m_phase;  // Can't be set to invalid values
};
```

### Rule of Thumb

**Start with everything private, only make public what's necessary.**

1. **Always private**: Internal state, implementation details
2. **Usually private**: Helper methods
3. **Public**: Constructors, interface methods
4. **Avoid public data**: Use getters/setters for controlled access

## Naming Convention: m_ Prefix

Common convention: prefix private member variables with `m_` (for "member"):

```cpp
class Synthesizer {
public:
  void setFrequency(float frequency) {
    m_frequency = frequency;  // m_ prefix = member variable
  }

private:
  float m_sampleRate;   // Clear it's a member variable
  float m_frequency;
  float m_phase;
};
```

**Why?**
- Distinguishes members from local variables
- Makes code more readable
- Not required, just a common style

## Header vs Implementation

### Header (.h) - Interface

```cpp
#pragma once

class Synthesizer {
public:
  Synthesizer(float sampleRate);
  void processBlock(float* output, int numSamples);
  void setFrequency(float freq);
  float getFrequency() const;

private:
  float m_sampleRate;
  float m_frequency;
  float m_phase;

  void updatePhase();  // Private helper method
};
```

### Implementation (.cpp) - Definitions

```cpp
#include "Synthesizer.h"
#include <cmath>

Synthesizer::Synthesizer(float sampleRate)
  : m_sampleRate(sampleRate),
    m_frequency(440.0f),
    m_phase(0.0f)
{}

void Synthesizer::processBlock(float* output, int numSamples) {
  for (int i = 0; i < numSamples; ++i) {
    output[i] = std::sin(m_phase * 2.0f * M_PI);
    updatePhase();
  }
}

void Synthesizer::setFrequency(float freq) {
  m_frequency = freq;
}

float Synthesizer::getFrequency() const {
  return m_frequency;
}

void Synthesizer::updatePhase() {
  m_phase += m_frequency / m_sampleRate;
  if (m_phase >= 1.0f) {
    m_phase -= 1.0f;
  }
}
```

**The `::` operator**: Scope resolution - `Synthesizer::processBlock` means "the `processBlock` method that belongs to `Synthesizer`"

## Constructors and Destructors

### Constructor

Initializes the object:

```cpp
class Synthesizer {
public:
  // Constructor with member initializer list
  Synthesizer(float sampleRate)
    : m_sampleRate(sampleRate),
      m_frequency(440.0f),
      m_phase(0.0f)
  {}

private:
  float m_sampleRate;
  float m_frequency;
  float m_phase;
};

Synthesizer synth(48000.0f);  // Calls constructor
```

### Destructor

Cleans up when object is destroyed:

```cpp
class AudioBuffer {
public:
  AudioBuffer(int size) {
    m_data = new float[size];  // Allocate memory
    m_size = size;
  }

  ~AudioBuffer() {  // Destructor (note the ~)
    delete[] m_data;  // Free memory
  }

private:
  float* m_data;
  int m_size;
};

// Object destroyed automatically when out of scope
{
  AudioBuffer buffer(1024);  // Constructor called
  // Use buffer...
}  // Destructor called automatically here
```

**Destructor rules:**
- Same name as class with `~` prefix
- No parameters, no return type
- Called automatically when object is destroyed
- Use for cleanup (free memory, close files, etc.)

## const Methods

Methods that don't modify the object should be marked `const`:

```cpp
class Synthesizer {
public:
  // const method - promises not to modify member variables
  float getFrequency() const {
    return m_frequency;
  }

  // Non-const method - can modify members
  void setFrequency(float freq) {
    m_frequency = freq;
  }

private:
  float m_frequency;
};

const Synthesizer synth(440.0f);
float freq = synth.getFrequency();  // OK - const method
synth.setFrequency(220.0f);         // ERROR - can't call non-const method on const object
```

**Best practice:** Mark all methods that don't modify state as `const`.

## Getters and Setters

Common pattern for controlled access to private data:

```cpp
class Voice {
public:
  // Getter - const method
  float getFrequency() const {
    return m_frequency;
  }

  // Setter - validates input
  void setFrequency(float freq) {
    if (freq > 0.0f && freq < 20000.0f) {
      m_frequency = freq;
    }
  }

private:
  float m_frequency = 440.0f;
};
```

**Why not make members public?**
- Can validate input
- Can change internal representation later
- Can add logic (logging, notifications, etc.)

## Static Members

Members that belong to the class itself, not instances:

```cpp
class Synthesizer {
public:
  static float getMaxFrequency() {
    return s_maxFrequency;
  }

  static void setMaxFrequency(float freq) {
    s_maxFrequency = freq;
  }

private:
  static float s_maxFrequency;  // Shared by all instances
  float m_frequency;             // Each instance has its own
};

// Define static member (in .cpp file)
float Synthesizer::s_maxFrequency = 20000.0f;

// Call without an instance
float max = Synthesizer::getMaxFrequency();
```

**Common convention:** Prefix static members with `s_`

## Inline Methods

Short methods can be defined in the header:

```cpp
class Voice {
public:
  float getFrequency() const {
    return m_frequency;  // Simple getter - inline is fine
  }

  void setFrequency(float freq);  // Complex logic - define in .cpp

private:
  float m_frequency;
};
```

Methods defined inside the class declaration are implicitly `inline`.

## this Pointer

Every member function has access to `this`, a pointer to the current object:

```cpp
class Voice {
public:
  void setFrequency(float frequency) {
    this->m_frequency = frequency;  // Explicit use of 'this'
    m_frequency = frequency;         // Same thing, 'this->' is implicit
  }

  Voice& reset() {
    m_phase = 0.0f;
    return *this;  // Return reference to self for chaining
  }

private:
  float m_frequency;
  float m_phase;
};

Voice v(440.0f);
v.reset().setFrequency(220.0f);  // Method chaining
```

## Common Patterns

### Simple Data Class

```cpp
class Point {
public:
  Point(float x, float y) : m_x(x), m_y(y) {}

  float getX() const { return m_x; }
  float getY() const { return m_y; }

  void setX(float x) { m_x = x; }
  void setY(float y) { m_y = y; }

private:
  float m_x;
  float m_y;
};
```

### Class with Resource Management

```cpp
class AudioBuffer {
public:
  AudioBuffer(int size)
    : m_size(size) {
    m_data = new float[size];
  }

  ~AudioBuffer() {
    delete[] m_data;
  }

  float* getData() { return m_data; }
  int getSize() const { return m_size; }

private:
  float* m_data;
  int m_size;
};
```

### Class with Private Helper Methods

```cpp
class Synthesizer {
public:
  Synthesizer(float sampleRate);
  void processBlock(float* output, int numSamples);

private:
  float m_sampleRate;
  float m_phase;

  // Private helpers
  void updatePhase();
  float generateSample();
};
```

## Quick Rules

1. **Use `class`** when you need private members (most of the time)
2. **Use `struct`** for simple data structures with mostly public members
3. **Mark member variables private** and provide getters/setters if needed
4. **Use `m_` prefix** for private member variables (common convention)
5. **Mark methods `const`** if they don't modify state
6. **Split header/implementation** for complex classes
7. **Always use `#pragma once`** in headers

## struct vs class - When to Use Which?

```cpp
// struct - Simple data, mostly public
struct Voice {
  float frequency;
  float phase;

  Voice(float freq) : frequency(freq), phase(0.0f) {}
};

// class - Encapsulation, private implementation
class Synthesizer {
public:
  Synthesizer(float sampleRate);
  void setFrequency(float freq);

private:
  float m_sampleRate;
  float m_frequency;
  Voice m_voice;
};
```

**Rule of thumb:**
- **struct**: Plain data structures, everything public
- **class**: Encapsulation needed, private implementation details
