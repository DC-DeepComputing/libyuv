---
name: libyuv/source/handle-negative-height-by-inverting-strides
description: When supporting vertical inversion with negative source height, use absolute height for secondary plane offsets and invert height, pointers, and strides
rubric: correctness
severity: blocking
---

**Rule.** When supporting vertical image inversion via negative source height in
format conversion, planar manipulation, rotation, or scaling functions:
1. Use absolute height (e.g., abs_src_height = (height < 0) ? -height : height or abs(height)) when computing pointer offsets to secondary planes (such as UV in NV12/NV21 or U/V in I420/I422) in contiguous memory buffers (sample, src_frame), rather than signed height.
2. Normalize negative height and invert strides: check if (height < 0), set height = -height, advance source plane pointers to the start of the last row (src + (height - 1) * src_stride), and negate source strides (src_stride = -src_stride). For subsampled planes (such as UV at half height), adjust pointers using their respective plane heights (e.g., halfheight - 1).

**Why.** In libyuv, passing a negative source height is the project-wide
convention for specifying vertical image inversion. Using signed negative height
when computing secondary plane pointer offsets produces negative arithmetic
offsets that point before the allocated buffer and cause memory corruption.
Furthermore, if negative height is not normalized and source pointers and
strides are not inverted before invoking row kernels or subroutines, loops will
underflow or process out-of-bounds memory addresses, resulting in ASan crashes
or corrupted output.

## Examples

Bad:

```cpp
// 1. Multiplying by signed src_height produces a negative offset when src_height < 0.
src_uv = sample + ((ptrdiff_t)src_width * src_height) +
         ((crop_y / 2) * aligned_src_width) + ((crop_x / 2) * 2);

// 2. Missing negative height normalization and stride inversion causes ASan buffer overflows.
const int halfheight = (height + 1) >> 1;
Convert16To8Plane(src_y, src_stride_y, dst_y, dst_stride_y, scale, width, height);
ScalePlane_16To8(src_u, src_stride_u, src_uv_width, halfheight, dst_u, ...);
```

Good:

```cpp
// 1. Use absolute height for contiguous secondary plane offsets.
const int abs_src_height = (src_height < 0) ? -src_height : src_height;
src_uv = sample + ((ptrdiff_t)src_width * abs_src_height) +
         ((crop_y / 2) * aligned_src_width) + ((crop_x / 2) * 2);

// 2. Invert height, advance source plane pointers to the last row, and negate strides.
if (height < 0) {
  height = -height;
  halfheight = (height + 1) >> 1;
  src_y = src_y + (ptrdiff_t)(height - 1) * src_stride_y;
  src_u = src_u + (ptrdiff_t)(halfheight - 1) * src_stride_u;
  src_stride_y = -src_stride_y;
  src_stride_u = -src_stride_u;
}
```
