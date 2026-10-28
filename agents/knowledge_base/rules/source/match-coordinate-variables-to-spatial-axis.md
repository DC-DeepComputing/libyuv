---
name: libyuv/source/match-coordinate-variables-to-spatial-axis
description: When implementing 2D image coordinate indexing or scaling loops, use y and dy for vertical row coordinates and x and dx for horizontal column coordinates
rubric: readability
severity: nit
---

**Rule.** Name 2D coordinate variables and step sizes consistently with their
spatial axis: use y and dy (or row indices) for vertical coordinates, height
iterations, and stride offsets, and use x and dx (or column indices) for
horizontal coordinates, width iterations, and column offsets. Avoid using
horizontal coordinate names (such as x or dx) or generic names (such as step)
when advancing vertical row positions.

**Why.** Using horizontal coordinate identifiers (such as x or dx) or ambiguous
names for vertical row stepping and stride-based row indexing is confusing and
risks transposition bugs between height and width operations.

## Examples

Bad:

```cpp
x = (1u << 31);
for (i = 0; i < dst_height; ++i) {
  ScaleRowUp(src_ptr + (x >> 32) * src_stride, dst_ptr, dst_width);
  x += step;
}
```

Good:

```cpp
dy = FixedDiv(src_height - 1, dst_height - 1);
y = (1 << 15) - 1;
for (i = 0; i < dst_height; ++i) {
  ScaleRowUp(src_ptr + (y >> 16) * src_stride, dst_ptr, dst_width);
  y += dy;
}
```
