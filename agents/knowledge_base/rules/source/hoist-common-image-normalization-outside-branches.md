---
name: libyuv/source/hoist-common-image-normalization-outside-branches
description: Hoist shared image parameter setup and normalization (such as negative-height inversion and stride flipping) before branching on format, channel, or optional variants
rubric: design
severity: should_fix
---

**Rule.** Hoist shared image parameter setup and input normalization logic—such
as negative-height inversion (`height < 0`), pointer adjustments, and stride
flipping—outside conditional branches rather than duplicating them across
format, channel, or optional parameter paths (such as `src_a == NULL`).

**Why.** Extracting common preprocessing and parameter normalization to function
entry eliminates duplicated code blocks, reduces code bloat and maintenance
overhead, ensures consistent state, and keeps branch bodies focused on
variant-specific logic.

## Examples

Bad:

```cpp
if (src_a == NULL) {
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_argb = dst_argb + (height - 1) * dst_stride_argb;
    dst_stride_argb = -dst_stride_argb;
  }
  // Process opaque RGB...
} else {
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_argb = dst_argb + (height - 1) * dst_stride_argb;
    dst_stride_argb = -dst_stride_argb;
  }
  // Process ARGB with alpha...
}
```

Good:

```cpp
// Negative height means invert the image.
if (height < 0) {
  height = -height;
  dst_argb = dst_argb + (height - 1) * dst_stride_argb;
  dst_stride_argb = -dst_stride_argb;
}

if (src_a == NULL) {
  // Process opaque RGB...
} else {
  // Process ARGB with alpha...
}
```
