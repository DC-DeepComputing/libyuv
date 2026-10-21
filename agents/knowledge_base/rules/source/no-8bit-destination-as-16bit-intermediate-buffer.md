---
name: libyuv/source/no-8bit-destination-as-16bit-intermediate-buffer
rubric: safety
severity: blocking
---

**Rule.** When implementing planar format conversions (such as 16-bit to 8-bit
conversions with subsampling), do not cast or reuse 8-bit destination plane
buffers (e.g., dst_y, dst_u, dst_v) as temporary storage for 16-bit intermediate
operations. Process rows directly using dedicated 16-to-8-bit row converters
(such as HalfWidthRow_16To8) or allocate a properly sized temporary buffer.

**Why.** Caller-provided destination buffers for 8-bit planes may be sized
tightly to the exact output dimensions. Subsampling odd or small dimensions
(e.g., SUBSAMPLE(1, 1, 1) == 1) requires 2 bytes per pixel for 16-bit
intermediate operations but only 1 byte is allocated for an 8-bit plane, causing
an out-of-bounds write buffer overflow.

## Examples

Bad:

```cpp
const int uv_width = SUBSAMPLE(width, 1, 1);
const int uv_stride = uv_width;
const int uv_height = SUBSAMPLE(height, 1, 1);
// Unsafe: dst_y may only be 1 byte for a 1x1 image, but ScalePlane_12 writes 2 bytes per pixel.
ScalePlane_12(src_u, src_stride_u, width, height, (uint16_t*)dst_y,
              uv_stride, uv_width, uv_height, kFilterBilinear);
Convert16To8Plane((uint16_t*)dst_y, uv_stride, dst_u, dst_stride_u, scale,
                  uv_width, uv_height);
```

Good:

```cpp
// Safe: Process rows directly without borrowing 8-bit destination buffers as 16-bit storage.
void (*HalfWidthRow_16To8)(const uint16_t* src_uv, ptrdiff_t src_uv_stride,
                           uint8_t* dst_uv, int scale, int width) =
    (width & 1) ? HalfWidthRow_16To8_Odd_C : HalfWidthRow_16To8_C;
HalfWidthRow_16To8(src_u, src_stride_u, dst_u, scale, width);
```
