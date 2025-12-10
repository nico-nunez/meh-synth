# Arrays and Containers

Raw arrays vs std::vector, buffer safety, and when to use each.

## Table of Contents
- [Raw Arrays](#raw-arrays)
- [std::vector](#stdvector)
- [Performance Comparison](#performance-comparison)
- [std::string vs char arrays](#stdstring-vs-char-arrays)

---

## Raw Arrays

### Declaration and Usage

```cpp
int arr[10];        // Array of 10 ints: indexes 0-9
arr[0] = 5;         // ✓ Valid - first element
arr[9] = 10;        // ✓ Valid - last element
arr[15] = 20;       // ✗ OUT OF BOUNDS - undefined behavior!
```

### The Problem: No Bounds Checking

**C++ doesn't check array bounds.** It calculates the memory address and writes there, even if it's past the array end.

```cpp
int buffer[10];
buffer[15] = 20;
// Calculates: buffer_start + (15 * sizeof(int))
// Writes to that memory location
// Corrupts whatever was there!
```

### When to Use Raw Arrays

- Fixed-size arrays known at compile time
- Embedded systems with no std library
- Performance-critical code (after profiling!)
- Interfacing with C APIs

```cpp
// Reasonable use cases:
int rgb[3] = {255, 0, 0};        // Small, fixed size
char buffer[256];                 // Stack buffer for C API
```

---

## std::vector

### Declaration and Usage

```cpp
std::vector<int> vec(10);    // Vector of 10 ints
vec[0] = 5;                  // Fast - no bounds check
vec.at(0) = 5;               // Safe - throws on out-of-bounds
vec.push_back(42);           // Grows automatically
```

### Benefits Over Raw Arrays

1. **Dynamic size** - grows/shrinks as needed
2. **Automatic memory management** - no manual delete
3. **Optional bounds checking** - `.at()` throws exceptions
4. **Can be returned/passed easily** - arrays decay to pointers
5. **Knows its size** - `.size()` method

### Common Operations

```cpp
std::vector<int> v;

// Adding elements
v.push_back(1);           // Add to end
v.emplace_back(2);        // Construct in place
v.insert(v.begin(), 0);   // Insert at position

// Access
v[0];                     // Fast, no bounds check
v.at(0);                  // Safe, throws if out of bounds
v.front();                // First element
v.back();                 // Last element

// Size
v.size();                 // Number of elements
v.empty();                // True if size() == 0
v.capacity();             // Allocated capacity

// Removal
v.pop_back();             // Remove last element
v.clear();                // Remove all elements

// Reserve space (avoid reallocations)
v.reserve(1000);          // Pre-allocate space
```

---

## Performance Comparison

### Memory Overhead

```cpp
int arr[1000];           // 4000 bytes (1000 * 4)
std::vector<int> vec(1000);  // 4000 bytes + 24 bytes overhead (3 pointers)
```

Overhead is negligible for any reasonably-sized container.

### Access Speed

```cpp
arr[i] = 5;              // Direct memory access
vec[i] = 5;              // Same - direct memory access
vec.at(i) = 5;           // Adds bounds check
```

**operator[]:** Same speed for both
**at():** Slightly slower due to bounds check

### When Raw Arrays Might Win

- Very small fixed arrays (< 100 bytes) on stack
- Avoiding heap allocation in tight loops
- After profiling proves it matters

### Real-World Advice

**Use `std::vector` by default.** The safety is worth it, and performance is nearly identical.

Only use raw arrays when:
1. Profiling shows a bottleneck
2. You need stack allocation for tiny, fixed arrays
3. Interfacing with C code

---

## std::string vs char arrays

### The Dangerous Way

```cpp
char buffer[100];
strcpy(buffer, userInput);    // Buffer overflow risk!
strcat(buffer, " world");     // More overflow risk
```

**Problems:**
- Fixed size
- No bounds checking
- Manual null terminator management
- Easy to overflow

### The Safe Way

```cpp
std::string str = userInput;
str += " world";              // Safe, grows automatically
```

**Benefits:**
- Dynamic sizing
- Automatic memory management
- Rich API (find, substr, replace, etc.)
- No null terminator bugs
- Small String Optimization (SSO) - strings ≤ 15-23 chars stored inline

### Performance

```cpp
std::string small = "hello";     // Stack (SSO) - very fast
std::string large = "very long...";  // Heap - still fast
```

For strings ≤ ~15-23 characters, `std::string` stores data inline (no heap allocation). Often **faster** than char arrays due to fewer allocations.

---

## Buffer Overflow Examples

### Integer Assignment - Safe
```cpp
int32_t x = 999999999999;     // Just truncates to 32 bits - no corruption
```

Single variables can't overflow their memory - type size is fixed.

### Array Access - Dangerous
```cpp
int buffer[10];
buffer[15] = 123;             // Writes past end - corrupts memory!

char str[5];
strcpy(str, "toolong");       // Writes 8 bytes into 5-byte space!
```

**This is how exploits happen.** Attackers craft input to overflow buffers and execute malicious code.

### The Safe Alternative

```cpp
std::vector<int> buffer(10);
buffer.at(15) = 123;          // Throws std::out_of_range exception

std::string str = "toolong";  // Automatically sized - no overflow
```

---

## Key Takeaways

1. **Use `std::vector` instead of raw arrays** unless you have a specific reason
2. **Use `std::string` instead of char arrays** - always
3. **Use `.at()` during development** for bounds checking, switch to `[]` only if profiling shows it matters
4. **Buffer overflows are the real danger** - not integer overflow
5. **Trust modern C++** - the standard library is highly optimized

---

## Coming Up

- std::array (fixed-size safe alternative to C arrays)
- std::span (safe view into arrays/vectors)
- Range-based for loops
- Iterator invalidation
