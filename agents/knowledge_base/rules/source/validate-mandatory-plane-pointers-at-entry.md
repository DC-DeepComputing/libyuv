---
name: libyuv/source/validate-mandatory-plane-pointers-at-entry
description: When implementing image conversion, planar, or comparison functions, validate all mandatory plane pointers and dimensions upfront at entry before subsampling arithmetic, pointer adjustments, or coalescing
rubric: correctness
severity: blocking
---

**Rule.** In image conversion, planar manipulation, scaling, and comparison
functions, validate all mandatory source and destination plane pointers
(including alpha planes like src_a or dst_a) and image dimensions upfront at
function entry before performing any subsampled plane arithmetic (such as (width
+ 1) >> 1 or (height + 1) / 2), pointer adjustments for negative height
inversion, or row coalescing (width *= height). For conversion and
transformation functions supporting negative height vertical flipping, check
width <= 0 || height == 0 || height == INT_MIN and return -1. For comparison and
analysis functions, check width <= 0 || height <= 0 and return early with -1 or
a default metric value. Additionally, ensure dimension boundary validation
(including INT_MAX / INT_MIN) precedes subsampling calculations to prevent
undefined signed integer overflow.

**Why.** Libyuv entry functions contractually validate input and output buffers
and image dimensions before processing. Omitting a required plane pointer check
(such as !dst_a) or dimension check causes null pointer dereferences,
out-of-bounds memory accesses, or integer overflow. Performing subsampling
arithmetic like (width + 1) >> 1 prior to validation invokes signed integer
overflow if width == INT_MAX. Furthermore, failing to return early on
non-positive dimensions before negative-height pointer arithmetic or row
coalescing (width *= height) corrupts buffer offsets and loop iterations.

## Examples

Bad:

```cpp
// 1. Subsampling arithmetic evaluated before entry checks; missing dst_a check
int ARGBToI420Alpha(const uint8_t* src_argb, int src_stride_argb,
                    uint8_t* dst_y, int dst_stride_y,
                    uint8_t* dst_u, int dst_stride_u,
                    uint8_t* dst_v, int dst_stride_v,
                    uint8_t* dst_a, int dst_stride_a,
                    int width, int height) {
  int halfwidth = (width + 1) >> 1;
  int halfheight = (height + 1) >> 1;
  if (!src_argb || !dst_y || !dst_u || !dst_v || width <= 0 || height == 0) {
    return -1;
  }
  // ...
}

// 2. Inversion pointer adjustments and row coalescing without entry return
if (height < 0) {
  height = -height;
  dst_y = dst_y + (height - 1) * dst_stride_y;
  dst_stride_y = -dst_stride_y;
}
if (src_stride_y == width && dst_stride_y == width) {
  width *= height;
  height = 1;
}

// 3. Comparison function without upfront pointer and positive dimension checks
uint64_t ComputeSumSquareErrorPlane(const uint8_t* src_a, int stride_a,
                                    const uint8_t* src_b, int stride_b,
                                    int width, int height) {
  uint64_t sse = 0;
  if (stride_a == width && stride_b == width && height > 0) { ... }
  return sse;
}
```

Good:

```cpp
// 1. Validate all planes (including dst_a) and dimensions before subsampling math
int ARGBToI420Alpha(const uint8_t* src_argb, int src_stride_argb,
                    uint8_t* dst_y, int dst_stride_y,
                    uint8_t* dst_u, int dst_stride_u,
                    uint8_t* dst_v, int dst_stride_v,
                    uint8_t* dst_a, int dst_stride_a,
                    int width, int height) {
  if (!src_argb || !dst_y || !dst_u || !dst_v || !dst_a || width <= 0 ||
      height == 0 || height == INT_MIN) {
    return -1;
  }
  int halfwidth = (width + 1) >> 1;
  int halfheight = (height + 1) >> 1;
  // ...
}

// 2. Validate width <= 0 || height == 0 || height == INT_MIN before pointer/coalesce math
if (width <= 0 || height == 0 || height == INT_MIN) {
  return -1;
}
if (height < 0) {
  height = -height;
  dst_y = dst_y + (height - 1) * dst_stride_y;
  dst_stride_y = -dst_stride_y;
}
if (src_stride_y == width && dst_stride_y == width) {
  width *= height;
  height = 1;
}

// 3. Plane comparison: validate input pointers and positive dimensions upfront
uint64_t ComputeSumSquareErrorPlane(const uint8_t* src_a, int stride_a,
                                    const uint8_t* src_b, int stride_b,
                                    int width, int height) {
  if (!src_a || !src_b || width <= 0 || height <= 0) {
    return 0;
  }
  uint64_t sse = 0;
  // ...
  return sse;
}
```
