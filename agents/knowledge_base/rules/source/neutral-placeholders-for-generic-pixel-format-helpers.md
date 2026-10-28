---
name: libyuv/source/neutral-placeholders-for-generic-pixel-format-helpers
description: When naming generic multi-channel pixel format conversion helpers and parameters, use neutral placeholders like XYZ or WXYZ instead of ABC to avoid channel name collisions
rubric: readability
severity: nit
---

**Rule.** When naming generic multi-channel pixel format conversion helpers,
shuffle kernels, or buffer parameters that support arbitrary channel
permutations, use neutral placeholder letters such as 'XYZ' (for 3-channel
formats) or 'WXYZ' (for 4-channel formats) rather than 'ABC' or 'ABCD' (e.g.,
ARGBToXYZRow_SVE2 or dst_xyz). Do not use 'A', 'B', or 'C' as generic
multi-channel placeholders.

**Why.** In pixel format naming conventions, letters like 'A', 'B', and 'C' have
established channel meanings (Alpha, Blue, and Chroma/Color). Using them as
generic multi-channel placeholders creates ambiguity with concrete format names,
whereas neutral placeholders like 'XYZ' and 'WXYZ' clearly signal a generic
format conversion or shuffle kernel.

## Examples

Bad:

```cpp
// Ambiguous placeholder 'ABC' collides with Alpha, Blue, and Chroma channels.
static inline void ARGBToABCRow_SVE2(const uint8_t* src_argb,
                                     uint8_t* dst_abc,
                                     int width,
                                     const uint8_t* indices) {
  // Generic 3-channel shuffle implementation
}
```

Good:

```cpp
// Neutral placeholder 'XYZ' clearly indicates a generic 3-channel format.
static inline void ARGBToXYZRow_SVE2(const uint8_t* src_argb,
                                     uint8_t* dst_xyz,
                                     int width,
                                     const uint8_t* indices) {
  // Generic 3-channel shuffle implementation
}
```
