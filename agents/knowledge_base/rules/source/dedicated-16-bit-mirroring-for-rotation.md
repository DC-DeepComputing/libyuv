---
name: libyuv/source/dedicated-16-bit-mirroring-for-rotation
description: When implementing 180-degree rotation or horizontal mirroring for 16-bit planar formats, use dedicated 16-bit functions (RotatePlane180_16, MirrorRow_16) rather than 8-bit functions with double width
rubric: correctness
severity: blocking
---

**Rule.** Do not implement 180-degree rotation or horizontal mirroring for
16-bit plane formats (such as I010, I210, I410, or P010) by calling 8-bit mirror
or rotation functions with double width (`width * 2`). Use dedicated 16-bit
functions (e.g., `RotatePlane180_16` or `MirrorRow_16`) to reverse whole 16-bit
pixel elements rather than individual bytes.

**Why.** 8-bit mirroring functions (`MirrorRow`) reverse data byte-by-byte.
Reusing an 8-bit mirror or rotation routine on 16-bit pixel data with `width *
2` swaps the high and low bytes of each 16-bit pixel in addition to mirroring
the row, corrupting pixel values and endianness.

## Examples

Bad:

```cpp
case kRotate180:
  RotatePlane180(src, src_stride, dst, dst_stride, width * 2, height);
  return 0;
```

Good:

```cpp
case kRotate180:
  RotatePlane180_16(src, src_stride, dst, dst_stride, width, height);
  return 0;
```
