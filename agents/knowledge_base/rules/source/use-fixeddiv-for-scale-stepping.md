---
name: libyuv/source/use-fixeddiv-for-scale-stepping
description: When calculating step sizes or coordinate slopes for image scaling, use 16.16 fixed-point arithmetic with FixedDiv() instead of 64-bit integer division.
rubric: safety
severity: should_fix
---

**Rule.** In image scaling slope and step calculations, use 16.16 fixed-point
representation with 32-bit integers and the FixedDiv() / FixedDiv1() helpers
rather than ad-hoc 64-bit integer division or 32.32 stepping.

**Why.** Libyuv standardizes scaling stepping on 16.16 fixed point. Using 32-bit
fixed-point arithmetic provides sufficient precision for image dimensions,
avoids expensive 64-bit divisions on 32-bit CPU architectures, and leverages
assembly-optimized implementations of FixedDiv.

## Examples

Bad:

```cpp
// Bad: Using 64-bit division and 32.32 fixed-point stepping.
step = ((uint64_t)(src_height - 1) << 32) / (dst_height - 1);
x = (1u << 31);
for (i = 0; i < dst_height; ++i) {
  ScaleRowUp(src_ptr + (x >> 32) * src_stride, dst_ptr, dst_width);
  dst_ptr += dst_stride;
  x += step;
}
```

Good:

```cpp
// Good: Using 16.16 fixed-point stepping with FixedDiv.
dy = FixedDiv(src_height - 1, dst_height - 1);
y = (1 << 15) - 1;
for (i = 0; i < dst_height; ++i) {
  ScaleRowUp(src_ptr + (y >> 16) * src_stride, dst_ptr, dst_width);
  dst_ptr += dst_stride;
  y += dy;
}
```
