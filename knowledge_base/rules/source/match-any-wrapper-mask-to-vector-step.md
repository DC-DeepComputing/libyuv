---
name: libyuv/source/match-any-wrapper-mask-to-vector-step
description: When defining SIMD remainder wrapper functions using ANY* or scale ANY macros in row_any.cc or scale_any.cc, set MASK to vector_step - 1
rubric: correctness
severity: should_fix
---

**Rule.** When instantiating SIMD remainder wrapper functions using ANY* macros
in source/row_any.cc or scale ANY macros (such as SDANY, SAANY, CANY) in
source/scale_any.cc, ensure the MASK argument equals the vector chunk size minus
one (pixel_step - 1) of the wrapped SIMD kernel. For example, use a mask of 31
for 32-pixel AVX2 kernels, 15 for 16-pixel SSE2/NEON kernels, 63 for 64-pixel
AVX-512 kernels, and 7 for 8-pixel kernels. Never copy a mask from a narrower
architecture (like SSE2 with 15) when defining wrappers for wider vector kernels
(like AVX2 with 31).

**Why.** The MASK parameter splits row processing into SIMD-aligned vector
chunks (width & ~MASK) and scalar remainder pixels (width & MASK). Passing an
undersized mask (such as 15 instead of 31 for a 32-pixel kernel) causes the SIMD
kernel to be invoked with non-multiple iteration counts, leading to incorrect
calculations or out-of-bounds memory over-reads and over-writes.

## Examples

Bad:

```cpp
// Bad: AVX2 kernel processes 32 pixels per iteration, but MASK is set to 15 (SSE2 mask)
ANY12MS(RGB565ToUVMatrixRow_Any_AVX2, RGB565ToUVMatrixRow_AVX2, 0, 2, 15)
ANY11(ARGBToRGB24Row_Any_AVX2, ARGBToRGB24Row_AVX2, 0, 4, 3, 15)
```

Good:

```cpp
// Good: MASK is set to pixel_step - 1 (31 for 32-pixel AVX2 kernels)
ANY12MS(RGB565ToUVMatrixRow_Any_AVX2, RGB565ToUVMatrixRow_AVX2, 0, 2, 31)
ANY11(ARGBToRGB24Row_Any_AVX2, ARGBToRGB24Row_AVX2, 0, 4, 3, 31)
```
