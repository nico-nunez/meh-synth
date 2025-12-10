# Headers and Includes

Everything about how C++ headers work and what to include where.

## Table of Contents
- [The Basics](#the-basics)
- [Include What You Use](#include-what-you-use)
- [Header Guards](#header-guards)

---

## The Basics

### Header Names vs Identifiers

```cpp
#include <iostream>   // Header FILE name
std::cout            // Thing DECLARED in that file (not std::iostream::cout!)
```

**Key insight:** Header file names and the identifiers inside are **completely independent**.

Examples:
- `#include <iostream>` → contains `std::cout`, `std::cin`, `std::istream`
- `#include <string>` → contains `std::string` (coincidentally same name)
- `#include <algorithm>` → contains `std::sort`, `std::find` (not std::algorithm!)
- `#include <fstream>` → contains `std::ofstream`, `std::ifstream` (not std::fstream!)

The similarity between `<string>` and `std::string` is just convenient naming, not a rule.

### Standard Library vs Your Headers

```cpp
#include <iostream>   // Standard library - angle brackets
#include "myutils.h"  // Your code - quotes
```

---

## Include What You Use

### Each File Should Be Self-Sufficient

**Always include what you directly use:**
```cpp
// myutils.h
#include <fstream>  // Needed for declarations
#include <string>

// myutils.cpp
#include "myutils.h"
#include <fstream>  // Include AGAIN - each file should be self-sufficient
#include <string>   // Don't rely on transitive includes
```

**Why?** If you later remove `#include <fstream>` from the header, the cpp file won't suddenly break.

### The Rule

Each `.cpp` and `.h` file should include all headers it **directly uses**, even if another included header already includes them.

**Exception you'll rarely use:** If your cpp file only uses types through function signatures declared in your header, and doesn't call any methods on them. But it's safer to just include what you use.

---

## Header Guards

### Prevent Multiple Inclusion

```cpp
// myutils.h
#ifndef MY_UTILS_H
#define MY_UTILS_H

// Your declarations here
void createWavFile();

#endif
```

**What it does:** Prevents the header from being included multiple times in the same translation unit (which would cause "redefinition" errors).

### Modern Alternative: #pragma once

```cpp
// myutils.h
#pragma once

// Your declarations here
void createWavFile();
```

**Pros:** Simpler, less typing
**Cons:** Not officially in the C++ standard (but supported by all major compilers)

**Recommendation:** Use `#ifndef` guards for maximum compatibility, or `#pragma once` if you're only targeting modern compilers.

---

## Common Questions

### Q: Why does ofstream accept const std::string& but not std::string_view (C++17)?

`std::ofstream` needs a null-terminated C-string to pass to the OS. `std::string` provides this via `.c_str()`, but `std::string_view` doesn't guarantee null-termination.

**Solution:** Convert explicitly: `std::ofstream(std::string(my_string_view))`

**Note:** C++23 added `std::string_view` support to `std::ofstream`.

---

## Coming Up

- Forward declarations (when you can avoid including headers)
- Circular dependencies and how to break them
- Include order and why it matters
