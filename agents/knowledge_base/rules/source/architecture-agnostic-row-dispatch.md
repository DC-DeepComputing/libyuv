---
name: libyuv/source/architecture-agnostic-row-dispatch
description: 'When dispatching SIMD row kernels in high-level conversion and planar functions, avoid architecture #ifdef checks and use uniform alignment checks with _Any_ fallbacks'
rubric: readability
severity: should_fix
---

**Rule.** In high-level planar, conversion, and scale functions across source/,
avoid #ifdef __aarch64__ or other architecture-specific preprocessor checks when
selecting row kernels. Keep row-function dispatch logic uniform across 32-bit
and 64-bit targets by using uniform alignment checks (such as IS_ALIGNED(width,
16)) and relying on _Any_ fallback kernels for remainder handling.

**Why.** High-level conversion and planar routines abstract SIMD implementations
behind HAS_* macros, TestCpuFlag(...), and row function pointers (_Any_
fallbacks vs. vector loops). Branching on target architecture bitness clutters
high-level dispatchers with redundant #ifdefs, whereas uniform alignment checks
allow vector kernels to execute on full unrolled blocks while _Any_ kernels
cleanly handle remaining elements on all architectures.

## Examples

Bad:

```cpp
#ifdef __aarch64__
  if (IS_ALIGNED(width, 16)) {
    HalfFloatRow = scale == 1.0f ? HalfFloat1Row_NEON : HalfFloatRow_NEON;
  }
#else
  if (IS_ALIGNED(width, 8)) {
    HalfFloatRow = scale == 1.0f ? HalfFloat1Row_NEON : HalfFloatRow_NEON;
  }
#endif
```

Good:

```cpp
  if (IS_ALIGNED(width, 16)) {
    HalfFloatRow = scale == 1.0f ? HalfFloat1Row_NEON : HalfFloatRow_NEON;
  }
```
