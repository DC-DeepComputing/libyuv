---
name: libyuv/unit_test/parametrize-color-conversion-test-references
description: When writing color conversion unit tests, derive reference transformation parameters dynamically from standard coefficients (Kr, Kb) rather than hardcoding precomputed floating-point constants
rubric: design
severity: should_fix
---

**Rule.** In color conversion unit tests, derive reference transformation
parameters parametrically from standard color space constants (e.g., Kr and Kb
per ITU-T Rec. H.273), bit depths, and range rather than hardcoding precomputed
floating-point coefficients across duplicate reference functions. Document the
specification source or equations used in comments.

**Why.** Hardcoding precomputed floating-point coefficients across separate
reference functions obscures their mathematical derivation, creates duplicate
reference logic, and makes precision differences across bit depths and ranges
difficult to verify and maintain.

## Examples

Bad:

```cpp
// BT.601 10-bit limited range YUV to RGB reference
static void YUV10ToRGBReference(int y, int u, int v, int* r, int* g, int* b) {
  *r = RoundToByte((y - 64) * 0.29109589041095890411 -
                   (v - 512) * -0.39900669642857142857);
  *g = RoundToByte((y - 64) * 0.29109589041095890411 -
                   (u - 512) * 0.09794057252372840107 -
                   (v - 512) * 0.20324191180944268679);
  *b = RoundToByte((y - 64) * 0.29109589041095890411 -
                   (u - 512) * -0.50430803571428571429);
}
```

Good:

```cpp
// Equations and constants from ITU-T Rec. H.273
YUVToRGBReferenceParameter CalcYUVToRGBReferenceParameter(
    double kr, double kb, int yuv_depth, int rgb_depth, bool yuv_full) {
  YUVToRGBReferenceParameter p;
  double kg = 1.0 - kr - kb;
  p.ub = 2.0 - 2.0 * kb;
  p.vr = 2.0 - 2.0 * kr;
  p.ug = -2.0 * (1.0 - kb) * kb / kg;
  p.vg = -2.0 * (1.0 - kr) * kr / kg;
  // Compute scaling factors from bit depths and range.
  return p;
}
```
