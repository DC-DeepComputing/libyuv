---
name: libyuv/api/group-specialized-variants-with-base-declarations
description: Place specialized and Matrix conversion function declarations and definitions immediately after their base counterparts with explanatory parameter documentation
rubric: readability
severity: should_fix
---

**Rule.** In libyuv header declarations and source implementations, place
specialized or parameterized function variants (such as `*Matrix`, `*16`, or
`*Alpha` conversions) immediately after their corresponding base function rather
than grouping them in separate sections. In public headers, accompany
specialized variants with an explanatory doc comment detailing matrix usage,
parameter conventions, or constants tables.

**Why.** Declaring and defining specialized variants adjacent to their primary
conversion function keeps headers and source files organized, discoverable, and
easy to maintain. Documenting constant tables and matrix conventions alongside
variant declarations clarifies parameter expectations for API consumers.

## Examples

Bad:

```cpp
// Bad: Matrix and specialized variants placed before or separated from base functions.
LIBYUV_API
int ARGBToI422Matrix(const uint8_t* src_argb, int src_stride_argb, ...);

LIBYUV_API
int ARGBToNV12Matrix(const uint8_t* src_argb, int src_stride_argb,
                    uint8_t* dst_y, int dst_stride_y,
                    uint8_t* dst_uv, int dst_stride_uv,
                    const struct YuvConstants* yuvconstants,
                    int width, int height);

// Convert ARGB To NV12.
LIBYUV_API
int ARGBToNV12(const uint8_t* src_argb, int src_stride_argb,
               uint8_t* dst_y, int dst_stride_y,
               uint8_t* dst_uv, int dst_stride_uv,
               int width, int height);
```

Good:

```cpp
// Good: Base function followed immediately by its documented specialized variant.
// Convert ARGB To NV12.
LIBYUV_API
int ARGBToNV12(const uint8_t* src_argb, int src_stride_argb,
               uint8_t* dst_y, int dst_stride_y,
               uint8_t* dst_uv, int dst_stride_uv,
               int width, int height);

// ARGB to NV12 with matrix. See ArgbConstants at top of file for usage.
LIBYUV_API
int ARGBToNV12Matrix(const uint8_t* src_argb, int src_stride_argb,
                    uint8_t* dst_y, int dst_stride_y,
                    uint8_t* dst_uv, int dst_stride_uv,
                    const struct YuvConstants* yuvconstants,
                    int width, int height);
```
