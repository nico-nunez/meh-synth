# Structs and Constructors in C++

## struct vs class

**The only difference**: default access level
- `struct`: members are **public** by default
- `class`: members are **private** by default

Everything else is identical - both can have constructors, methods, inheritance, etc.

```cpp
struct MyStruct {
  int x;  // public by default
};

class MyClass {
  int x;  // private by default
};
```

## Convention

- **struct**: Plain data structures, mostly public members
- **class**: When you need encapsulation/private implementation

## C++ Terminology

In C++, use these terms:
- **Member variable** / **Data member** = variables in a class/struct (not "property")
- **Member function** / **Method** = functions in a class/struct
- **Member** = general term for both

```cpp
struct Voice {
  float frequency;  // Member variable / data member

  void reset() {    // Member function / method
    frequency = 0.0f;
  }
};
```

## Constructors

Constructors are special functions that initialize objects. They have the same name as the struct/class and no return type.

### Basic Constructor

```cpp
struct Voice {
  float frequency;
  float phaseValue;

  // Constructor
  Voice(float freq) {
    frequency = freq;
    phaseValue = 0.0f;
  }
};

// Usage
Voice myVoice(440.0f);
```

### Member Initializer List (Preferred)

The `: member(value), ...` syntax after the parameter list:

```cpp
struct Voice {
  float frequency;
  float phaseValue;
  float phaseIncrement;

  Voice(float freq, float sampleRate)
    : frequency(freq),           // Initialize frequency
      phaseValue(0.0f),          // Initialize phaseValue
      phaseIncrement(freq / sampleRate)  // Calculate and initialize
  {
    // Body can be empty or contain additional logic
  }
};
```

**Why use initializer lists?**
- More efficient (direct initialization vs assignment)
- Required for `const` members and references
- Clearer intent

**Important**: Members are initialized in **declaration order**, not the order in the initializer list.

### Multiple Constructors

You can have multiple constructors (overloading):

```cpp
struct Voice {
  float frequency;
  float phaseValue;

  // Default constructor
  Voice() : frequency(440.0f), phaseValue(0.0f) {}

  // Constructor with frequency
  Voice(float freq) : frequency(freq), phaseValue(0.0f) {}

  // Constructor with both
  Voice(float freq, float phase) : frequency(freq), phaseValue(phase) {}
};

Voice v1;              // Uses default constructor
Voice v2(220.0f);      // Uses second constructor
Voice v3(330.0f, 0.5f); // Uses third constructor
```

### Optional Constructor Parameters

Use default parameter values for optional parameters:

```cpp
struct Voice {
  float frequency;
  float phaseValue;
  float phaseIncrement;

  // sampleRate has a default value
  Voice(float freq, float sampleRate = 48000.0f)
    : frequency(freq),
      phaseValue(0.0f),
      phaseIncrement(freq / sampleRate)
  {}
};

Voice v1(440.0f);           // Uses default sampleRate (48000.0f)
Voice v2(440.0f, 44100.0f); // Overrides sampleRate
```

**Rules:**
- Default parameters must be at the **end** of the parameter list
- Can't have required params after optional ones

```cpp
// Good
Voice(float freq, float sr = 48000.0f, float phase = 0.0f);

// Bad - required param after optional
Voice(float freq = 440.0f, float sr);  // Error!
```

**Gotcha:** You can't skip middle parameters:

```cpp
Voice v(220.0f, /* can't skip sampleRate */, 0.8f);  // Not possible!
```

If you need that flexibility, use multiple constructor overloads instead.

### Default Member Initializers (C++11+)

You can provide default values directly on member variables:

```cpp
struct Voice {
  float frequency = 440.0f;
  float phaseValue = 0.0f;
  float phaseIncrement = 0.0f;

  // Constructor only needs to set what's different
  Voice(float freq, float sampleRate)
    : frequency(freq),
      phaseIncrement(freq / sampleRate)
  {
    // phaseValue uses default (0.0f)
  }
};
```

**Default constructor gets initialized values automatically:**

```cpp
struct Voice {
  float frequency = 440.0f;
  float phaseValue = 0.0f;
};

Voice v;  // frequency is 440.0f, phaseValue is 0.0f
```

**When to use default member initializers vs constructor:**
- **Simple constants** → Default member initializers (`float gain = 1.0f;`)
- **Zero/null values** → Default member initializers (`float phase = 0.0f;`)
- **Calculated from parameters** → Constructor initializer list
- **Multiple constructors with same defaults** → Default member initializers

**Best of both:**

```cpp
struct Voice {
  // Defaults for simple values
  float phaseValue = 0.0f;
  float gain = 1.0f;

  // Set in constructor
  float frequency;
  float sampleRate;
  float phaseIncrement;

  Voice(float freq, float sr)
    : frequency(freq),
      sampleRate(sr),
      phaseIncrement(freq / sr)
  {
    // phaseValue and gain use their defaults
  }
};
```

## Default Constructor

If you don't write any constructors, C++ provides a default constructor that does nothing (leaves members uninitialized for primitive types).

```cpp
struct Point {
  float x;
  float y;
};

Point p;  // x and y are UNINITIALIZED (garbage values)
```

**Gotcha**: If you write *any* constructor, the default constructor is no longer auto-generated.

```cpp
struct Point {
  float x;
  float y;

  Point(float x_, float y_) : x(x_), y(y_) {}
};

Point p;  // ERROR! No default constructor available
Point p(1.0f, 2.0f);  // OK
```

To keep it, explicitly define it:

```cpp
struct Point {
  float x;
  float y;

  Point() : x(0.0f), y(0.0f) {}  // Default constructor
  Point(float x_, float y_) : x(x_), y(y_) {}
};
```

Or use `= default`:

```cpp
struct Point {
  float x = 0.0f;
  float y = 0.0f;

  Point() = default;  // Use compiler-generated default constructor
  Point(float x_, float y_) : x(x_), y(y_) {}
};
```

## Methods in Structs

Structs can have methods just like classes:

```cpp
struct Voice {
  float frequency;
  float phaseValue;

  Voice(float freq) : frequency(freq), phaseValue(0.0f) {}

  // Method
  void reset() {
    phaseValue = 0.0f;
  }

  // Method that returns a value
  float getCurrentPhase() const {  // 'const' means it doesn't modify the object
    return phaseValue;
  }
};

Voice v(440.0f);
v.reset();
float phase = v.getCurrentPhase();
```

## Common Gotchas

1. **Uninitialized members**: Always initialize primitive types (int, float, etc.)
2. **Declaration order matters**: Initializer list should match declaration order
3. **No default constructor after custom constructor**: Explicitly add one if needed
4. **Forgot return type?**: Constructors have NO return type (not even `void`)

## Quick Reference

```cpp
struct Example {
  // Members with defaults
  int value = 0;
  float data = 1.0f;

  // Default constructor
  Example() = default;

  // Custom constructor with initializer list
  Example(int v, float d) : value(v), data(d) {}

  // Method
  void doSomething() {
    // ...
  }

  // Const method (doesn't modify object)
  int getValue() const {
    return value;
  }
};
```
