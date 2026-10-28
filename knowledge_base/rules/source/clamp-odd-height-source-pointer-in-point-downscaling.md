---
name: libyuv/source/clamp-odd-height-source-pointer-in-point-downscaling
description: When implementing 2:1 plane downscaling with point filtering, adjust the source pointer and zero src_stride for trailing odd rows to prevent out-of-bounds buffer reads
rubric: correctness
severity: should_fix
---

**Rule.** In 2:1 plane downscaling routines using point filtering (nearest
neighbor) where src_ptr is offset to sample odd rows, adjust src_ptr back by
src_stride and set src_stride to 0 when processing trailing rows on odd source
heights. Do not pass the post-loop src_ptr directly to row downscalers without
compensating for the two-row iteration step overshooting the buffer boundary.

**Why.** Point downscaling initializes src_ptr to row 1 and advances by 2 *
src_stride per step. For odd source heights (such as 3 rows), the loop handles
row 1 and advances src_ptr to row 3 (exceeding valid row index 2), causing
trailing odd-row handlers to perform out-of-bounds reads unless src_ptr is
adjusted back to the last valid row.

## Examples

Bad:

```cpp
// src_ptr was offset to row 1 for point filtering, and advanced by 2 * src_stride per step.
for (y = 0; y < src_height / 2; ++y) {
  ScaleRowDown2(src_ptr, src_stride, dst_ptr, dst_width);
  src_ptr += row_stride;
  dst_ptr += dst_stride;
}
if (src_height & 1) {
  // Bug: src_ptr points past the last valid row (e.g., row 3 for height 3).
  ScaleRowDown2(src_ptr, src_stride, dst_ptr, dst_width);
}
```

Good:

```cpp
for (y = 0; y < src_height / 2; ++y) {
  ScaleRowDown2(src_ptr, src_stride, dst_ptr, dst_width);
  src_ptr += row_stride;
  dst_ptr += dst_stride;
}
if (src_height & 1) {
  if (!filtering) {
    src_ptr -= src_stride;  // Adjust back to the last valid row.
  }
  src_stride = 0;
  ScaleRowDown2(src_ptr, src_stride, dst_ptr, dst_width);
}
```
