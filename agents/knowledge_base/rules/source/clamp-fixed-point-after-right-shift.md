---
name: libyuv/source/clamp-fixed-point-after-right-shift
description: When scaling, interpolating, or converting pixel values using fixed-point arithmetic, apply range clamping helpers like clamp255 after downshifting, not before
rubric: correctness
severity: blocking
---

**Rule.** When scaling, interpolating, dithering, attenuating, or converting
pixel values using fixed-point arithmetic, apply range clamping helpers (such as
`clamp255`, `clamp1023`, `Clamp`, `ClampMax`, or macros like `C16TO8`) to the
final value after all right shifts (downshifts) are completed. Ensure the
clamping helper or macro encloses the entire shifted expression rather than
clamping the unshifted intermediate product before shifting.

**Why.** In fixed-point arithmetic, intermediate products and interpolated
values carry fractional and integer precision in higher bits (e.g., Q16 or
scaled integer formats). Clamping an unshifted intermediate value before
downshifting (such as `clamp255(val * scale) >> 16`) prematurely truncates the
high-precision product to the target range [0, 255] or [0, 1023], causing
subsequent right shifts by 8 or 16 bits to collapse valid non-zero pixel outputs
to zero.

## Examples

Bad:

```cpp
// Bad: Clamping before right-shifting truncates the product to 255, collapsing output to 0.
dst_uv[x] = clamp255(((src_uv[x] + src_uv[src_uv_stride + x] + 1) >> 1) * scale) >> 16;

// Bad: Clamping interpolated fixed-point product before downshifting.
int interpolated = (src_ptr[0] * y0_fraction + src_ptr1[0] * y1_fraction) >> 8;
dst_ptr[0] = STATIC_CAST(uint8_t, clamp255(interpolated * scale) >> 16);
```

Good:

```cpp
// Good: Right-shift the fixed-point product to target depth before clamping.
dst_uv[x] = clamp255((((src_uv[x] + src_uv[src_uv_stride + x] + 1) >> 1) * scale) >> 16);

// Good: Complete the downshift inside the clamping helper or helper macro.
int interpolated = (src_ptr[0] * y0_fraction + src_ptr1[0] * y1_fraction) >> 8;
dst_ptr[0] = STATIC_CAST(uint8_t, clamp255((interpolated * scale) >> 16));

// Or encapsulate using a macro like C16TO8:
dst_ptr[0] = STATIC_CAST(uint8_t, C16TO8(interpolated, scale));
```
