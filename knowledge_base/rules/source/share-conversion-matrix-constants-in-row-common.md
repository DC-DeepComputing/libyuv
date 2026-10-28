---
name: libyuv/source/share-conversion-matrix-constants-in-row-common
description: When implementing color conversion SIMD kernels and C fallbacks, share centralized matrix structures in row_common.cc rather than defining duplicate private constants in architecture-specific row files
rubric: design
severity: should_fix
---

**Rule.** When implementing or updating color conversion kernels across SIMD and
C backends, share centralized matrix coefficient structures defined in
row_common.cc (such as ArgbConstants and YuvConstants) rather than defining
duplicate private constant structures or tables in architecture-specific row
files. Pass the centralized constant pointer to matrix row kernels to ensure
identical coefficients and bit-exact outputs across all platforms.

**Why.** Centralizing matrix constants in row_common.cc eliminates duplicate
coefficient tables across architecture files (such as row_neon.cc, row_gcc.cc,
row_rvv.cc), ensures consistent naming across color spaces (such as BT.601,
BT.709, BT.2020, and JPEG), and prevents rounding or precision divergence
between SIMD kernels and portable C reference fallbacks.

## Examples

Bad:

```cpp
// Private duplicate matrix constant structure defined in architecture row file
struct RgbConstants {
  uint8_t kRGBToY[4];
  uint16_t kAddY;
};
static const struct RgbConstants kRgb24JPEGConstants = {{29, 150, 77, 0}, 0x0080};

void RGB24ToYJRow_NEON(const uint8_t* src_rgb24, uint8_t* dst_yj, int width) {
  RGBToYMatrixRow_NEON(src_rgb24, dst_yj, width, &kRgb24JPEGConstants);
}
```

Good:

```cpp
// Reuse centralized matrix constants defined in row_common.cc / row.h
void RGB24ToYJRow_NEON(const uint8_t* src_rgb24, uint8_t* dst_yj, int width) {
  RGBToYMatrixRow_NEON(src_rgb24, dst_yj, width, &kArgbJPEGConstants);
}
```
