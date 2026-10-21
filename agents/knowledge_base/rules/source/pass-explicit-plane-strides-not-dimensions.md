---
name: libyuv/source/pass-explicit-plane-strides-not-dimensions
rubric: correctness
severity: blocking
---

**Rule.** When invoking plane transformation functions (such as RotatePlane,
ScalePlane, MirrorPlane, or CopyPlane), pass explicit plane stride arguments
(such as dst_stride_y, dst_stride_u, or src_stride_y) corresponding to each
buffer. Never substitute image dimension variables (such as height, halfheight,
width, or halfwidth) in place of stride parameters.

**Why.** Planar buffers frequently contain row padding, custom memory alignment,
or negative strides for vertical inversion where the row stride differs from the
image width or height. Hardcoding dimension variables as stride arguments
ignores caller-provided layout and causes row misalignment or buffer overruns.

## Examples

Bad:

```cpp
// Bad: Passing image dimensions 'height' or 'halfheight' as stride arguments.
RotatePlane90_16(src_u, src_stride_u, dst_u, height, halfwidth, height);
ScalePlane_16(src_y, src_stride_y, width, height,
              dst_y, height, halfwidth, halfheight, kFilterBilinear);
```

Good:

```cpp
// Good: Passing explicit plane stride arguments for source and destination.
RotatePlane90_16(src_u, src_stride_u, dst_u, dst_stride_u, halfwidth,
                 height);
ScalePlane_16(src_y, src_stride_y, width, height,
              dst_y, dst_stride_y, halfwidth, halfheight, kFilterBilinear);
```
