# Sine Wave Generator - Learning Guide

## What This Program Does

This program generates a pure sine wave tone and saves it as a WAV file. It's written from scratch without audio libraries so you can see exactly how digital audio works.

## Compilation

```bash
# On Mac/Linux:
g++ -o sine_wave sine_wave_generator.cpp -std=c++11

# On Windows (with MinGW):
g++ -o sine_wave.exe sine_wave_generator.cpp -std=c++11

# Run it:
./sine_wave
```

This creates `output.wav` in the same directory.

## Key Concepts You're Learning

### 1. Digital Audio Basics
- Audio is just an array of numbers representing air pressure over time
- Sample rate (44,100 Hz) = 44,100 numbers per second
- Each number represents the speaker position at that instant

### 2. The Sine Wave Formula
```cpp
value = sin(2 * π * frequency * time)
```
- `frequency`: How many cycles per second (440 Hz = A4 note)
- `time`: Current position in seconds (sample_number / sample_rate)
- Result: A value between -1.0 and 1.0

### 3. Data Conversion
- Math gives us floating point values (-1.0 to 1.0)
- WAV files use 16-bit integers (-32,768 to 32,767)
- We multiply by 32,767 to convert

### 4. WAV File Format
A WAV file is just:
- A header describing the audio (sample rate, bit depth, channels)
- Raw audio data (the array of samples)
- All written as binary data

## Experiments to Try

### Easy:
1. **Change the frequency**: Try 220 (A3), 880 (A5), or 100 (low rumble)
2. **Change duration**: Make it longer or shorter
3. **Change amplitude**: Try 0.2 (quieter) or 0.8 (louder), but don't go above 1.0!

### Medium:
4. **Add two frequencies together**: Generate two sine waves and add their values
   ```cpp
   double value = 0.25 * sin(2.0 * M_PI * 440.0 * time) +
                  0.25 * sin(2.0 * M_PI * 880.0 * time);
   ```
   This creates a chord!

5. **Make it fade in/out**: Multiply amplitude by a value that changes over time
   ```cpp
   double fadeIn = static_cast<double>(i) / (SAMPLE_RATE * 0.5);  // 0.5 sec fade
   fadeIn = (fadeIn > 1.0) ? 1.0 : fadeIn;  // Cap at 1.0
   double value = AMPLITUDE * fadeIn * sin(...);
   ```

### Advanced:
6. **Create stereo**: Change channels to 2, generate different samples for left/right
7. **Add harmonics**: Mix multiple frequencies (fundamental + overtones)
8. **Implement tremolo**: Modulate amplitude with a slow sine wave

## What You've Actually Learned

Even though this is "just" a sine wave generator, you now understand:
- How digital audio is represented (samples and sample rates)
- How frequencies relate to pitch
- How audio effects are just math operations on arrays
- The WAV file format structure
- Binary file I/O in C++

This is the foundation. Every audio effect, synthesizer, or plugin is ultimately doing the same thing: reading samples, doing math on them, and writing samples.

## Next Steps

Once you're comfortable modifying this:
1. Read an existing WAV file and process it (add distortion, change volume)
2. Implement a simple low-pass filter
3. Then move to JUCE where you'll do this in real-time instead of offline

The concepts are the same, but real-time audio adds constraints (you must process each buffer within milliseconds, no memory allocations allowed, etc.)
