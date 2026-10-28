---
name: libyuv/source/support-arbitrary-heights-for-kfilterlinear
description: When implementing or dispatching fast paths for kFilterLinear scaling, support arbitrary destination heights rather than restricting the path to equal heights
rubric: correctness
severity: should_fix
---

**Rule.** When implementing or dispatching fast paths for kFilterLinear scaling,
support arbitrary destination heights (dst_height != src_height) via vertical
point/nearest sampling rather than requiring dst_height == src_height. Step
source rows by stride according to vertical scaling ratios instead of
restricting the fast path to equal-height transforms.

**Why.** In libyuv, kFilterLinear specifies filtering along the horizontal axis
only with nearest-neighbor point sampling vertically. Restricting horizontal
linear fast paths to equal heights prevents scaling pipelines from using
optimized row kernels for general scaling dimensions.

## Examples

Bad:

```cpp
// Restricting linear horizontal scaling to identical source and destination heights.
if (dst_height == src_height && (dst_width + 1) / 2 == src_width &&
    filtering == kFilterLinear) {
  ScalePlaneColUp2_Linear(src_width, src_height, dst_width, dst_height,
                          src_stride, dst_stride, src, dst);
  return;
}
```

Good:

```cpp
// Supporting arbitrary destination heights using vertical point sampling.
if ((dst_width + 1) / 2 == src_width && filtering == kFilterLinear) {
  ScalePlaneUp2_Linear(src_width, src_height, dst_width, dst_height,
                       src_stride, dst_stride, src, dst);
  return;
}
```
