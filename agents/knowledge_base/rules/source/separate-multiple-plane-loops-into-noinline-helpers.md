---
name: libyuv/source/separate-multiple-plane-loops-into-noinline-helpers
description: When implementing plane conversion functions with alternative branching loops, separate multiple row loops into dedicated static LIBYUV_NOINLINE helper functions
rubric: safety
severity: nit
---

**Rule.** Avoid writing multiple distinct row-processing loops inside a single
plane processing function. Separate alternative branches (such as alpha versus
opaque paths) into dedicated file-static helper functions marked with
LIBYUV_NOINLINE, or unify the row kernel signature so the outer function
contains only a single loop.

**Why.** Compilers such as Visual C++ often generate suboptimal code and
register allocations across functions containing multiple distinct loops.
Isolating each loop into a separate non-inlined helper ensures optimal compiler
loop optimization and clean register usage.

## Examples

Bad:

```cpp
void MergeARGB16To8Plane(...) {
  if (src_a == NULL) {
    for (y = 0; y < height; ++y) {
      MergeXRGB16To8Row(src_r, src_g, src_b, dst_argb, depth, width);
      // ...
    }
  } else {
    for (y = 0; y < height; ++y) {
      MergeARGB16To8Row(src_r, src_g, src_b, src_a, dst_argb, depth, width);
      // ...
    }
  }
}
```

Good:

```cpp
LIBYUV_NOINLINE
static void MergeARGB16To8PlaneOpaque(...) {
  for (y = 0; y < height; ++y) {
    MergeXRGB16To8Row(src_r, src_g, src_b, dst_argb, depth, width);
    // ...
  }
}

LIBYUV_NOINLINE
static void MergeARGB16To8PlaneAlpha(...) {
  for (y = 0; y < height; ++y) {
    MergeARGB16To8Row(src_r, src_g, src_b, src_a, dst_argb, depth, width);
    // ...
  }
}

void MergeARGB16To8Plane(...) {
  if (src_a == NULL) {
    MergeARGB16To8PlaneOpaque(...);
  } else {
    MergeARGB16To8PlaneAlpha(...);
  }
}
```
