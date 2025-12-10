# Common Pitfalls

Things that will bite you coming from TypeScript or other safe languages.

## Table of Contents
- [Integer Overflow](#integer-overflow)
- [Buffer Overflow](#buffer-overflow)
- [Dangling References](#dangling-references)
- [The Trust Problem](#the-trust-problem)

---

## Integer Overflow

### What Happens

```cpp
int32_t value = 2147483647;  // Max value: 0111...1111
value++;                     // Wraps to -2147483648: 1000...0000
```

**For signed integers:** This is **undefined behavior**. Compiler can:
- Wrap around (common)
- Crash your program
- Assume it never happens and optimize incorrectly
- Literally anything (spec says "nasal demons")

**For unsigned integers:** Well-defined wraparound (modulo 2^N).

```cpp
uint32_t uvalue = 4294967295; // Max: 1111...1111
uvalue++;                     // Wraps to 0: 0000...0000 (defined behavior)
```

### How It Works

```
2147483647 = 0111 1111 1111 1111 1111 1111 1111 1111
+        1
─────────────────────────────────────────────────────
2147483648 = 1000 0000 0000 0000 0000 0000 0000 0000
             ↑ Sign bit flipped = negative number
```

The CPU only uses allocated bits (32 for int32_t). If the result needs 33 bits, bit 33 disappears (goes to CPU carry flag, not memory).

### Assignment Truncates Safely

```cpp
int64_t big = 10000000000;
int32_t small = big;  // Keeps lower 32 bits only
```

**No buffer overflow.** The destination variable size is fixed by its type. Only the lower bits are copied.

### How to Prevent

```cpp
// Check before overflow
if (value > INT32_MAX - 1) {
    // Handle error
}

// Use wider types
int64_t bigValue = value;

// Use safe math libraries
// Or compiler flags: -ftrapv (trap on overflow)
```

---

## Buffer Overflow

### The Real Danger

**This is how most security vulnerabilities happen.**

```cpp
int buffer[10];      // Allocates indexes 0-9
buffer[15] = 123;    // OUT OF BOUNDS - corrupts adjacent memory!

char str[5];
strcpy(str, "toolong");  // Writes 8 bytes into 5-byte space!
```

### Why It's Dangerous

C++ doesn't check array bounds. It calculates:
```
address = array_start + (index * element_size)
```

And writes there, even if the index is out of bounds. Whatever was in that memory gets overwritten.

### Integer vs Buffer Overflow

**Integer overflow (safe-ish):**
```cpp
int32_t x = 999999999999;  // Truncates to 32 bits - stays within variable
```

**Buffer overflow (dangerous):**
```cpp
int arr[10];
arr[15] = 5;  // Writes PAST the array into adjacent memory!
```

### Attack Vector

```cpp
void handleInput(const char* userInput) {
    char buffer[100];
    strcpy(buffer, userInput);  // If userInput > 100 chars...
}

// Attacker sends 150 chars, overwrites:
// - Return address on stack
// - Other variables
// - Function pointers
// Can execute arbitrary code!
```

### The Solution

```cpp
// Use std::vector
std::vector<int> buffer(10);
buffer.at(15) = 5;  // Throws exception - caught the bug!

// Use std::string
std::string str = userInput;  // Automatically sized, no overflow
```

---

## Dangling References

### Returning References to Local Variables

```cpp
// DANGER!
int& getBadRef() {
    int x = 42;
    return x;  // Returns reference to local variable
}  // x destroyed here, but reference still points to it!

int& ref = getBadRef();
cout << ref;  // Undefined behavior - accessing freed memory
```

### Returning string_view to Temporaries

```cpp
// DANGER!
std::string_view getBadView() {
    std::string data = "hello";
    return std::string_view(data);  // Viewing local variable
}  // data destroyed, view now dangling

std::string_view sv = getBadView();
cout << sv;  // Undefined behavior - string was destroyed
```

### Storing References to Temporaries

```cpp
// DANGER!
const std::string& ref = std::string("hello");  // Temporary string
// Temporary destroyed at end of statement
cout << ref;  // Undefined behavior
```

### The Safe Alternatives

```cpp
// Return by value - uses move semantics
std::string getData() {
    std::string data = "hello";
    return data;  // Moved to caller - safe!
}

// Return pointer to heap-allocated data (but prefer smart pointers)
std::unique_ptr<int> getPtr() {
    return std::make_unique<int>(42);  // Caller owns it
}

// Pass output parameter by reference
void getData(std::string& out) {
    out = "hello";  // Modifies caller's string
}
```

---

## The Trust Problem

### The Paradox

C++ philosophy: **"We trust you to know what you're doing"**

But this means:
- No bounds checking on arrays (performance)
- No null pointer checks (performance)
- Undefined behavior for many errors (allows optimizations)
- Manual memory management (control)

**The irony:** This "trust" has caused decades of security vulnerabilities.

### Historical Context

C++ (and C) designed in 1970s-80s with priorities:
1. Performance above all else
2. Programmer knows best
3. Close to the hardware
4. Small systems (embedded, OS kernels)

**Result:** Most security bugs in major software (browsers, OSes) trace back to memory safety issues in C/C++.

### Modern Response

**Rust:** "We DON'T trust you" - enforces memory safety at compile time

**Modern C++:** Provides safe alternatives (`std::vector`, `std::string`, smart pointers) - but doesn't force you to use them

**TypeScript/JavaScript:** Automatic memory management, bounds checking, no manual pointers

### What This Means for You

Coming from TypeScript, you need to actively think about:
- Memory ownership (who frees this?)
- Buffer boundaries (is this index valid?)
- Lifetime (does this pointer/reference still point to valid data?)
- Type sizes (will this value fit?)

**Use modern C++ features to make the safe path easy:**
- `std::vector` instead of arrays
- `std::string` instead of char arrays
- `std::unique_ptr` instead of raw `new`/`delete`
- `.at()` during development, `[]` only after profiling

---

## Key Takeaways

1. **Signed integer overflow = undefined behavior** (unsigned is well-defined)
2. **Buffer overflow = memory corruption** - the real security danger
3. **Don't return references/views to local variables** - they'll be destroyed
4. **C++ trusts you completely** - which means it won't stop you from doing dangerous things
5. **Use modern C++ to stay safe** - the standard library is your friend

---

## Coming Up

- Use-after-free bugs
- Double-free bugs
- Memory leaks
- Iterator invalidation
- Undefined behavior catalog
