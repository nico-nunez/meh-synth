# Makefiles for C++ Projects

## What is a Makefile?

A Makefile is a build automation tool that:
- Compiles your source files
- Links them into an executable
- Only recompiles changed files (incremental builds)
- Defines build targets (debug, release, clean, etc.)

**Why use it?** Instead of typing `clang++ main.cpp utils.cpp -o main` every time, just type `make`.

## Basic Makefile Structure

```makefile
# Variables
CXX = clang++
CXXFLAGS = -std=c++17 -Wall
TARGET = main
SOURCES = main.cpp utils.cpp

# Target: dependencies
#   command (must be indented with TAB, not spaces!)
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
```

**Run it:**
```bash
make          # Builds $(TARGET)
make clean    # Removes built files
```

## Key Syntax

### Variables

```makefile
CXX = clang++           # Define variable
CXXFLAGS = -std=c++17   # Another variable

# Use with $(VARIABLE_NAME)
$(CXX) $(CXXFLAGS) -o main main.cpp
```

### Targets and Rules

```makefile
target: dependencies
	command

# Example:
main: main.cpp utils.cpp
	clang++ -o main main.cpp utils.cpp
```

**Important:** Commands MUST be indented with a **TAB character**, not spaces!

### Special Variables

```makefile
$@  # The target name
$^  # All dependencies
$<  # First dependency

# Example:
main: main.cpp utils.cpp
	$(CXX) -o $@ $^
	# Expands to: clang++ -o main main.cpp utils.cpp
```

### .PHONY Targets

Targets that don't create files:

```makefile
.PHONY: clean debug release

clean:
	rm -f $(TARGET)

debug: CXXFLAGS += -g
debug: $(TARGET)
```

Without `.PHONY`, if a file named "clean" exists, `make clean` won't run.

## Flat Directory Structure

Simple Makefile for all files in one directory:

```makefile
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = main
SOURCES = main.cpp Voice.cpp WavWriter.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean
```

## Subdirectory Structure - Manual Listing

When you have `src/synth/`, `src/utils/`, etc:

```makefile
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = main

# List all source files with paths
SOURCES = src/main.cpp \
          src/synth/Voice.cpp \
          src/synth/Synthesizer.cpp \
          src/utils/WavWriter.cpp

# Add src/ to include search path
INCLUDES = -Isrc

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean
```

**Downside:** You have to manually add every new `.cpp` file. Annoying!

## Subdirectory Structure - Automatic (Wildcard)

Automatically find all `.cpp` files:

```makefile
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = main

# Automatically find all .cpp files in src/ and subdirectories
SOURCES = $(wildcard src/*.cpp) \
          $(wildcard src/*/*.cpp) \
          $(wildcard src/*/*/*.cpp)

INCLUDES = -Isrc

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean
```

**How it works:**
- `$(wildcard src/*.cpp)` finds all `.cpp` in `src/`
- `$(wildcard src/*/*.cpp)` finds all `.cpp` one level deep (`src/synth/`, `src/utils/`)
- `$(wildcard src/*/*/*.cpp)` finds all `.cpp` two levels deep

**Pros:** Automatic! Add a new `.cpp` file, it's included.
**Cons:** Must specify depth levels, includes ALL `.cpp` files (even test files you might not want).

## Better Approach - Using Shell Commands

Use `find` to recursively find all `.cpp` files:

```makefile
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = main

# Recursively find all .cpp files in src/
SOURCES = $(shell find src -name '*.cpp')

INCLUDES = -Isrc

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean
```

**Pros:** Truly recursive, finds all `.cpp` files at any depth.
**Cons:** Still includes everything (might grab test files).

## Advanced: Separate Compilation (Object Files)

Instead of recompiling everything every time, compile each `.cpp` to `.o` (object file), then link:

```makefile
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = main

# Find all source files
SOURCES = $(shell find src -name '*.cpp')

# Convert src/foo/Bar.cpp -> build/src/foo/Bar.o
OBJECTS = $(SOURCES:%.cpp=build/%.o)

INCLUDES = -Isrc

# Link object files into executable
$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)

# Compile .cpp to .o
build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(TARGET)
	rm -rf build

.PHONY: clean
```

**How it works:**
1. Each `.cpp` compiles to a `.o` file in `build/`
2. Only changed files recompile (incremental builds!)
3. All `.o` files link into final executable

**Pros:** Fast incremental builds - only recompile what changed.
**Cons:** More complex Makefile.

## Debug vs Release Builds

Different flags for debug (with symbols) vs release (optimized):

```makefile
CXX = clang++
TARGET = main
SOURCES = $(shell find src -name '*.cpp')
INCLUDES = -Isrc

DEBUG_FLAGS = -std=c++17 -Wall -Wextra -g -O0
RELEASE_FLAGS = -std=c++17 -Wall -Wextra -O2 -DNDEBUG

# Default: debug build
debug: CXXFLAGS = $(DEBUG_FLAGS)
debug: $(TARGET)

release: CXXFLAGS = $(RELEASE_FLAGS)
release: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: debug release clean
```

**Usage:**
```bash
make debug     # Build with debug flags
make release   # Build with optimization
make           # Defaults to debug
```

## Complete Example - Recommended for Your Project

```makefile
CXX = clang++
TARGET = main

# Compiler flags
DEBUG_FLAGS = -std=c++17 -Wall -Weffc++ -Wextra -Werror -pedantic-errors \
              -Wconversion -Wsign-conversion -g -O0
RELEASE_FLAGS = -std=c++17 -Wall -Weffc++ -Wextra -Werror -pedantic-errors \
                -Wconversion -Wsign-conversion -O2 -DNDEBUG

# Find all source files recursively
SOURCES = $(shell find src -name '*.cpp')

# Convert to object files (src/foo.cpp -> build/src/foo.o)
OBJECTS = $(SOURCES:%.cpp=build/%.o)

# Include paths
INCLUDES = -Isrc

# Default: debug build
.DEFAULT_GOAL := debug

# Debug build
debug: CXXFLAGS = $(DEBUG_FLAGS)
debug: $(TARGET)

# Release build
release: CXXFLAGS = $(RELEASE_FLAGS)
release: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)

# Compile
build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -f $(TARGET)
	rm -rf build

# Rebuild
rebuild: clean $(TARGET)

.PHONY: debug release clean rebuild
```

**Features:**
- ✅ Automatic source file discovery
- ✅ Incremental builds (only recompile changed files)
- ✅ Debug and release targets
- ✅ Clean separation (object files in `build/`)
- ✅ Include path setup for subdirectories

**Usage:**
```bash
make              # Build debug (default)
make debug        # Build debug
make release      # Build release
make clean        # Remove built files
make rebuild      # Clean + build
```

## Common Makefile Variables

```makefile
CXX         # C++ compiler (clang++, g++)
CXXFLAGS    # Compiler flags (-std=c++17, -Wall, etc.)
LDFLAGS     # Linker flags (for libraries: -lm, -lpthread)
INCLUDES    # Include paths (-Isrc, -I/usr/local/include)
SOURCES     # Source files (.cpp files)
OBJECTS     # Object files (.o files)
TARGET      # Output executable name
```

## Dependency Tracking (Advanced)

Make can auto-generate header dependencies so changing a `.h` file rebuilds files that include it:

```makefile
# Generate .d dependency files
DEPENDS = $(OBJECTS:.o=.d)

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Include dependency files
-include $(DEPENDS)
```

**How it works:**
- `-MMD -MP` generates `.d` files listing header dependencies
- `-include $(DEPENDS)` loads those dependencies
- If a header changes, Make knows to recompile files that include it

## Makefile vs CMake

**Makefile:**
- ✅ Simple, direct
- ✅ No extra tools needed
- ❌ Platform-specific (Linux/Mac mainly)
- ❌ Manual setup for complex projects

**CMake:**
- ✅ Cross-platform (generates Makefiles, Visual Studio projects, Xcode projects)
- ✅ Better for large/complex projects
- ✅ Standard in modern C++ (especially with libraries)
- ❌ More complex, extra tool to learn
- ❌ Overkill for small projects

**Recommendation:**
- Learning project, < 20 files: **Makefile**
- Moving to JUCE: **CMake** (JUCE uses it)
- Large project, cross-platform: **CMake**

## Quick Reference

```makefile
# Variables
VAR = value
VAR += more        # Append
VAR := value       # Immediate assignment

# Wildcards
$(wildcard src/*.cpp)           # Find files
$(shell find src -name '*.cpp') # Shell command

# Pattern rules
%.o: %.cpp                     # .cpp -> .o
	command

# Special variables
$@    # Target name
$<    # First dependency
$^    # All dependencies
$(dir path/to/file.cpp)  # Returns: path/to/

# Functions
$(wildcard pattern)           # Find files
$(shell command)              # Run shell command
$(subst from,to,text)         # Substitute
$(patsubst pattern,replacement,text)  # Pattern substitute

# Phony targets
.PHONY: clean debug release
```

## Common Gotchas

**1. Tabs vs Spaces**
```makefile
target:
    command    # ERROR! This is spaces

target:
	command    # CORRECT! This is a tab
```

Commands MUST be indented with TAB, not spaces. Configure your editor!

**2. Missing .PHONY**
```makefile
# If a file named "clean" exists, make clean won't work
clean:
	rm -f $(TARGET)

# Fix:
.PHONY: clean
clean:
	rm -f $(TARGET)
```

**3. Missing backslash in multi-line**
```makefile
SOURCES = src/main.cpp
          src/Voice.cpp    # ERROR! Missing \ on previous line

SOURCES = src/main.cpp \
          src/Voice.cpp    # CORRECT!
```

**4. Wrong variable syntax**
```makefile
echo $CXX      # Wrong - expands to shell variable
echo $(CXX)    # Correct - Makefile variable
```

## Example Project Structure

```
sine-wav-generator/
  src/
    main.cpp
    synth/
      Voice.cpp
      Synthesizer.cpp
    utils/
      WavWriter.cpp
  build/           # Generated by Makefile
    src/
      main.o
      synth/
        Voice.o
        Synthesizer.o
      utils/
        WavWriter.o
  main             # Final executable
  Makefile
```

**Makefile automatically:**
- Finds all `.cpp` files in `src/`
- Compiles to `.o` in `build/`
- Links into `main` executable
- Only recompiles changed files
