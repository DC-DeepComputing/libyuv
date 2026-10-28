---
name: libyuv/source/reject-negative-crop-dimensions
description: When validating crop dimensions in conversion APIs like ConvertToI420, reject non-positive crop_width or crop_height with -1 instead of silently normalizing negative heights
rubric: correctness
severity: nit
---

**Rule.** In high-level conversion and cropping APIs (such as ConvertToI420 and
ConvertToARGB), validate that crop dimensions (crop_width, crop_height) are
strictly positive and return an error (-1) if negative or zero. Do not silently
normalize or take the absolute value of negative crop dimensions; vertical
flipping must be controlled exclusively via negative src_height.

**Why.** In libyuv, image inversion and vertical flipping are specified
exclusively by passing a negative source height (src_height < 0). Silently
normalizing negative crop dimensions obscures invalid caller arguments and
creates ambiguity over which parameter controls vertical orientation.

## Examples

Bad:

```cpp
// Silently normalizing negative crop_height
if (crop_height < 0) {
  crop_height = -crop_height;
}
inv_crop_height = (src_height < 0) ? -crop_height : crop_height;
```

Good:

```cpp
// Treating negative or zero crop dimensions as invalid input
if (!dst_y || !dst_u || !dst_v || !sample || src_width <= 0 ||
    crop_width <= 0 || src_height == 0 || crop_height <= 0) {
  return -1;
}
inv_crop_height = (src_height < 0) ? -crop_height : crop_height;
```
