---
name: libyuv/api/no-duplicate-function-declarations
description: When adding or updating function declarations in header files, verify the function is not already declared earlier to avoid duplicate declarations
rubric: readability
severity: should_fix
---

**Rule.** Do not add duplicate function declarations in header files. When
adding new API declarations or conversion variants, verify that identical or
existing function prototypes are not already declared elsewhere in the header.

**Why.** Compilers silently accept identical C/C++ function re-declarations,
allowing accidental copy-paste duplicates to slip through unnoticed. Duplicate
declarations bloat the public API surface, confuse consumers, and introduce
maintenance hazards if one signature is updated while its duplicate is
overlooked.

## Examples

Bad:

```cpp
// Convert ARGB to J420. (JPEG full range I420).
LIBYUV_API
int ARGBToJ420(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_yj,
               int dst_stride_yj,
               uint8_t* dst_u,
               int dst_stride_u,
               uint8_t* dst_v,
               int dst_stride_v,
               int width,
               int height);

// Redundant duplicate declaration later in the same header:
LIBYUV_API
int ARGBToJ420(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_yj,
               int dst_stride_yj,
               uint8_t* dst_u,
               int dst_stride_u,
               uint8_t* dst_v,
               int dst_stride_v,
               int width,
               int height);
```

Good:

```cpp
// Convert ARGB to J420. (JPEG full range I420).
LIBYUV_API
int ARGBToJ420(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_yj,
               int dst_stride_yj,
               uint8_t* dst_u,
               int dst_stride_u,
               uint8_t* dst_v,
               int dst_stride_v,
               int width,
               int height);
```
