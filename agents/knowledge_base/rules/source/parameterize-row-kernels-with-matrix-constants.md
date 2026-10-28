---
name: libyuv/source/parameterize-row-kernels-with-matrix-constants
description: When implementing color-space conversion row kernels, parameterize the SIMD row functions with matrix constant pointers rather than creating per-architecture wrapper row functions
rubric: design
severity: should_fix
---

**Rule.** Do not write architecture-specific row function wrappers (e.g.
*Row_NEON or *Row_SSSE3) that hardcode or bind color-space matrix constants.
Instead, expose low-level row functions parameterized with matrix constant
pointers (e.g. *MatrixRow_NEON), and have high-level frame conversion functions
in source/convert*.cc pass the appropriate color-space matrix constants.

**Why.** Hardcoding color-space matrix constants in per-architecture row
wrappers duplicates boilerplate functions across every SIMD target file (NEON,
SSSE3, AVX2, RVV, etc.). Parameterizing row kernels with matrix pointers and
binding constants in higher-level conversion functions centralizes color-space
management and reduces code duplication across backends.

## Examples

Bad:

```cpp
// In source/row_neon.cc (duplicated across SSSE3, AVX2, RVV, etc.):
void ARGBToUV444Row_NEON(const uint8_t* src_argb,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width) {
  ARGBToUV444MatrixRow_NEON(src_argb, dst_u, dst_v, width,
                            &kArgbI601Constants);
}
```

Good:

```cpp
// In source/row_neon.cc: expose matrix-parameterized row function.
void ARGBToUV444MatrixRow_NEON(const uint8_t* src_argb,
                               uint8_t* dst_u,
                               uint8_t* dst_v,
                               int width,
                               const struct ArgbConstants* c);

// In source/convert_from_argb.cc: bind matrix constants at conversion level.
int ARGBToI444(const uint8_t* src_argb, int src_stride_argb, ...) {
  return ARGBToI444Matrix(src_argb, src_stride_argb, ..., &kArgbI601Constants,
                          width, height);
}
```
