# Polyphonic Synthesis: How Multiple Notes Become One Waveform

## The Core Concept

**Polyphony = summing multiple independent oscillators into a single audio stream.**

```cpp
float output = voice1.getSample() + voice2.getSample() + voice3.getSample();
```

This creates a **single waveform** that encodes all frequencies. This isn't a limitation—it's how sound actually works.

---

## Why One Waveform Can Contain Multiple Notes

### The Physics
- Your speaker cone can only be in **one position** at any given instant
- In nature, sound waves physically add together in the air before reaching your ear
- Your eardrum receives a **single pressure value** at each moment
- Digital audio mimics this: one sample value per time step

### The Math: Information Is Preserved Over Time

**Single sample = ambiguous:**
```
Sample value: 0.6
Could be: sine(261Hz), sine(329Hz), or infinite other combinations
```

**Multiple samples = unique pattern:**
```
261 Hz sine over 10 samples:
[0.0, 0.5, 0.9, 1.0, 0.9, 0.5, 0.0, -0.5, -0.9, -1.0]

329 Hz sine over 10 samples:
[0.0, 0.6, 1.0, 0.8, 0.2, -0.5, -1.0, -0.8, -0.2, 0.5]

Sum (261 + 329 Hz):
[0.0, 1.1, 1.9, 1.8, 1.1, 0.0, -1.0, -1.3, -1.1, -0.5]
      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
      This pattern can ONLY come from 261Hz + 329Hz
```

**Key insight:** You need at least **one full cycle** to characterize a frequency. The temporal pattern acts as a unique "fingerprint."

At 44.1kHz:
- 261 Hz = ~169 samples/cycle
- 329 Hz = ~134 samples/cycle
- The interference pattern over these samples uniquely encodes both frequencies

---

## How Your Ear Decodes Multiple Frequencies

Your cochlea (inner ear) performs **frequency decomposition**:
- Different hair cells resonate at different frequencies
- 261 Hz component → vibrates specific cells
- 329 Hz component → vibrates different cells
- 392 Hz component → vibrates yet other cells
- Brain interprets the activation pattern as "three simultaneous notes"

**Analogy:** Like seeing purple light and perceiving "red + blue components" even though you only receive one color—your sensors respond to different wavelengths in a specific ratio.

---

## The Fourier Transform: Detecting Frequencies

The Fourier Transform proves that **any complex waveform can be uniquely decomposed into sine waves**.

### The Core Idea: Correlation

**How do you detect if a specific frequency is present in a signal?**

Generate a "test tone" at that frequency and compare it to your signal:
1. Multiply the signal by the test tone, sample by sample
2. Add up all the products
3. If the frequency is present → large sum
4. If the frequency is absent → sum cancels to ~0

This is **correlation** - testing if two waves oscillate at the same frequency.

### What "In Sync" Means

**In sync = both waves reach peaks and valleys at the same time**

#### Example 1: Signal CONTAINS 440 Hz (in sync)
```
Sample #:        0     1     2     3     4     5     6     7
─────────────────────────────────────────────────────────────
Your signal:   0.0   0.7   1.0   0.7   0.0  -0.7  -1.0  -0.7  (single 440 Hz tone)
Test (440 Hz): 0.0   0.7   1.0   0.7   0.0  -0.7  -1.0  -0.7

Multiply:      0.0   0.49  1.0   0.49  0.0   0.49  1.0   0.49
               ^^^   ^^^^  ^^^   ^^^^  ^^^   ^^^^  ^^^   ^^^^
               All positive! Peaks align.

Sum: 0.0 + 0.49 + 1.0 + 0.49 + 0.0 + 0.49 + 1.0 + 0.49 = 3.96 ← LARGE!
```

Notice:
- When both positive: `0.7 × 0.7 = 0.49` ✓
- When both negative: `-0.7 × -0.7 = 0.49` ✓
- They rise and fall together = **in sync**

#### Example 2: Signal DOES NOT contain 440 Hz (out of sync)
```
Sample #:        0     1     2     3     4     5     6     7
─────────────────────────────────────────────────────────────
Your signal:   0.0   1.0   0.0  -1.0   0.0   1.0   0.0  -1.0  (880 Hz, not 440)
Test (440 Hz): 0.0   0.7   1.0   0.7   0.0  -0.7  -1.0  -0.7

Multiply:      0.0   0.7   0.0  -0.7   0.0  -0.7   0.0   0.7
               ^^^   ^^^   ^^^   ^^^   ^^^   ^^^   ^^^   ^^^
               Mix of positive and negative!

Sum: 0.0 + 0.7 + 0.0 + (-0.7) + 0.0 + (-0.7) + 0.0 + 0.7 = 0.0 ← Cancels!
```

Products alternate positive/negative because waves are **out of sync**.

### Single Frequency vs. Complex Pattern

**Single 440 Hz tone:**
```
Sample #:  0     1     2     3     4     5     6     7     8     9
────────────────────────────────────────────────────────────────────
Signal:   0.0   0.7   1.0   0.7   0.0  -0.7  -1.0  -0.7   0.0   0.7
          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          Smooth, regular pattern - all peaks same height
```

**Mixed signal (440 Hz + 880 Hz):**
```
440 Hz:   0.0   0.7   1.0   0.7   0.0  -0.7  -1.0  -0.7   0.0   0.7
880 Hz:   0.0   1.0   0.0  -1.0   0.0   1.0   0.0  -1.0   0.0   1.0
          +     +     +     +     +     +     +     +     +     +
────────────────────────────────────────────────────────────────────
MIXED:    0.0   1.7   1.0  -0.3   0.0   0.3  -1.0  -1.7   0.0   1.7
          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          Irregular! Peaks vary: 1.7, 1.0, 0.3... This encodes both frequencies.
```

**Real C major chord (261 + 329 + 392 Hz) at 44.1kHz:**
```
Sample #:  0     1     2     3     4     5     6     7     8
──────────────────────────────────────────────────────────────
261 Hz:   0.00  0.04  0.07  0.11  0.15  0.18  0.22  0.26  0.29
329 Hz:   0.00  0.05  0.09  0.14  0.19  0.23  0.28  0.32  0.37
392 Hz:   0.00  0.06  0.11  0.17  0.22  0.28  0.33  0.39  0.44
          +     +     +     +     +     +     +     +     +
──────────────────────────────────────────────────────────────
MIXED:    0.00  0.15  0.27  0.42  0.56  0.69  0.83  0.97  1.10
          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
          Complex pattern encoding all three frequencies
```

This mixed signal is what `voice1.getSample() + voice2.getSample() + voice3.getSample()` produces.

When you test this against 261 Hz, the correlation will be **large** (frequency is present).
When you test against 440 Hz, the correlation will be **~0** (frequency is absent).

---

### Discrete Fourier Transform (DFT) Formula

For a signal with N samples `x[0], x[1], ..., x[N-1]`:

```
X[k] = Σ(n=0 to N-1) x[n] · e^(-i·2π·k·n/N)
       ^^^^^^^^^^^   ^^^^   ^^^^^^^^^^^^^^^
       "add up"    "signal"  "test tone at frequency k"
```

Where:
- `X[k]` = amplitude/phase of frequency bin k
- `x[n]` = time-domain sample at index n
- `k` = frequency bin (0 to N-1)
- `i` = imaginary unit (√-1, where i² = -1)
- `N` = total number of samples

**Euler's formula:** `e^(iθ) = cos(θ) + i·sin(θ)`

So the DFT becomes:
```
X[k] = Σ(n=0 to N-1) x[n] · [cos(2π·k·n/N) - i·sin(2π·k·n/N)]
```

**What this does:**
- For each frequency k, multiply each sample by a test tone at that frequency
- Sum all the products
- If frequency k is present → waves align → large sum
- If frequency k is absent → waves misaligned → products cancel to ~0

### Inverse Fourier Transform

You can go backwards—reconstruct the original signal from frequencies:

```
x[n] = (1/N) · Σ(k=0 to N-1) X[k] · e^(i·2π·k·n/N)
```

This proves information is **preserved**: frequencies → time → frequencies (lossless).

### Practical Example

If your signal is `sin(2π·261·t) + sin(2π·329·t)`:
- Run DFT on 1 second of samples (44,100 samples)
- You'll see two spikes in the frequency domain:
  - One at bin corresponding to 261 Hz
  - One at bin corresponding to 329 Hz
- All other bins will be near zero

---

## Key Takeaways

✓ Summing oscillators creates a **single waveform** with a unique temporal pattern
✓ Pattern over time (especially 1+ full cycles) uniquely identifies component frequencies
✓ This mimics physical reality—sound waves add in air before reaching your ear
✓ Fourier Transform mathematically guarantees information preservation
✓ Your ear/brain naturally performs this frequency decomposition in hardware

---

## Further Topics to Explore

- **Fast Fourier Transform (FFT):** Efficient algorithm (O(N log N) vs O(N²))
- **Nyquist theorem:** Why sample rate must be 2x highest frequency
- **Aliasing:** What happens when frequencies exceed Nyquist limit
- **Phase relationships:** How phase affects the summed waveform
- **Windowing:** Techniques for analyzing finite-length signals
