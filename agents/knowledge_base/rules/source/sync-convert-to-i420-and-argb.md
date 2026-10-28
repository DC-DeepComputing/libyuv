---
name: libyuv/source/sync-convert-to-i420-and-argb
description: When modifying input sample parsing, plane offset calculation, or format decoding in ConvertToI420 or ConvertToARGB, apply corresponding updates to the counterpart dispatcher.
rubric: correctness
severity: should_fix
---

**Rule.** When updating input sample parsing, plane offset calculations,
negative height handling, or format decoding cases in ConvertToI420 or
ConvertToARGB, make the equivalent updates in the counterpart function. Keep
format support, cropping math, and stride adjustments synchronized across both
high-level conversion dispatchers.

**Why.** ConvertToI420 and ConvertToARGB are companion high-level format
dispatchers that parse packed, planar, and biplanar sample formats into I420 and
ARGB. Inconsistencies between them lead to subtle bugs or format incompatibility
depending on whether callers convert to I420 or ARGB.

## Examples

Bad:

```cpp
// In source/convert_to_i420.cc alone (omitted from source/convert_to_argb.cc):
// Handling negative height for NV12 UV plane offset:
src_uv = sample + aligned_src_width * (abs_src_height + crop_y / 2) + crop_x;
```

Good:

```cpp
// Updated in both source/convert_to_i420.cc and source/convert_to_argb.cc:
// Handling negative height consistently across both dispatchers:
src_uv = sample + aligned_src_width * (abs_src_height + crop_y / 2) + crop_x;
```
