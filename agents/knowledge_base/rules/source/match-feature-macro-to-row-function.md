---
name: libyuv/source/match-feature-macro-to-row-function
rubric: correctness
severity: should_fix
---

**Rule.** Ensure every SIMD row function, remainder wrapper, and calling
dispatcher is guarded by its own dedicated #ifdef HAS_<FUNCTION>_<ARCH> (or
HAS_<ROWFUNC>_<ARCH>) feature macro in include/libyuv/row.h or scale_row.h that
exactly matches the specific function name. Do not reuse or share a single macro
across paired or related row functions (such as ARGB and XRGB variants), and
never group multiple distinct remainder wrappers (such as ANY* macros in
row_any.cc or SDANY in scale_any.cc) under an unrelated function's guard block.

**Why.** In libyuv, HAS_* feature macros provide 1:1 conditional compilation
control for individual SIMD kernels. Reusing a macro across paired row functions
or grouping multiple row wrappers under an unrelated function's #ifdef block
creates false coupling and identifier mismatches, causing compilation failures
or leaving wrappers undefined when individual SIMD features are enabled or
disabled independently in specific target configurations.

## Examples

Bad:

```cpp
// Bad: Reusing HAS_MERGEARGBROW_NEON macro for MergeXRGBRow dispatcher
#if defined(HAS_MERGEARGBROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    MergeXRGBRow = MergeXRGBRow_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      MergeXRGBRow = MergeXRGBRow_NEON;
    }
  }
#endif

// Bad: SplitUVRow_Any_MSA is grouped under YUY2ToUV422Row_MSA's guard
#ifdef HAS_YUY2TOUV422ROW_MSA
ANY12(YUY2ToUV422Row_Any_MSA, YUY2ToUV422Row_MSA, 1, 4, 1, 31)
ANY12(SplitUVRow_Any_MSA, SplitUVRow_MSA, 0, 2, 0, 31)
#endif
```

Good:

```cpp
// Good: Dedicated macro strictly matching the specific row function
#if defined(HAS_MERGEXRGBROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    MergeXRGBRow = MergeXRGBRow_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      MergeXRGBRow = MergeXRGBRow_NEON;
    }
  }
#endif

// Good: Each SIMD remainder wrapper is guarded by its own dedicated feature macro
#ifdef HAS_YUY2TOUV422ROW_MSA
ANY12(YUY2ToUV422Row_Any_MSA, YUY2ToUV422Row_MSA, 1, 4, 1, 31)
#endif
#ifdef HAS_SPLITUVROW_MSA
ANY12(SplitUVRow_Any_MSA, SplitUVRow_MSA, 0, 2, 0, 31)
#endif
```
