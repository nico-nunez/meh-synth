# Core Concepts

Quick reference for fundamental C++ concepts, especially useful coming from TypeScript.

## Table of Contents
- [Namespaces](#namespaces)
- [Function Parameters](#function-parameters)
- [Output Formatting](#output-formatting)
- [Assertions](#assertions)
- [When to Optimize](#when-to-optimize)
- [Key Takeaways](#key-takeaways)

---

## Namespaces

### Standard Library Always Needs std::
```cpp
std::vector<int> v;   // ALWAYS need std::
std::cout << "hi";    // ALWAYS need std::
std::string s;        // ALWAYS need std::
```

**Never do this in headers:**
```cpp
using namespace std;  // BAD - pollutes everyone's code
```

### Your Own Code Needs Namespaces Too
```cpp
// myutils.h
namespace myutils {
    void createWavFile();
    void writeInt32(std::ofstream& file, int value);
}  // namespace myutils

// myutils.cpp
namespace myutils {
    void createWavFile() { /* ... */ }
    void writeInt32(std::ofstream& file, int value) { /* ... */ }
}

// main.cpp
myutils::createWavFile();  // Clear where it comes from
```

**Why?** Prevents name conflicts in large projects. Same reason `std::` exists.

### Nested Namespaces
You can nest namespaces for more organization:
```cpp
namespace mycompany {
    namespace audio {
        void createWavFile();
    }
}

// Usage:
mycompany::audio::createWavFile();

// C++17 shorthand:
namespace mycompany::audio {
    void createWavFile();
}
```

---

## Function Parameters

### Pass by Value vs Reference
```cpp
void foo(std::string s);          // COPY - expensive for large objects
void foo(const std::string& s);   // REFERENCE - no copy, read-only
void foo(std::string& s);         // REFERENCE - can modify original
```

**Rule:** For strings/vectors/objects, use `const T&` unless you need to modify.

### The Function Signature is King

**Critical concept:** The function parameter determines HOW the argument is passed, not the caller.

```cpp
void process(Oscillator& osc);  // Function expects a reference

Oscillator original;
Oscillator copy = original;

process(original);  // Passes reference to original
process(copy);      // Passes reference to copy (not original!)
```

**Implication for loops:**
```cpp
// BAD - modifies copies, not originals
for (auto osc : oscillators) {           // osc = COPY
    process(osc);  // Function gets reference to COPY
}

// GOOD - modifies originals
for (auto& osc : oscillators) {          // osc = REFERENCE
    process(osc);  // Function gets reference to ORIGINAL
}
```

**Remember:** The function signature controls what it receives, whether you pass a value or reference. A reference to a copy is still just a reference to a copy!

### Optional Parameters (Default Values)
```cpp
// In .h file - defaults ONLY here
std::ofstream createWavFile(const std::string& filename = "output.wav");

// In .cpp file - NO defaults here
std::ofstream createWavFile(const std::string& filename) {
    return std::ofstream(filename, std::ios::binary);
}
```

**Important:** Default parameters must be rightmost in parameter list.

---

## Output Formatting

### Default Precision
`std::cout` defaults to **6 significant figures** total (not decimal places):
```cpp
double freq = 261.62556530059863;
std::cout << freq;  // Prints: 261.626 (6 significant figures)
```

### Controlling Precision
```cpp
#include <iomanip>

// Set number of digits after decimal point
std::cout << std::fixed << std::setprecision(10) << freq;
// Prints: 261.6255653006

// Set total significant figures
std::cout << std::setprecision(15) << freq;
// Prints: 261.625565300599
```

### Manipulators are Sticky
**Important:** `std::setprecision()` and `std::fixed` persist on the stream:
```cpp
std::cout << std::setprecision(3);
std::cout << 3.14159;  // 3.14
std::cout << 2.71828;  // 2.72  (still using precision 3!)

// Reset to default
std::cout << std::setprecision(6);
```

**Common patterns:**
- `std::fixed` - fixed decimal places (e.g., `123.4500`)
- `std::scientific` - scientific notation (e.g., `1.234500e+02`)
- `std::defaultfloat` - revert to default behavior

---

## Assertions

**Debugging checks that crash if conditions fail** - only active in debug builds.

```cpp
#include <cassert>

size_t count = v.size();
assert(count <= INT32_MAX);  // Crashes in debug if false
int32_t samples = static_cast<int32_t>(count);
```

### How They Work

**Debug builds (`-g`):**
- Check runs at runtime
- If false → program crashes with error message showing file/line
- Helps catch bugs during development

**Release builds (`-O2 -DNDEBUG`):**
- Entire `assert()` is removed (compiled out)
- Zero runtime cost
- Like the line was never there

### Key Points

✅ **Runtime only (debug)** - executes when program runs, not at compile time
✅ **Crashes/aborts (unrecoverable)** - terminates program immediately, no exception to catch
✅ **Removed from release** - need proper `if`/`throw` for production error handling

### When to Use

**Use assertions for:**
```cpp
assert(index < v.size());        // Catch out-of-bounds bugs
assert(ptr != nullptr);          // Catch null pointer bugs
assert(count <= INT32_MAX);      // Catch overflow during testing
```
- Programming errors (bugs in YOUR code)
- Things that "should never happen"
- Sanity checks during development

**DON'T use assertions for:**
```cpp
// BAD - disappears in release!
assert(file.is_open());

// GOOD - always check in production
if (!file.is_open()) {
    std::cerr << "Error: Could not open file\n";
    return 1;
}
```
- User input validation
- File I/O errors
- Any error that can happen in production

### Assertions vs Exceptions

```cpp
// Assertion - catches YOUR bugs during development
assert(count > 0);  // "This should never be zero in correct code"

// Exception - handles runtime errors in production
if (count == 0) {
    throw std::runtime_error("Empty container");
}
```

**Rule of thumb:** If it can fail in production, use proper error handling, not assertions.

### Use Assertions Liberally

**Assertions are cheap (free in release) and catch bugs early** - don't be shy about using them.

**Common patterns for liberal use:**
```cpp
// Preconditions (function inputs)
void processAudio(const std::vector<short>& samples) {
    assert(!samples.empty());
    assert(samples.size() <= INT32_MAX);
    // ...
}

// Before risky operations
assert(divisor != 0);
result = numerator / divisor;

// When making assumptions
assert(notesToGenerate.size() <= INT_MAX);
int count = static_cast<int>(notesToGenerate.size());

// Invariants (state that should always be true)
assert(writePos <= buffer.size());
```

**Why liberal use is good:**
- Acts as **executable documentation** - shows assumptions to code readers
- Catches bugs early during development
- Zero cost in release builds
- Builds confidence in correctness

**When NOT to overuse:**
- Don't assert obvious things just assigned: `int x = 5; assert(x == 5);`
- Don't replace actual error handling for production scenarios
- Don't assert after every line (too much noise)

**Bottom line:** If you're wondering "should I add an assert here?", the answer is probably yes.

---

## When to Optimize

### Don't Optimize Unless:
1. You've profiled and found a real bottleneck
2. It's in a hot path (called millions of times)
3. The optimization makes measurable difference

### Examples of Premature Optimization
- Optimizing file I/O operations (I/O is 1000x slower than any code optimization)
- Adding function overloads for `const char*` vs `const std::string&`
- Using raw arrays instead of `std::vector` without measurements

### Examples of Reasonable Optimization
- Functions in tight loops called millions of times per second
- Hot paths identified by a profiler

---

## Key Takeaways

1. **Include what you use** - in every file
2. **Use namespaces** - prevents conflicts
3. **Pass by const reference** - for non-primitive types
4. **Use std::vector and std::string** - safety > microscopic performance gains
5. **Buffer overflows are the real danger** - not integer overflow
6. **Profile before optimizing** - don't guess

---

**Remember:** C++ gives you control and performance, but trusts you completely. Use modern C++ features (`std::vector`, `std::string`, smart pointers) to get safety without sacrificing much speed.
