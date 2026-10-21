---
name: libyuv/source/match-simd-feature-macro-cpu-flag-and-function-name
rubric: correctness
severity: should_fix
---

**Rule.** When dispatching architecture-optimized SIMD kernels, ensure that the
compile-time `#if defined(HAS_*)` / `#ifdef HAS_*` feature macro, the runtime
`TestCpuFlag(kCpuHas*)` check, and the assigned SIMD function name consistently
specify the exact same instruction set extension level (e.g., SSE2 vs. SSSE3,
AVX2 vs. AVX512, or NEON vs. SVE2).

**Why.** Mismatches between the compile-time feature guard, runtime CPU flag
check, and kernel function name cause logic bugs where optimized paths are
skipped on capable CPUs or unsupported instructions are executed on older
hardware.

## Examples

Bad:

```cpp
#ifdef HAS_SCALEROWUP2LINEAR_16_SSE2
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ScaleRowUp = ScaleRowUp2_Linear_16_Any_SSSE3;
  }
#endif
```

Good:

```cpp
#ifdef HAS_SCALEROWUP2LINEAR_16_SSE2
  if (TestCpuFlag(kCpuHasSSE2)) {
    ScaleRowUp = ScaleRowUp2_Linear_16_Any_SSE2;
  }
#endif
```
