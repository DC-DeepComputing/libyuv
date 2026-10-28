---
name: libyuv/source/avoid-redundant-single-row-branches-in-bilinear-upscalers
description: When implementing 2x bilinear plane or UV upscaling routines, omit redundant src_height == 1 branches because single-row inputs are dispatched to horizontal linear scalers
rubric: readability
severity: should_fix
---

**Rule.** In 2x bilinear upscaling routines (such as ScaleUVBilinearUp2 and
ScalePlaneUp2_Bilinear), do not include special-cased src_height == 1 branches.
Single-row inputs are already handled by horizontal-only linear scaling
dispatchers (such as ScaleUVLinearUp2 and ScalePlaneUp2_Linear), making 1-row
branches inside 2D bilinear upscalers redundant.

**Why.** Higher-level scaling dispatchers route single-height inputs directly to
horizontal linear scalers. Adding an explicit src_height == 1 branch inside 2D
bilinear upscalers introduces dead or unreachable fallback code and unnecessary
branching.

## Examples

Bad:

```cpp
if (src_height == 1) {
  Scale2RowUp(src_ptr, 0, dst_ptr, dst_stride, dst_width);
} else {
  Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
  dst_ptr += dst_stride;
  for (x = 0; x < src_height - 1; ++x) {
    Scale2RowUp(src_ptr, src_stride, dst_ptr, dst_stride, dst_width);
    src_ptr += src_stride;
    dst_ptr += 2 * dst_stride;
  }
  if (!(dst_height & 1)) {
    Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
  }
}
```

Good:

```cpp
Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
dst_ptr += dst_stride;
for (x = 0; x < src_height - 1; ++x) {
  Scale2RowUp(src_ptr, src_stride, dst_ptr, dst_stride, dst_width);
  src_ptr += src_stride;
  dst_ptr += 2 * dst_stride;
}
if (!(dst_height & 1)) {
  Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
}
```
