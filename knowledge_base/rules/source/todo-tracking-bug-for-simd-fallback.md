---
name: libyuv/source/todo-tracking-bug-for-simd-fallback
description: When introducing image conversion or row-processing functions with only a C scalar fallback, add a TODO comment with a tracking bug for SIMD optimizations
rubric: readability
severity: nit
---

**Rule.** When introducing a new image conversion, scaling, or row-processing
routine that only provides a portable C scalar fallback, add a TODO comment
referencing the tracking bug to implement optimized architecture-specific SIMD
versions (e.g., NEON and AVX2). Ensure the comment includes an explicit issue
reference rather than an unassigned or bare TODO.

**Why.** libyuv relies on SIMD implementations for performant image processing.
Tracking missing SIMD kernels with an explicit tracking bug ensures that
follow-up vectorization work is documented, prioritized, and completed.

## Examples

Bad:

```cpp
// C version
void (*InterpolateRow_16To8)(...) = InterpolateRow_16To8_C;
```

Good:

```cpp
// TODO(bug 930): Add NEON and AVX2 versions.
void (*InterpolateRow_16To8)(...) = InterpolateRow_16To8_C;
```
