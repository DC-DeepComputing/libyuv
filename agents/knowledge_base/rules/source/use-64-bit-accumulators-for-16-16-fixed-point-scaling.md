---
name: libyuv/source/use-64-bit-accumulators-for-16-16-fixed-point-scaling
description: When accumulating 16.16 fixed-point coordinates in scaling loops, promote upper bounds and accumulators (max_y, y64) to int64_t to prevent signed integer overflow on large dimensions
rubric: correctness
severity: should_fix
---

**Rule.** In 16.16 fixed-point image scaling loops, declare coordinate upper
bounds (max_y) and vertical accumulators (y64) as int64_t (e.g.,
(int64_t)(src_height - 1) << 16). Accumulate row step increments (dy) using the
64-bit variable before clamping against max_y and shifting to derive row indices
(yi = (int)(y64 >> 16)).

**Why.** Left-shifting image dimensions of 32768 or greater by 16 bits
((src_height - 1) << 16) or repeatedly adding fixed-point step sizes (dy) into a
32-bit signed integer causes signed integer overflow and undefined behavior.
Promoting accumulators and bounds to int64_t ensures coordinate comparisons and
clamping behave correctly for arbitrarily large image heights.

## Examples

Bad:

```cpp
const int max_y = (src_height - 1) << 16;
int y = 0;
for (int j = 0; j < dst_height; ++j) {
  if (y > max_y) {
    y = max_y;
  }
  int yi = y >> 16;
  ScaleRow(src_ptr + yi * src_stride, dst_ptr, dst_width);
  dst_ptr += dst_stride;
  y += dy;
}
```

Good:

```cpp
const int64_t max_y = (int64_t)(src_height - 1) << 16;
int64_t y64 = y;
for (int j = 0; j < dst_height; ++j) {
  if (y64 > max_y) {
    y64 = max_y;
  }
  int yi = (int)(y64 >> 16);
  ScaleRow(src_ptr + yi * src_stride, dst_ptr, dst_width);
  dst_ptr += dst_stride;
  y64 += dy;
}
```
