---
name: libyuv/source/derive-full-range-yuv-to-rgb-matrix-coefficients
rubric: correctness
severity: should_fix
---

**Rule.** When defining full-range YUV to RGB conversion matrix coefficients
from luma weighting factors WR and WB (where WG = 1 - WR - WB), derive the
coefficients using exact colorimetric equations: R = Y + 2*(1 - WR)*V, B = Y +
2*(1 - WB)*U, and G = Y - (2*(WR*(1 - WR)*V + WB*(1 - WB)*U)) / (1 - WB - WR).
Round fixed-point matrix multiplier constants from these derived decimal values
rather than hand-estimating or approximating coefficients.

**Why.** Full-range YUV to RGB conversion matrices must match the analytical
inverse of standard colorimetric luma and color-difference definitions (such as
BT.709 or BT.2020), otherwise color conversion produces inaccurate RGB outputs
and color distortion.

## Examples

Bad:

```cpp
// BT.709 full range YUV to RGB reference (incorrectly approximated)
//  R = Y                - V * -1.28033
//  G = Y - U *  0.21482 - V *  0.38059
//  B = Y - U * -2.12798
#define UB -128
#define UG 14
#define VG 24
#define VR -82
```

Good:

```cpp
// BT.709 full range YUV to RGB reference
//  R = Y + (2 * (1 - WR)) * V
//  G = Y - ((2 * ((WR * (1 - WR) * V) + (WB * (1 - WB) * U))) / (1 - WB - WR))
//  B = Y + (2 * (1 - WB)) * U
//  Given WR = 0.2126, WB = 0.0722:
//  R = Y - V * -1.5748
//  G = Y - U * 0.18732 - V * 0.46812
//  B = Y - U * -1.8556
#define UB -119 /* round(-1.8556 * 64) */
#define UG 14   /* round(0.18732 * 64) */
#define VG 30   /* round(0.46812 * 64) */
#define VR -101 /* round(-1.5748 * 64) */
```
