---
name: libyuv/source/use-zero-stride-for-edge-rows-in-multi-row-scaling
description: When invoking multi-row kernels on boundary and odd rows, pass literal 0 for source stride rather than mutating stride variables or calling 1D scalers
rubric: readability
severity: should_fix
---

**Rule.** In image plane upscaling, downscaling, and color conversion routines
using multi-row kernels (such as Scale2RowUp, ScaleRowDown*, ARGBToUVMatrixRow,
or InterpolateRow), pass literal 0 for the source stride on edge, boundary, and
trailing odd-height rows (height & 1). Avoid branching on height to invoke
separate 1D scalers or memcpy, and pass literal 0 directly at the call site
rather than mutating a local stride variable before the call.

**Why.** Multi-row interpolation and box-filter kernels sample both src_ptr and
src_ptr + src_stride to compute vertical averages. Passing literal src_stride =
0 clamps vertical sampling to the current row, naturally handling
single/boundary rows without special-case 1D kernels while preventing buffer
overreads on final odd rows without relying on mutable local stride variables.

## Examples

Bad:

```cpp
// Bad: special-casing 1D scalers or memcpy for edge rows
if (src_height == 1) {
  ScaleRowUp(src_ptr, dst_ptr, dst_width);
  memcpy(dst_ptr + dst_stride, dst_ptr, dst_width);
}

// Bad: dereferencing past buffer, or mutating local stride variable to 0
if (src_height & 1) {
  src_stride = 0;  // Mutating local stride variable
  ScaleRowDown2(src_ptr, src_stride, dst_ptr, dst_width, scale);
}
```

Good:

```cpp
// Good: pass literal 0 for edge rows in multi-row upscaling
Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
dst_ptr += dst_stride;
for (y = 0; y < src_height - 1; ++y) {
  Scale2RowUp(src_ptr, src_stride, dst_ptr, dst_stride, dst_width);
  src_ptr += src_stride;
  dst_ptr += 2 * dst_stride;
}

// Good: pass literal 0 directly for src_stride on the final odd row
if (src_height & 1) {
  if (!filtering) {
    src_ptr -= src_stride;  // Point to last row.
  }
  ScaleRowDown2(src_ptr, 0, dst_ptr, dst_width, scale);
}
```
