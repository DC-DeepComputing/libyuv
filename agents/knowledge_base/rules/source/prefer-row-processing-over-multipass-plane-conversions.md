---
name: libyuv/source/prefer-row-processing-over-multipass-plane-conversions
description: When combining spatial scaling or subsampling with bit-depth conversion, process rows iteratively or use fused row kernels instead of multi-pass full-plane conversions
rubric: safety
severity: should_fix
---

**Rule.** When combining spatial scaling, subsampling, or channel
transformations with bit-depth conversion, avoid multi-pass operations that
process entire planes into temporary buffers. Structure the conversion
row-by-row in a streaming loop or implement a fused row function to maximize CPU
cache locality and eliminate redundant memory traffic.

**Why.** Performing multiple full-plane passes forces entire frame buffers
through memory, evicting CPU caches and incurring high memory bandwidth
overhead. Streaming row-by-row or using fused row kernels keeps working data in
L1/L2 cache and avoids allocating or reusing large temporary buffers.

## Examples

Bad:

```cpp
// Bad: Scale full UV planes into a temporary buffer and convert bit depth in separate passes.
ScalePlane_12(src_u, src_stride_u, width, height, (uint16_t*)dst_y,
              uv_stride, uv_width, uv_height, kFilterBilinear);
Convert16To8Plane((uint16_t*)dst_y, uv_stride, dst_u, dst_stride_u, scale,
                  uv_width, uv_height);
```

Good:

```cpp
// Good: Stream row-by-row or use a fused row conversion kernel for cache efficiency.
for (y = 0; y < uv_height; ++y) {
  HalfWidthRow_16To8(src_u, src_stride_u, dst_u, scale, width);
  src_u += src_stride_u * 2;
  dst_u += dst_stride_u;
}
```
