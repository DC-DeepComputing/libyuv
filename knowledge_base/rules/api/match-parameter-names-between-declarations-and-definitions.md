---
name: libyuv/api/match-parameter-names-between-declarations-and-definitions
description: When declaring or defining functions across headers, source files, and row macros, match parameter names identically between declarations and definitions
rubric: readability
severity: should_fix
---

**Rule.** Ensure function parameter names match identically between header
declarations (e.g., in include/libyuv/*.h and include/libyuv/row.h) and their
corresponding source definitions (e.g., in source/*.cc and source/row_any.cc),
including definitions instantiated via macro templates such as ANY11C, ANY12PT,
and ANY*. When renaming parameters, update all paired header prototypes, source
definitions, and dispatch macros to prevent identifier drift.

**Why.** Mismatched parameter names between declarations and definitions harm
readability, cause confusion for API consumers and implementers, hinder IDE
tooling, and trigger compiler or linter diagnostics such as clang-tidy
readability-inconsistent-declaration-parameter-name and -Wdocumentation.

## Examples

Bad:

```cpp
// include/libyuv/row.h
void DivideRow_16_Any_AVX2(const uint16_t* src_y,
                           uint16_t* dst_y,
                           int scale,
                           int width);
void SplitUVRow_16_Any_AVX2(const uint16_t* src_uv,
                            uint16_t* dst_u,
                            uint16_t* dst_v,
                            int depth,
                            int width);

// source/row_any.cc
#define ANY11C(NAMEANY, ANY_SIMD, ...)                       \
  void NAMEANY(const STYPE* src_ptr, DTYPE* dst_ptr,         \
               int scale, int width) { ... }

#define ANY12PT(NAMEANY, ANY_SIMD, T, BPP, MASK)                            \
  void NAMEANY(const T* uv_buf, T* u_buf, T* v_buf, int depth, int width) { \
    ...                                                                     \
  }
```

Good:

```cpp
// include/libyuv/row.h
void DivideRow_16_Any_AVX2(const uint16_t* src_ptr,
                           uint16_t* dst_ptr,
                           int scale,
                           int width);
void SplitUVRow_16_Any_AVX2(const uint16_t* src_uv,
                            uint16_t* dst_u,
                            uint16_t* dst_v,
                            int depth,
                            int width);

// source/row_any.cc
#define ANY11C(NAMEANY, ANY_SIMD, ...)                       \
  void NAMEANY(const STYPE* src_ptr, DTYPE* dst_ptr,         \
               int scale, int width) { ... }

#define ANY12PT(NAMEANY, ANY_SIMD, T, BPP, MASK)                            \
  void NAMEANY(const T* src_uv, T* dst_u, T* dst_v, int depth, int width) { \
    ...                                                                     \
  }
```
