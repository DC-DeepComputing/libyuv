---
name: libyuv/unit_test/test-full-container-range-for-high-bit-depth
description: When unit testing high-bit-depth SIMD vs C kernels in uint16 buffers, initialize inputs across the full container range instead of masking to nominal bit depth
rubric: testing
severity: should_fix
---

**Rule.** When testing high-bit-depth image formats (such as 10-bit or 12-bit
stored in uint16_t buffers) comparing SIMD routines against C reference
implementations, initialize test input buffers spanning the full range of the
underlying container type (e.g., fastrand() & 65535 for uint16_t) rather than
masking strictly to the nominal bit depth (e.g., (1 << DEPTH) - 1). Verify that
out-of-range, saturated, and boundary inputs produce identical clamping,
saturation, and overflow behavior across C reference code and all SIMD
architectures.

**Why.** Input buffers in production may carry arbitrary unmasked upper bits or
out-of-range values outside the nominal precision. Masking test inputs to the
nominal bit depth fails to test out-of-range clamping and saturation behavior,
hiding divergences between C reference code and SIMD instructions (such as ARM
NEON vs. x86 packing/narrowing). Exercising the full integer container range
ensures all architectures and fallback implementations handle edge cases,
unmasked bits, and overflow identically.

## Examples

Bad:

```cpp
// Masking 16-bit inputs to nominal 10-bit or 12-bit depth misses out-of-range clamping bugs
for (int i = 0; i < kPixels; ++i) {
  src_pixels_r[i] = fastrand() & ((1 << DEPTH) - 1);
  src_pixels_g[i] = fastrand() & ((1 << DEPTH) - 1);
  src_pixels_b[i] = fastrand() & ((1 << DEPTH) - 1);
  src_pixels_a[i] = fastrand() & ((1 << DEPTH) - 1);
}
```

Good:

```cpp
// Spanning the full 16-bit container range tests saturation parity between SIMD and C
for (int i = 0; i < kPixels; ++i) {
  src_pixels_r[i] = fastrand() & 65535;
  src_pixels_g[i] = fastrand() & 65535;
  src_pixels_b[i] = fastrand() & 65535;
  src_pixels_a[i] = fastrand() & 65535;
}
```
