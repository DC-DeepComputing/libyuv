---
name: libyuv/api/no-full-format-names-for-partial-planes
description: When declaring image conversion or scaling APIs named after full formats, accept all format planes instead of a partial subset like UV
rubric: readability
severity: should_fix
---

**Rule.** Do not name public functions after full image formats (such as
ScaleI420ToI444 or I420Scale) when they only operate on a subset of planes (such
as only U and V). Format-level conversion and scaling APIs must accept all
planes of the named format (e.g., Y, U, and V), whereas functions operating
strictly on individual planes or chroma pairs should be named after the specific
planes (e.g., ScalePlane or ScaleUV).

**Why.** Naming a function after full image formats (such as I420 or I444)
implies that it processes complete frames. Omitting required planes like the
luma (Y) plane in a format-named function misleads callers, creates inconsistent
API contracts across libyuv headers, and breaks caller expectations for
full-format conversions.

## Examples

Bad:

```cpp
// Bad: Named after full formats (I420, I444) but only accepts chroma planes (U, V).
LIBYUV_API
int ScaleI420ToI444Bilinear(const uint8_t* src_u,
                            int src_stride_u,
                            const uint8_t* src_v,
                            int src_stride_v,
                            int src_width,
                            int src_height,
                            uint8_t* dst_u,
                            int dst_stride_u,
                            uint8_t* dst_v,
                            int dst_stride_v);
```

Good:

```cpp
// Good: Format-level conversions accept all planes of the format (Y, U, and V).
LIBYUV_API
int I420ToI444(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_y,
               int dst_stride_y,
               uint8_t* dst_u,
               int dst_stride_u,
               uint8_t* dst_v,
               int dst_stride_v,
               int width,
               int height);

// Good: Functions operating on specific planes are named for those planes.
LIBYUV_API
int ScalePlane(const uint8_t* src,
               int src_stride,
               int src_width,
               int src_height,
               uint8_t* dst,
               int dst_stride,
               int dst_width,
               int dst_height,
               enum FilterMode filtering);
```
