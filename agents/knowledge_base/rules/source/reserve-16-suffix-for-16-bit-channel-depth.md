---
name: libyuv/source/reserve-16-suffix-for-16-bit-channel-depth
description: When defining SIMD row helper macros for multi-pixel operations, avoid `_16` suffixes for pixel counts and use `_D` or `_2X`, reserving `_16` for 16-bit channels
rubric: readability
severity: should_fix
---

**Rule.** In SIMD row kernels and helper macros, do not use `_16` suffixes to
denote processing 16 pixels or unrolled batches. Reserve the `_16` suffix
exclusively for 16-bit per channel (high bit-depth) image formats and functions
(such as ScalePlane_16), and use suffixes like `_D` or `_2X` for double-width or
multi-pixel macro variants.

**Why.** In libyuv, the `_16` suffix universally designates 16-bit per channel
pixel buffers and operations operating on uint16_t data. Using `_16` to indicate
a loop unroll factor or pixel count creates ambiguity between pixel count and
channel bit-depth.

## Examples

Bad:

```cpp
// Bad: using _16 suffix for processing 16 8-bit pixels
#define STOREARGB_16(a_l, a_h, r_l, r_h, g_l, g_h, b_l, b_h, pdst_argb) \
  ...
```

Good:

```cpp
// Good: use _D or _2X for multi-vector/double-width macros on 8-bit data
#define STOREARGB_D(a_l, a_h, r_l, r_h, g_l, g_h, b_l, b_h, pdst_argb) \
  ...

// Good: reserve _16 suffix for operations on 16-bit per channel buffers
void ScalePlane_16(const uint16_t* src, int src_stride, ...);
```
