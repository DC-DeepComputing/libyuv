---
name: libyuv/source/rotate-interleaved-planes-over-split-transpose
description: When rotating biplanar or interleaved UV formats, rotate interleaved planes directly using plane rotation helpers instead of introducing combined split-and-transpose functions
rubric: design
severity: nit
---

**Rule.** When rotating biplanar or interleaved UV formats (such as NV12, P010,
or P210), rotate interleaved UV planes directly using plane rotation helpers
(such as RotatePlane or RotatePlane_16) rather than introducing combined UV
split-and-transpose or split-and-rotate functions. Keep geometric rotation
decoupled from channel splitting, unpacking, and bit-depth conversion.

**Why.** Combined split-and-rotate functions (like NV12ToI420Rotate /
SplitTransposeUV) are legacy patterns being phased out in favor of rotating
interleaved channels as unified planes (e.g., treating interleaved UV pairs as
16-bit or 32-bit elements). Decoupling rotation from channel splitting avoids
proliferating specialized transposition helpers and simplifies handling
high-bit-depth formats (such as P010/P210) that require format-specific bit
shifting during plane separation.

## Examples

Bad:

```cpp
// Bad: Introducing combined UV split-and-transpose helper for 16-bit data.
void TransposeUVWxH_16_C(const uint8_t* src,
                         int src_stride,
                         uint16_t* dst_u,
                         int dst_stride_u,
                         uint16_t* dst_v,
                         int dst_stride_v,
                         int width,
                         int height);
```

Good:

```cpp
// Good: Rotate interleaved UV planes directly as unified 16-bit elements.
int RotatePlane_16(const uint16_t* src,
                   int src_stride,
                   uint16_t* dst,
                   int dst_stride,
                   int width,
                   int height,
                   enum RotationMode mode);
```
