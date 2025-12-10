# Integer Types

Quick reference for choosing the right integer type in C++.

## The Basics

### int vs int32_t
`int` is **architecture-dependent** - only guaranteed to be at least 16 bits:
```cpp
int x = 42;        // Usually 32-bit, but NOT guaranteed
int32_t y = 42;    // ALWAYS exactly 32 bits
```

### size_t
**Platform-dependent unsigned type** for sizes and indices:
- 32-bit system → `size_t` is 32-bit
- 64-bit system → `size_t` is 64-bit
- Always large enough to index any object in memory

---

## When to Use Each Type

### size_t - for sizes, counts, array indices
```cpp
std::vector<int> v;
size_t count = v.size();
for (size_t i = 0; i < count; i++) {}
```

**Why?**
- No casting with standard library (`.size()`, `.reserve()`, etc.)
- Matches platform word size → native performance
- Unsigned is natural (sizes/indices are never negative)

**The gotcha - unsigned underflow:**
```cpp
for (size_t i = 10; i >= 0; i--) {}  // INFINITE LOOP! Wraps to SIZE_MAX
```

**Fix:** Use `i > 0` instead, or use signed type for reverse loops.

### uintN_t / intN_t - for fixed-size values
```cpp
#include <cstdint>

uint32_t userId = 12345;         // Unsigned ID, not a size
int32_t temperature = -5;        // Can be negative
int16_t audioSample = 16384;     // WAV PCM sample
uint8_t flags = 0b10101010;      // Bit flags
```

**Use when:**
- Binary file formats (WAV, PNG, etc.)
- Network protocols or serialization
- Cross-platform code where exact size matters

### int - for general arithmetic
```cpp
int sampleRate = 44100;          // Simple parameter
int offset = x - y;              // Signed arithmetic
```

**Use when:**
- General calculations (especially if can go negative)
- Simple parameters/counters
- You don't need portability guarantees

---

## The Downcasting Problem

**The risk:** `size_t` (64-bit) → `int32_t` (32-bit) can lose data if container is huge.

```cpp
size_t count = v.size();                    // Could be > 2 billion
int32_t samples = static_cast<int32_t>(count);  // Data loss if count > INT32_MAX!
```

### Common Approaches

**1. Trust + assertions (most common)**
```cpp
size_t count = v.size();
assert(count <= INT32_MAX);  // Crashes in debug builds if violated
int32_t samples = static_cast<int32_t>(count);
```
- Debug builds catch violations during testing
- Release builds assume it won't happen
- Used in most codebases for performance

**2. Runtime checks (safety-critical)**
```cpp
size_t count = v.size();
if (count > INT32_MAX) {
    throw std::overflow_error("Container too large");
}
int32_t samples = static_cast<int32_t>(count);
```
- Used when data loss is unacceptable (medical, aerospace, financial)
- Small performance cost for the check

**3. Use larger types, downcast at boundaries (best practice)**
```cpp
// Keep size_t internally
size_t totalSamples = SAMPLE_RATE * DURATION * noteCount;

// Check once at meaningful boundary (e.g., WAV format limit)
if (totalSamples > INT32_MAX) {
    std::cerr << "Error: Audio too long for WAV format\n";
    return 1;
}

// Cast only when writing to file
writeInt32(file, static_cast<int32_t>(totalSamples));
```
- Check at domain boundaries, not every cast
- Use knowledge of constraints (WAV files are limited to INT32_MAX anyway)

**4. Domain knowledge (no check needed)**
```cpp
// A 3-second audio clip = 132,300 samples (way below INT32_MAX)
int32_t samples = static_cast<int32_t>(v.size());  // Safe
```
- When you *know* it can't overflow
- Document the assumption

### What Large Projects Do

**Reality:** Most use a mix:
- **Hot paths:** Trust + assertions (performance matters)
- **User input boundaries:** Explicit checks (untrusted data)
- **Internal code:** Domain knowledge + occasional assertions
- **Critical sections:** Runtime checks

Many projects just cast and rely on testing to catch issues. This works because containers rarely get that big in practice.

---

## Quick Reference

**Choose your type:**
- **Sizes/counts/indices** → `size_t`
- **Other unsigned values** → `uint32_t`, `uint64_t`, etc.
- **Binary formats** → `int32_t`, `int16_t`, etc. (exact size)
- **General arithmetic** → `int` or `int32_t`

**Downcasting safely:**
1. Use larger type (`size_t`) internally
2. Check at domain boundaries (WAV format limit, API constraints, etc.)
3. Add assertions in debug builds
4. Only cast at the last moment

---

## Key Takeaways

1. **Default to size_t** - for sizes, counts, indices (no friction with standard library)
2. **Use fixed-size types** - for binary formats where exact size matters
3. **size_t is platform-dependent** - adapts to 32/64-bit systems (this is good!)
4. **Downcast carefully** - check at boundaries, use assertions, or rely on domain knowledge
5. **Watch unsigned underflow** - `size_t` wraps around in reverse loops
