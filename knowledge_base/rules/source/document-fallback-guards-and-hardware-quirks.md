---
name: libyuv/source/document-fallback-guards-and-hardware-quirks
description: When adding architecture- or parameter-specific fallback guards to bypass SIMD kernels, clearly document the hardware quirk and failure mechanism in comments
rubric: readability
severity: nit
---

**Rule.** When adding conditional guards that bypass SIMD kernels for scalar
fallback paths due to hardware quirks or edge cases (such as denormal inputs,
saturation limits, or alignment restrictions), explicitly document the exact
failure mechanism and the input conditions causing it. Avoid terse or ambiguous
caveat comments that can be misread as describing the behavior of the enabled
branch rather than the failure mode being avoided.

**Why.** Terse comments above conditional guards can confuse maintainers about
whether the comment describes the guarded branch's behavior or the problem being
prevented. Thoroughly documenting hardware-specific quirks (such as ARM NEON
flushing denormal inputs to zero during multiplication) ensures future readers
understand why a scalar fallback is required and prevents unintended removal of
necessary guards.

## Examples

Bad:

```cpp
// Caveat: Scaling value becomes denormal and flushed to zero
if (has_neon && scale >= 1.0f / 4096.0f) {
  ScaleRowDown2_NEON(src_ptr, src_stride, dst_ptr, dst_width);
} else {
  ScaleRowDown2_C(src_ptr, src_stride, dst_ptr, dst_width);
}
```

Good:

```cpp
// When scale is 1/65535, the scale * 2^-112 factor is a denormal.
// While the scalar C version preserves denormals, ARM NEON vmul
// flushes denormal inputs to zero even when the result would be normal.
// Fall back to scalar C when scale is below 1/4096.
if (has_neon && scale >= 1.0f / 4096.0f) {
  ScaleRowDown2_NEON(src_ptr, src_stride, dst_ptr, dst_width);
} else {
  ScaleRowDown2_C(src_ptr, src_stride, dst_ptr, dst_width);
}
```
