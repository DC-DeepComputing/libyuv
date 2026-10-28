---
name: libyuv/source/separate-odd-width-handling-into-odd-row-functions
description: When handling odd-width edges in image scaling or subsampling row kernels, define separate *_Odd row helper functions rather than adding src_width parameters or internal branching
rubric: design
severity: should_fix
---

**Rule.** When implementing row-level image scaling or subsampling kernels (such
as ScaleRowDown* or HalfWidthRow*), keep standard row function signatures
uniform by operating on dst_width without extra src_width parameters. Separate
odd source width handling into dedicated *_Odd row helper functions (e.g.,
ScaleRowDown2Box_Odd_C), and select between standard and *_Odd row kernels at
the plane-level dispatch stage.

**Why.** In libyuv, row scaler and converter kernels are dispatched via uniform
function pointer signatures and implemented across multiple SIMD instruction
sets (SSSE3, AVX2, NEON, RVV, LSX). Isolating odd-width boundary logic in
dedicated *_Odd row functions preserves uniform function pointer interfaces,
avoids branch overhead inside inner loops, and keeps SIMD vector loops clean.

## Examples

Bad:

```cpp
// Bad: Adding src_width and odd-pixel tail branching inside standard row scaler
void ScaleRowDown2_16To8_C(const uint16_t* src_ptr,
                           ptrdiff_t src_stride,
                           int src_width,
                           uint8_t* dst,
                           int dst_width) {
  int x;
  for (x = 0; x < src_width / 2 - 1; ++x) {
    dst[x] = (src_ptr[x * 2] + src_ptr[x * 2 + 1]) >> 1;
  }
  if ((src_width & 1) != 0) {
    dst[x] = src_ptr[x * 2];
  }
}
```

Good:

```cpp
// Good: Standard row function operates uniformly on dst_width
void ScaleRowDown2_16To8_C(const uint16_t* src_ptr,
                           ptrdiff_t src_stride,
                           uint8_t* dst,
                           int dst_width) {
  for (int x = 0; x < dst_width; ++x) {
    dst[x] = (src_ptr[x * 2] + src_ptr[x * 2 + 1]) >> 1;
  }
}

// Dedicated *_Odd row function handles odd source width at the boundary
void ScaleRowDown2_16To8_Odd_C(const uint16_t* src_ptr,
                               ptrdiff_t src_stride,
                               uint8_t* dst,
                               int dst_width) {
  for (int x = 0; x < dst_width - 1; ++x) {
    dst[x] = (src_ptr[x * 2] + src_ptr[x * 2 + 1]) >> 1;
  }
  dst[dst_width - 1] = src_ptr[(dst_width - 1) * 2];
}
```
