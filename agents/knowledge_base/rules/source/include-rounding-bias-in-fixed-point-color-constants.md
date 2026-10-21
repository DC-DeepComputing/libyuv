---
name: libyuv/source/include-rounding-bias-in-fixed-point-color-constants
rubric: correctness
severity: should_fix
---

**Rule.** When defining fixed-point color space conversion constants or SIMD
matrix structs (such as RgbConstants or ArgbConstants for RGB-to-Y/YUV
conversions), ensure additive bias values include both the nominal DC offset and
the half-LSB rounding term (e.g., 16.5 * 256 = 0x1080 for 8-bit BT.601 Y, and
0.5 * 256 = 0x0080 for full-range JPEG Y). Do not omit the +0.5 rounding term
(e.g., using 0x1000 instead of 0x1080), which causes truncation bias and
divergence from reference C implementations.

**Why.** Fixed-point color conversion formulas right-shift intermediate products
(e.g., >> 8) to obtain the final 8-bit channel value. Including the half-LSB
rounding bias (+128 or +0.5 * 2^N) ensures mathematically correct
round-to-nearest integer results and prevents truncation bias and unit test
mismatches between SIMD kernels and C reference fallbacks.

## Examples

Bad:

```cpp
// Omits half-LSB rounding bias (uses 16 * 256 = 0x1000 instead of 16.5 * 256):
static const struct RgbConstants kRgb24I601Constants = {{25, 129, 66, 0},
                                                        0x1000};
```

Good:

```cpp
// Includes nominal DC offset (16) plus half-LSB rounding bias (+0.5): 16.5 * 256 = 0x1080
static const struct RgbConstants kRgb24I601Constants = {{25, 129, 66, 0},
                                                        0x1080};
```
