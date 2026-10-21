---
name: libyuv/source/no-row-coalescing-in-multipass-conversion-loops
rubric: safety
severity: should_fix
---

**Rule.** Do not coalesce rows (`width *= height; height = 1;`) in image
conversion loops that call multiple row processing kernels per scanline on the
same source buffer (such as separate Y and UV plane extraction kernels). Keep
row-by-row iteration so each scanline remains in CPU cache across consecutive
kernel passes.

**Why.** When a conversion function executes multiple row kernels per iteration
over the same source buffer, row-by-row processing keeps scanlines in L1/L2
cache for subsequent kernels. Coalescing rows into a single long row causes the
first kernel to stream through the entire image and evict it from cache, forcing
subsequent kernels to reload the entire frame from main memory and degrading
performance.

## Examples

Bad:

```cpp
// Coalesce rows.
if (src_stride_argb == width * 4 && dst_stride_y == width &&
    dst_stride_u == width && dst_stride_v == width) {
  width *= height;
  height = 1;
  src_stride_argb = dst_stride_y = dst_stride_u = dst_stride_v = 0;
}

for (y = 0; y < height; ++y) {
  ARGBToYMatrixRow(src_argb, dst_y, width, argbconstants);
  ARGBToUV444MatrixRow(src_argb, dst_u, dst_v, width, argbconstants);
  src_argb += src_stride_argb;
  dst_y += dst_stride_y;
  dst_u += dst_stride_u;
  dst_v += dst_stride_v;
}
```

Good:

```cpp
for (y = 0; y < height; ++y) {
  ARGBToYMatrixRow(src_argb, dst_y, width, argbconstants);
  ARGBToUV444MatrixRow(src_argb, dst_u, dst_v, width, argbconstants);
  src_argb += src_stride_argb;
  dst_y += dst_stride_y;
  dst_u += dst_stride_u;
  dst_v += dst_stride_v;
}
```
