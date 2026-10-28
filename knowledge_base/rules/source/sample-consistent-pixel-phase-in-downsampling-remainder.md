---
name: libyuv/source/sample-consistent-pixel-phase-in-downsampling-remainder
description: When implementing downsampling row kernels with remainder handling (e.g., dst_width & 1), sample the same pixel phase as the main unrolled loop
rubric: correctness
severity: should_fix
---

**Rule.** In downsampling row kernels (such as ScaleRowDown2*_C,
ScaleRowDown4*_C, or odd-dimension variants like ScaleRowDown2*_Odd_C), ensure
any remainder step (such as if (dst_width & 1)) samples the identical relative
pixel offset (e.g., src_ptr[1] for 2x point downsampling) used in the main
unrolled loop. Do not reset remainder sampling to src_ptr[0] unless handling a
single trailing edge boundary pixel.

**Why.** In point downsampling, unrolled loops advance by multi-pixel steps and
sample a consistent pixel phase within each block (such as the second pixel
src_ptr[1]). Inadvertently sampling src_ptr[0] in remainder handling causes an
off-by-one pixel phase shift and visual distortion on odd-width image rows.

## Examples

Bad:

```cpp
for (int x = 0; x < dst_width - 1; x += 2) {
  dst[0] = src_ptr[1];
  dst[1] = src_ptr[3];
  dst += 2;
  src_ptr += 4;
}
if (dst_width & 1) {
  dst[0] = src_ptr[0];  // Wrong phase: samples index 0 instead of index 1
  dst += 1;
  src_ptr += 2;
}
```

Good:

```cpp
for (int x = 0; x < dst_width - 1; x += 2) {
  dst[0] = src_ptr[1];
  dst[1] = src_ptr[3];
  dst += 2;
  src_ptr += 4;
}
if (dst_width & 1) {
  dst[0] = src_ptr[1];  // Consistent phase: matches main unrolled loop
  dst += 1;
  src_ptr += 2;
}
```
