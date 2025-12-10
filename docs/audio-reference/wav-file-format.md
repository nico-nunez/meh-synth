# WAV File Format

Understanding the structure (header) of WAV files and why you can't just write raw audio data.

## The Problem

You might think saving audio would be simple:
```cpp
// Why not just this?
for (short sample : samples) {
    file.write(sample);
}
```

**Problem:** Audio players need metadata to interpret the raw numbers:
- How fast to play it? (sample rate)
- How many channels? (mono/stereo)
- How are samples encoded? (16-bit PCM, 32-bit float, etc.)
- Where does the data start/end?

**Solution:** WAV files have a **header** (metadata) before the audio data.

---

## WAV File Structure

A WAV file is organized into **chunks** - blocks of data with an ID and size.

```
┌─────────────────────────────────────┐
│         RIFF HEADER                 │  ← File type & size
├─────────────────────────────────────┤
│         FORMAT CHUNK (fmt)          │  ← Audio format details
├─────────────────────────────────────┤
│         DATA CHUNK (data)           │  ← Actual audio samples
└─────────────────────────────────────┘
```

**This is why you need a function like `writeWavHeader()` or `writeWavMetadata()`** - it writes the header chunks that describe the audio data.

---

## Chunk Breakdown

### 1. RIFF Header (12 bytes)

Identifies the file as a WAV file and tells how big it is.

```cpp
writeString(file, "RIFF", 4);           // Magic number: "RIFF"
writeInt32(file, fileSize);             // File size - 8 bytes
writeString(file, "WAVE", 4);           // Format: "WAVE"
```

**Why "RIFF"?** WAV is part of the Resource Interchange File Format family (also used for AVI video).

**File size calculation:**
```
fileSize = 36 + (numSamples * bytesPerSample)
         = header size + data size
```

### 2. Format Chunk (24 bytes)

Describes HOW the audio is encoded.

```cpp
writeString(file, "fmt ", 4);           // Chunk ID (note the space!)
writeInt32(file, 16);                   // Chunk size (16 for PCM)

writeInt16(file, 1);                    // Audio format (1 = PCM)
writeInt16(file, 1);                    // Channels (1 = mono, 2 = stereo)
writeInt32(file, sampleRate);           // Sample rate (e.g., 44100)
writeInt32(file, byteRate);             // Bytes per second
writeInt16(file, blockAlign);           // Bytes per sample across all channels
writeInt16(file, 16);                   // Bits per sample
```

**Key fields explained:**

**Audio format (1 = PCM):**
- PCM = Pulse Code Modulation (uncompressed)
- Value 1 means raw integer samples
- Other values = compressed formats (MP3, etc.)

**Sample rate:**
- Samples per second (44,100 Hz for CD quality)
- Determines highest representable frequency (Nyquist)

**Byte rate:**
```
byteRate = sampleRate × channels × bytesPerSample
         = 44,100 × 1 × 2
         = 88,200 bytes/second
```

**Block align:**
```
blockAlign = channels × bytesPerSample
           = 1 × 2
           = 2 bytes per sample frame
```
(A "frame" is one sample for all channels)

**Bits per sample:**
- 16 bits = values from -32,768 to 32,767
- Determines dynamic range/quality

### 3. Data Chunk (8 bytes + samples)

Contains the actual audio samples.

```cpp
writeString(file, "data", 4);           // Chunk ID
writeInt32(file, dataSize);             // Size of audio data (32-bit field!)

// Then write all samples
for (short sample : samples) {
    writeInt16(file, sample);
}
```

**Data size calculation:**
```
dataSize = numSamples × bytesPerSample
         = numSamples × 2
```

**Important: 32-bit limit**
- The `dataSize` field is 32-bit (max ~2.1 billion)
- This limits WAV files to ~4GB or ~6.8 hours at 44.1kHz stereo 16-bit
- Use `int32_t` for sample counts when working with WAV format to match this constraint
- If you need longer files, consider using WAV64 or other formats

---

## Complete Example

```cpp
// For a 3-second, 44.1kHz mono file:
numSamples = 44,100 × 3 = 132,300 samples

File structure:
[RIFF] "RIFF"                    4 bytes
[RIFF] fileSize = 264,636        4 bytes    (36 + 132,300×2)
[RIFF] "WAVE"                    4 bytes

[FMT ] "fmt "                    4 bytes
[FMT ] 16                        4 bytes    (chunk size)
[FMT ] 1                         2 bytes    (PCM)
[FMT ] 1                         2 bytes    (mono)
[FMT ] 44,100                    4 bytes    (sample rate)
[FMT ] 88,200                    4 bytes    (byte rate)
[FMT ] 2                         2 bytes    (block align)
[FMT ] 16                        2 bytes    (bits per sample)

[DATA] "data"                    4 bytes
[DATA] 264,600                   4 bytes    (132,300 × 2)
[DATA] <132,300 samples>         264,600 bytes

Total: 264,644 bytes
```

---

## Why Little-Endian?

WAV format stores multi-byte integers in **little-endian** (least significant byte first):

```
Value: 44,100 (0xAC44 in hex)

Big-endian:    AC 44
Little-endian: 44 AC  ← WAV uses this
```

**This is handled by:**
```cpp
file.write(reinterpret_cast<const char*>(&value), 4);
```
On x86/x64 (Intel/AMD), integers are already little-endian in memory, so this "just works."

---

## Why This Structure?

### Extensibility
New chunks can be added without breaking old players:
```
[RIFF][WAVE][fmt ][fact][data][metadata]...
              ↑               ↑
          required        optional
```

Players that don't understand `[fact]` or `[metadata]` can skip them.

### Compatibility
The chunk structure is self-describing:
- Each chunk has a size field
- Players can skip unknown chunks
- Allows format evolution

### Simplicity
Fixed layout makes parsing straightforward:
1. Read chunk ID (4 bytes)
2. Read chunk size (4 bytes)
3. Read/skip chunk data
4. Repeat until end

---

## Common Terms

**Header vs Structure:**
Both are used, but technically:
- **Header** = the metadata at the beginning of the file
- **Structure** = the organization/layout of the entire file
- **Format** = how the data is encoded

In practice, people use them interchangeably when talking about WAV files.

**In your code:**
A function like `writeWavHeader()` or `writeWavMetadata()` writes the **header chunks** (RIFF + fmt), then `main.cpp` writes the **data chunk**.

---

## Key Takeaways

1. **WAV files = header + samples** - you can't just dump raw audio
2. **Header (metadata) describes the audio** - sample rate, channels, format
3. **Chunks are self-describing** - ID + size + data
4. **Little-endian byte order** - least significant byte first
5. **Header/metadata must be written first** - without it, players can't interpret the file
6. **32-bit format limit** - WAV files limited to ~4GB due to 32-bit size fields; use `int32_t` for sample counts

---

## Coming Up

- Other audio formats (AIFF, FLAC, MP3)
- Bit depth and dynamic range
- Why 16-bit PCM is the standard
- Floating-point audio formats
