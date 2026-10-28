---
name: libyuv/api/document-parameter-ranges-and-bit-alignment
description: When declaring public APIs taking depth, scale, bias, or uint16_t buffers, document LSB/MSB bit alignment, valid depth ranges, and channel-specific coefficients
rubric: readability
severity: should_fix
---

**Rule.** When declaring or documenting public functions in include/libyuv/ that
process high-bit-depth pixel planes stored in 16-bit integer containers
(uint16_t) or accept parameters with non-obvious ranges or conversion factors
(such as depth, bits, scale, or bias), document the expected data layout and
parameter constraints in header comments:
1. **Bit Alignment**: Explicitly state whether high-bit-depth samples (e.g., 10-bit or 12-bit in uint16_t) are stored in the low bits (LSB-aligned, like standard planar I010/I012) or the high/upper bits (MSB-aligned, like biplanar P010/P012). Avoid ambiguous phrases like 'within 12bit range' that omit bit positioning.
2. **Valid Ranges**: Document supported numeric ranges for depth parameters (e.g., [10, 16]).
3. **Channel Coefficients**: Provide accurate channel-specific scaling factors or offsets (e.g., distinguishing luma Y scaling from chroma U/V scaling).

**Why.** In 16-bit integer containers (uint16_t), 10-bit and 12-bit samples can
be stored either LSB-aligned (common in software planar formats) or MSB-aligned
(common in hardware and Microsoft biplanar formats). Stating only the bit-depth
range without specifying alignment leaves buffer layout ambiguous. Furthermore,
callers cannot infer valid parameter ranges, depth constraints, or
channel-specific scaling factors from parameter names alone, leading to
misconfigured conversions, inverted channel scaling, or out-of-range memory
accesses.

## Examples

Bad:

```cpp
// Variant 1: Ambiguous bit alignment for uint16_t high-bit-depth buffer
// Expect sample to be within 12bit range.
LIBYUV_API
int ScalePlane_12(const uint16_t* src,
                  int src_stride,
                  int src_width,
                  int src_height,
                  uint16_t* dst,
                  int dst_stride,
                  int dst_width,
                  int dst_height,
                  enum FilterMode filtering);

// Variant 2: Omitted depth range and bit alignment
LIBYUV_API
void MergeAR30Plane(const uint16_t* src_r,
                    int src_stride_r,
                    const uint16_t* src_g,
                    int src_stride_g,
                    const uint16_t* src_b,
                    int src_stride_b,
                    uint8_t* dst_ar30,
                    int dst_stride_ar30,
                    int width,
                    int height,
                    int depth);

// Variant 3: Inaccurate / swapped channel scaling factors
LIBYUV_API
void Convert8To8Plane(const uint8_t* src_y,
                      int src_stride_y,
                      uint8_t* dst_y,
                      int dst_stride_y,
                      int scale,  // 225 for Y, 220 for U,V (swapped)
                      int bias,   // 16
                      int width,
                      int height);
```

Good:

```cpp
// Variant 1: Explicit LSB alignment for 12-bit samples
// Sample is expected to be in the low 12 bits.
LIBYUV_API
int ScalePlane_12(const uint16_t* src,
                  int src_stride,
                  int src_width,
                  int src_height,
                  uint16_t* dst,
                  int dst_stride,
                  int dst_width,
                  int dst_height,
                  enum FilterMode filtering);

// Variant 2: Documented valid depth range and LSB alignment
// Merge separate 'depth' bit R, G and B planes stored in lsb
// into one interleaved AR30 plane. depth should be in range [10, 16].
LIBYUV_API
void MergeAR30Plane(const uint16_t* src_r,
                    int src_stride_r,
                    const uint16_t* src_g,
                    int src_stride_g,
                    const uint16_t* src_b,
                    int src_stride_b,
                    uint8_t* dst_ar30,
                    int dst_stride_ar30,
                    int width,
                    int height,
                    int depth);

// Variant 3: Accurate channel-specific scaling factors
LIBYUV_API
void Convert8To8Plane(const uint8_t* src_y,
                      int src_stride_y,
                      uint8_t* dst_y,
                      int dst_stride_y,
                      int scale,  // 220 for Y, 225 for U,V
                      int bias,   // 16
                      int width,
                      int height);
```
