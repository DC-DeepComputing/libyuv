---
name: libyuv/api/order-yuvconstants-before-dimensions
description: In libyuv conversion and row functions accepting a YuvConstants pointer, place the yuvconstants parameter immediately before width and height, keeping dimensions last
rubric: readability
severity: should_fix
---

**Rule.** In libyuv conversion, rotation, or row functions that take a
YuvConstants pointer (such as matrix conversion functions), place the
`yuvconstants` parameter immediately before `width` and `height`, keeping image
dimensions as the final parameters in the function signature.

**Why.** Consistent parameter ordering across libyuv's public API headers and
internal row functions maintains uniform API design, prevents confusion at call
sites, and matches the project convention of placing image dimensions at the end
of parameter lists.

## Examples

Bad:

```cpp
int I420ToRGB565Matrix(const uint8_t* src_y,
                       int src_stride_y,
                       const uint8_t* src_u,
                       int src_stride_u,
                       const uint8_t* src_v,
                       int src_stride_v,
                       uint8_t* dst_rgb565,
                       int dst_stride_rgb565,
                       int width,
                       int height,
                       const struct YuvConstants* yuvconstants);
```

Good:

```cpp
int I420ToRGB565Matrix(const uint8_t* src_y,
                       int src_stride_y,
                       const uint8_t* src_u,
                       int src_stride_u,
                       const uint8_t* src_v,
                       int src_stride_v,
                       uint8_t* dst_rgb565,
                       int dst_stride_rgb565,
                       const struct YuvConstants* yuvconstants,
                       int width,
                       int height);
```
