---
name: libyuv/api/accurately-describe-function-variants-in-doc-comments
description: When commenting function variants or format conversions, ensure doc comments accurately describe the specific variant rather than retaining stale comments copied from base functions
rubric: readability
severity: should_fix
---

**Rule.** When adding, updating, or declaring function variants (such as Alpha,
Matrix, or Filter variations of base format conversions), ensure doc comments
accurately specify the exact variant, qualifiers, and planes being handled.
Avoid copying comments from base functions without updating them to reflect the
variant's behavior.

**Why.** Stale or copy-pasted comments that omit variant qualifiers (such as
'with Alpha' or matrix transformations) mislead callers about what channels,
planes, and transformations the function actually performs.

## Examples

Bad:

```cpp
// Convert ARGB to I420.
LIBYUV_API
int ARGBToI420Alpha(const uint8_t* src_argb,
                    int src_stride_argb,
                    uint8_t* dst_y,
                    int dst_stride_y,
                    uint8_t* dst_u,
                    int dst_stride_u,
                    uint8_t* dst_v,
                    int dst_stride_v,
                    uint8_t* dst_a,
                    int dst_stride_a,
                    int width,
                    int height);
```

Good:

```cpp
// Convert ARGB to I420 with Alpha.
LIBYUV_API
int ARGBToI420Alpha(const uint8_t* src_argb,
                    int src_stride_argb,
                    uint8_t* dst_y,
                    int dst_stride_y,
                    uint8_t* dst_u,
                    int dst_stride_u,
                    uint8_t* dst_v,
                    int dst_stride_v,
                    uint8_t* dst_a,
                    int dst_stride_a,
                    int width,
                    int height);
```
