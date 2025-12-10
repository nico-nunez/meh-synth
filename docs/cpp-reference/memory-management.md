# Memory Management

Understanding stack vs heap, ownership, move semantics, and RAII.

## Table of Contents
- [Stack vs Heap](#stack-vs-heap)
- [Move Semantics and Ownership](#move-semantics-and-ownership)
- [RAII Pattern](#raii-pattern)

---

## Stack vs Heap

### Stack - Fast, Limited, Automatic
- Size known at **compile time**
- Automatically cleaned up when scope ends
- Limited size (usually ~1-8 MB)
- Very fast allocation/deallocation

```cpp
int x = 42;                    // Stack
int arr[100];                  // Stack - size known at compile time
std::string s = "hello";       // Stack (object itself, may use heap internally)
```

### Heap - Flexible, Slower, Manual Management
- Size can be **dynamic/unknown at compile time**
- Must be manually managed (or use smart pointers)
- Much larger available space
- Slower allocation/deallocation

```cpp
int* ptr = new int(42);        // Heap - must delete later
delete ptr;

std::vector<int> vec(n);       // Heap - size 'n' not known at compile time
                               // Automatically cleaned up when vec destroyed

std::string s = "very long..."; // String data on heap if > ~15-23 chars
```

### When Each is Used
```cpp
// Stack - size known at compile time
int fixed[100];
char buffer[256];

// Heap - size determined at runtime
std::vector<int> dynamic(user_input_size);
std::string filename = getUserInput();

// Stack object managing heap memory (best practice)
std::vector<int> v;     // 'v' itself on stack
v.push_back(1);         // Data stored on heap
```

**Key insight:** Modern C++ containers (`std::vector`, `std::string`) give you heap flexibility with stack-like automatic cleanup (RAII).

---

## Move Semantics and Ownership

### Returning by Value is OK!
```cpp
std::ofstream createFile() {
    std::ofstream file("output.wav", std::ios::binary);
    return file;  // Uses move semantics automatically - efficient!
}
```

Modern C++ (C++11+) automatically moves when returning local objects. **No copy made.**

### What Gets Moved?

When you return an object:
1. Compiler tries Return Value Optimization (RVO) - constructs directly at destination
2. If RVO doesn't apply, uses move constructor
3. Move transfers ownership of resources (file handles, heap memory, etc.)
4. Original object left in valid but empty state

### Ownership Matters - Views Don't Own Data

```cpp
// GOOD - string owns its data
std::string getData() {
    std::string data = "hello";
    return data;  // Moved to caller - safe!
}

// BAD - string_view doesn't own data
std::string_view getBadData() {
    std::string data = "hello";
    return std::string_view(data);  // DANGER! Pointing to destroyed data
}  // data destroyed here, but string_view still points to it

// GOOD usage of string_view - just viewing existing data
void printData(std::string_view sv) {
    std::cout << sv;  // OK - just viewing, not storing
}
```

### Types That Own vs View

**Own their data (safe to return):**
- `std::string`
- `std::vector<T>`
- `std::unique_ptr<T>`
- `std::shared_ptr<T>`
- Most standard containers

**View/reference data (dangerous to return):**
- `std::string_view` - view into string data
- Raw pointers (`T*`) - might point to anything
- References (`T&`) - aliases another object
- `std::span<T>` - view into array/vector

**Rule:** Return types that **own** their data. Don't return views to local variables - the data will be destroyed.

---

## RAII Pattern

**Resource Acquisition Is Initialization**

### The Concept

Resources (memory, files, locks) are tied to object lifetime:
- Acquire resource in constructor
- Release resource in destructor
- Automatic cleanup when object goes out of scope

### Examples

```cpp
// File handling - RAII
{
    std::ofstream file("output.txt");
    file << "data";
}  // File automatically closed here

// Manual way (error-prone):
FILE* f = fopen("output.txt", "w");
fprintf(f, "data");
fclose(f);  // Easy to forget!

// Memory - RAII
{
    std::vector<int> v = {1, 2, 3};
}  // Memory automatically freed

// Manual way (error-prone):
int* arr = new int[3];
arr[0] = 1;
delete[] arr;  // Easy to forget!

// Smart pointers - RAII
{
    std::unique_ptr<int> ptr = std::make_unique<int>(42);
}  // Automatically deleted

// Manual way (error-prone):
int* ptr = new int(42);
delete ptr;  // Easy to forget!
```

### Why It Matters

RAII prevents:
- Memory leaks
- Resource leaks (files, sockets, locks)
- Forgetting cleanup in error paths

**Modern C++ Rule:** Prefer RAII types over manual `new`/`delete`.

---

## Coming Up

- Smart pointers (`unique_ptr`, `shared_ptr`, `weak_ptr`)
- Reference counting
- Move-only types
- Custom deleters
