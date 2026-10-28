---
name: libyuv/source/avoid-trailing-underscores-on-row-function-pointers
description: When declaring local function pointers for SIMD row kernel dispatch, name them after the base row function without trailing underscores or implementation suffixes
rubric: readability
severity: should_fix
---

**Rule.** When declaring local function pointer variables for SIMD row function
dispatch, name the variable in PascalCase matching the base row function name
(e.g., MergeUVRow), without trailing underscores or implementation/architecture
suffixes. Do not use trailing underscores (such as MergeUVRow_), which are
reserved for class member variables, nor architecture-specific names on the
dispatch variable.

**Why.** Trailing underscores are reserved for class data members in Google C++
style and should not appear on local variables. Naming the local function
pointer directly after the base row function provides a clean, uniform dispatch
interface for architecture-specific kernel selection throughout libyuv
conversion and scaling routines.

## Examples

Bad:

```cpp
void (*MergeUVRow_)(const uint8_t* src_u, const uint8_t* src_v,
                    uint8_t* dst_uv, int width) = MergeUVRow_C;
#if defined(HAS_MERGEUVROW_NEON)
if (TestCpuFlag(kCpuHasNEON)) {
  MergeUVRow_ = MergeUVRow_NEON;
}
#endif
MergeUVRow_(src_u, src_v, dst_uv, width);
```

Good:

```cpp
void (*MergeUVRow)(const uint8_t* src_u, const uint8_t* src_v,
                   uint8_t* dst_uv, int width) = MergeUVRow_C;
#if defined(HAS_MERGEUVROW_NEON)
if (TestCpuFlag(kCpuHasNEON)) {
  MergeUVRow = MergeUVRow_NEON;
}
#endif
MergeUVRow(src_u, src_v, dst_uv, width);
```
