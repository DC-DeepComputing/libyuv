---
name: libyuv/source/clamp-vertical-scaling-coordinate-to-max-y
description: When iterating rows in fixed-point image scaling loops, clamp vertical coordinates (y, y64) against max_y at the loop top before calculating row indices or source buffer offsets
rubric: safety
severity: should_fix
---

**Rule.** In fixed-point image scaling loops, clamp the vertical row coordinate
(y or y64) against max_y = (src_height - 1) << 16 (or ((int64_t)src_height - 1)
<< 16) at the very top of each destination row loop iteration before computing
integer row indices (e.g., yi = (int)(y64 >> 16)), calculating source buffer row
offsets, or calling row scalers (such as ScaleARGBCols, ScaleARGBFilterCols, or
ScalePlaneCols). Do not defer coordinate clamping inside conditional
row-advancement branches (such as if (yi != lasty)), which can skip clamping
when the condition is false or cause out-of-bounds reads.

**Why.** Stepping vertically with fixed-point increments (dy) across destination
rows can accumulate past the last valid source row on edge iterations due to
step calculation rounding or resolution disparities. Clamping y or y64 to max_y
at the top of the destination row loop guarantees the coordinate bounds
invariant (y <= max_y) holds unconditionally before integer truncation or source
pointer arithmetic, preventing out-of-bounds memory reads and redundant nested
checks.

## Examples

Bad:

```cpp
// Variant 1: Unclamped vertical coordinate when computing row offsets
for (j = 0; j < dst_height; ++j) {
  ScaleARGBCols(dst_argb, src_argb + (y64 >> 16) * src_stride, dst_width, x, dx);
  dst_argb += dst_stride;
  y64 += dy;
}

// Variant 2: Clamping deferred inside conditional row-advancement branch
for (j = 0; j < dst_height; ++j) {
  yi = (int)(y64 >> 16);
  if (yi != lasty) {
    if (y64 > max_y) {
      y64 = max_y;
      yi = (int)(y64 >> 16);
    }
    ScaleARGBFilterCols(rowptr, src_argb + yi * src_stride, dst_width, x, dx);
  }
}
```

Good:

```cpp
// Variant 1: Clamp y64 against max_y at loop top before computing row offset
for (j = 0; j < dst_height; ++j) {
  if (y64 > max_y) {
    y64 = max_y;
  }
  ScaleARGBCols(dst_argb, src_argb + (y64 >> 16) * src_stride, dst_width, x, dx);
  dst_argb += dst_stride;
  y64 += dy;
}

// Variant 2: Clamp at loop top before computing integer index and conditional branch
for (j = 0; j < dst_height; ++j) {
  if (y64 > max_y) {
    y64 = max_y;
  }
  yi = (int)(y64 >> 16);
  if (yi != lasty) {
    ScaleARGBFilterCols(rowptr, src_argb + yi * src_stride, dst_width, x, dx);
  }
}
```
