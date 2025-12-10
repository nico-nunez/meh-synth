# Sampling and the Nyquist Limit

Understanding why sample rate determines the highest frequency we can represent.

## The Core Insight

**Nyquist-Shannon Sampling Theorem:** To accurately represent a frequency, you need at least **2 samples per cycle**.

**Why?** You need to capture both the peak and trough of the wave before the cycle repeats.

```
Sample Rate = 44,100 Hz
Maximum Frequency = 44,100 / 2 = 22,050 Hz (Nyquist frequency)
```

## Visualizing It

### One Sample Per Cycle - NOT ENOUGH
```
Wave:     /\    /\    /\
Samples:  *     *     *

Result: Just captures peaks, looks like DC offset - can't reconstruct the wave!
```

### Two Samples Per Cycle - MINIMUM
```
Wave:     /\    /\    /\
Samples:  * *   * *   * *

Result: Captures peak and trough - can reconstruct the original frequency
```

### More Samples Per Cycle - BETTER
```
Wave:     /\      /\      /\
Samples:  * * * * * * * * * * * *

Result: More detail, better reconstruction, less error
```

## The Aha Moment

**Q:** Why is 44.1kHz the CD standard?
**A:** Human hearing range is ~20Hz to 20kHz. To capture 20kHz, you need at least 40kHz sample rate. 44.1kHz provides a safety margin.

**What happens at exactly the Nyquist frequency (22.05kHz)?**
- You get exactly 2 samples per cycle
- Reconstruction is possible but not ideal
- In practice, you want some headroom (that's why 44.1 > 40)

## What Happens Above Nyquist?

### Aliasing - Frequency Folding

If you try to represent a frequency **above** the Nyquist limit, it "folds back" and appears as a **lower, incorrect frequency**.

```
Sample Rate: 8,000 Hz
Nyquist: 4,000 Hz

Try to represent 6,000 Hz:
- Not enough samples per cycle
- Appears as 2,000 Hz instead!
  (8,000 - 6,000 = 2,000)
```

### Example: Wagon Wheel Effect

Ever notice car wheels appearing to spin backwards in movies? Same principle!
- Wheel spins at frequency F
- Camera samples at rate S
- If F > S/2, it appears to spin at (S - F) = backwards

## Practical Implications

### Sample Rates in Common Use

```
8,000 Hz   - Telephone quality (Nyquist: 4 kHz)
22,050 Hz  - Low quality audio (Nyquist: 11.025 kHz)
44,100 Hz  - CD quality (Nyquist: 22.05 kHz) - covers human hearing
48,000 Hz  - Professional audio/video (Nyquist: 24 kHz)
96,000 Hz  - High-res audio (Nyquist: 48 kHz) - headroom for processing
```

### Why Higher Sample Rates?

1. **Anti-aliasing filters** - Need room to filter out frequencies near Nyquist
2. **Processing headroom** - Effects can create harmonics above input frequencies
3. **Digital artifacts** - Higher rate = gentler filters = less phase distortion
4. **Not for playback** - Most downsampled to 44.1/48kHz for final output

## The Math

**Nyquist Frequency Formula:**
```
f_nyquist = sample_rate / 2
```

**Minimum samples per cycle:**
```
samples_per_cycle = sample_rate / frequency
```

**Example: 440 Hz at 44.1 kHz**
```
samples_per_cycle = 44,100 / 440 ≈ 100 samples
```
Plenty of samples to accurately represent an A4 note!

**Example: 22,050 Hz at 44.1 kHz**
```
samples_per_cycle = 44,100 / 22,050 = 2 samples
```
Bare minimum - on the edge of representable.

## Key Takeaways

1. **Need 2+ samples per cycle** to represent a frequency
2. **Nyquist frequency = sample_rate / 2** - theoretical maximum
3. **Above Nyquist = aliasing** - wrong frequencies appear
4. **44.1kHz covers human hearing** (20 Hz - 20 kHz with headroom)
5. **More samples = better accuracy** but diminishing returns

---

## Coming Up

- Anti-aliasing filters and how they work
- Bit depth vs sample rate (amplitude resolution vs time resolution)
- Oversampling in audio processing
- Why the WAV file format stores samples as integers
