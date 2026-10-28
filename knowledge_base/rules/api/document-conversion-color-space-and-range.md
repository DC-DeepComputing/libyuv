---
name: libyuv/api/document-conversion-color-space-and-range
description: When documenting format conversion functions in public headers, consistently specify the color space and range across all sibling conversion variants
rubric: readability
severity: nit
---

**Rule.** In public conversion header doc comments (`include/libyuv/**`),
consistently document the color space and range (e.g., 'BT.709 full range',
'BT.601 limited', 'BT.2020') across all sibling destination and source
conversion variants (such as `*ToARGB`, `*ToABGR`, and `*ToRGB24`) for formats
tied to specific color matrices. Do not specify the color matrix and range on
one destination variant while leaving sister variants undocumented or ambiguous.

**Why.** Public conversion functions for formats tied to specific color spaces
and ranges (such as F420 or H420) are often consumed in sets across multiple
output formats (ARGB, ABGR, RAW). Inconsistent documentation across sister
functions creates ambiguity about whether different variants use identical color
conversion matrices and ranges.

## Examples

Bad:

```cpp
// Convert F420 to ARGB. BT.709 full range
LIBYUV_API
int F420ToARGB(const uint8_t* src_y, int src_stride_y,
               uint8_t* dst_argb, int dst_stride_argb,
               int width, int height);

// Convert F420 to ABGR.
LIBYUV_API
int F420ToABGR(const uint8_t* src_y, int src_stride_y,
               uint8_t* dst_abgr, int dst_stride_abgr,
               int width, int height);
```

Good:

```cpp
// Convert F420 to ARGB. BT.709 full range
LIBYUV_API
int F420ToARGB(const uint8_t* src_y, int src_stride_y,
               uint8_t* dst_argb, int dst_stride_argb,
               int width, int height);

// Convert F420 to ABGR. BT.709 full range
LIBYUV_API
int F420ToABGR(const uint8_t* src_y, int src_stride_y,
               uint8_t* dst_abgr, int dst_stride_abgr,
               int width, int height);
```
