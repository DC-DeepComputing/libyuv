---
name: libyuv/api/no-mixed-depth-scaling-apis
description: When designing public scaling APIs, keep functions homogeneous in channel bit depth rather than combining geometric scaling with bit-depth or format conversion
rubric: design
severity: should_fix
---

**Rule.** Do not expose public scaling APIs that combine geometric scaling with
bit-depth or format conversion (such as scaling and converting from 16-bit to
8-bit in a single function). Keep public scaling functions homogeneous in
channel bit depth (e.g., uint8_t to uint8_t or uint16_t to uint16_t), and
implement format conversions or cross-depth subsampling separately using
dedicated conversion row functions or two-step pipelines.

**Why.** Combining geometric scaling with bit-depth conversion creates
combinatorial technical debt across all filtering modes (point, linear,
bilinear, box), scaling directions (upsampling and downsampling), and SIMD
instruction sets (SSE2, AVX2, NEON). Keeping scaling APIs homogeneous maintains
orthogonal functionality, simplifies SIMD kernel maintenance, and matches
libyuv's architecture where format conversions are handled by dedicated
conversion functions.

## Examples

Bad:

```cpp
// Anti-pattern: combining scaling and bit-depth conversion in a public API
LIBYUV_API
int ScalePlane_16To8(const uint16_t* src,
                     int src_stride,
                     int src_width,
                     int src_height,
                     uint8_t* dst,
                     int dst_stride,
                     int dst_width,
                     int dst_height,
                     enum FilterMode filtering);
```

Good:

```cpp
// Homogeneous scaling API operating on uniform channel bit depths
LIBYUV_API
int ScalePlane_16(const uint16_t* src,
                  int src_stride,
                  int src_width,
                  int src_height,
                  uint16_t* dst,
                  int dst_stride,
                  int dst_width,
                  int dst_height,
                  enum FilterMode filtering);

// Format and bit-depth conversions are exposed separately in conversion APIs
LIBYUV_API
void Convert16To8Plane(const uint16_t* src_y,
                       int src_stride_y,
                       uint8_t* dst_y,
                       int dst_stride_y,
                       int scale,
                       int width,
                       int height);
```
