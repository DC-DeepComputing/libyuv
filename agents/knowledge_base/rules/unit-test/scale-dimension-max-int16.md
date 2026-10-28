---
name: libyuv/unit-test/scale-dimension-max-int16
description: When testing or implementing scaling routines using 16.16 fixed-point coordinates, keep positive source dimensions at or below INT16_MAX (32767) to prevent integer coordinate overflow
rubric: correctness
severity: nit
---

**Rule.** When invoking scaling routines (such as `ARGBScale`, `I420Scale`,
`ScalePlane`, or `ScaleUV`) or writing scaling unit tests using 16.16
fixed-point arithmetic, do not use source dimensions exceeding 32767
(`INT16_MAX`). Values of 32768 or greater overflow the 16-bit signed integer
portion of 16.16 coordinates.

**Why.** libyuv scaling routines use 16.16 signed fixed-point coordinates (`x`,
`y`, `dx`, `dy`) where the integer component is limited to a signed 16-bit
range. Positive coordinates at 32768 or above overflow into negative numbers
(`32768 << 16` becomes `0x80000000` / `-2147483648`), causing out-of-bounds
sampling or coordinate distortion.

## Examples

Bad:

```cpp
// Testing maximum height with 32768 overflows 16.16 signed fixed point.
const int src_w = 1280;
const int src_h = 32768;
ARGBScale(orig_pixels, src_w * 4, src_w, src_h, dest_pixels, dst_w * 4, dst_w, dst_h, kFilterBilinear);
```

Good:

```cpp
// 32767 (INT16_MAX) is the maximum positive dimension representable in 16.16 fixed point.
const int src_w = 1280;
const int src_h = 32767;
ARGBScale(orig_pixels, src_w * 4, src_w, src_h, dest_pixels, dst_w * 4, dst_w, dst_h, kFilterBilinear);
```
