---
name: libyuv/source/match-is-aligned-step-to-simd-vector-size
rubric: correctness
severity: should_fix
---

**Rule.** When dispatching between a direct SIMD row kernel and its _Any
tail-handling wrapper (e.g., in if (IS_ALIGNED(width, step)) checks or ternary
selection), verify that the alignment step matches the exact vector chunk size
processed per iteration by the target SIMD kernel (e.g., 8 for MMI/SSE2 64-bit,
16 for SSE2/NEON/MSA 128-bit, 32 for AVX2, 64 for AVX-512). Additionally, when
widening a SIMD kernel's unroll factor or vector processing step, update all
calling dispatchers' IS_ALIGNED checks to match the new chunk size so unaligned
tails do not bypass the _Any wrapper.

**Why.** Direct SIMD row kernels process fixed vector chunks per iteration
without internal bounds checks and require inputs to be exact multiples of their
vector step size. If IS_ALIGNED(width, step) checks against a divisor smaller
than the kernel's chunk size (or retains a stale step size after kernel
widening), widths with unaligned tails will bypass the _Any fallback wrapper and
execute the direct kernel, leading to out-of-bounds memory over-reads,
over-writes, or data corruption.

## Examples

Bad:

```cpp
// Bad: IS_ALIGNED uses SSE2/AVX2 step (16/32) instead of AVX2/AVX-512 kernel chunk size (32/64)
#if defined(HAS_ARGBTOUVMATRIXROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBToUVMatrixRow = ARGBToUVMatrixRow_Any_AVX2;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVMatrixRow = ARGBToUVMatrixRow_AVX2;
    }
  }
#endif

// Bad: Stale alignment check (8) after widening NEON kernel to 16 pixels per iteration
if (TestCpuFlag(kCpuHasNEON)) {
  ARGB4444ToYRow = ARGB4444ToYRow_Any_NEON;
  if (IS_ALIGNED(width, 8)) {
    ARGB4444ToYRow = ARGB4444ToYRow_NEON;
  }
}
```

Good:

```cpp
// Good: IS_ALIGNED step matches the exact AVX2 (32) and AVX-512 (64) vector chunk sizes
#if defined(HAS_ARGBTOUVMATRIXROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBToUVMatrixRow = ARGBToUVMatrixRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      ARGBToUVMatrixRow = ARGBToUVMatrixRow_AVX2;
    }
  }
#endif

// Good: Alignment check updated to match widened 16-pixel NEON kernel step
if (TestCpuFlag(kCpuHasNEON)) {
  ARGB4444ToYRow = ARGB4444ToYRow_Any_NEON;
  if (IS_ALIGNED(width, 16)) {
    ARGB4444ToYRow = ARGB4444ToYRow_NEON;
  }
}
```
