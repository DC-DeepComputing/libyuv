---
name: libyuv/api/group-conversion-matrix-constants-by-pixel-format
description: When declaring conversion matrix constants across pixel formats and color spaces in public headers, group constants into distinct commented sections by format variant
rubric: readability
severity: nit
---

**Rule.** When declaring conversion matrix constants (such as struct
ArgbConstants or struct YuvConstants) across multiple pixel formats and color
spaces in public headers, organize them into distinct, well-commented sections
grouped by primary pixel format variant (e.g., xRGB, xBGR, RGBx, BGRx, or YUV
vs. YVU) rather than interleaving color spaces in a single block. Precede each
group with a clear section comment specifying the input/output format and target
conversion.

**Why.** Grouping conversion matrix constants into dedicated, commented sections
by format layout allows library callers to quickly find the appropriate
constants for their memory configurations and prevents confusion across channel
permutations.

## Examples

Bad:

```cpp
// Conversion matrix for RGB to YUV
LIBYUV_API extern const struct ArgbConstants kArgbI601Constants;  // BT.601
LIBYUV_API extern const struct ArgbConstants kAbgrI601Constants;
LIBYUV_API extern const struct ArgbConstants kRgbaI601Constants;
LIBYUV_API extern const struct ArgbConstants kBgraI601Constants;
LIBYUV_API extern const struct ArgbConstants kArgbJPEGConstants;  // BT.601 full
LIBYUV_API extern const struct ArgbConstants kAbgrJPEGConstants;
```

Good:

```cpp
// Conversion matrix for xRGB to YUV.
LIBYUV_API extern const struct ArgbConstants kArgbI601Constants;  // BT.601
LIBYUV_API extern const struct ArgbConstants kArgbJPEGConstants;  // BT.601 full
LIBYUV_API extern const struct ArgbConstants kArgbH709Constants;  // BT.709

// Conversion matrix for xBGR to YUV.
LIBYUV_API extern const struct ArgbConstants kAbgrI601Constants;  // BT.601
LIBYUV_API extern const struct ArgbConstants kAbgrJPEGConstants;  // BT.601 full
LIBYUV_API extern const struct ArgbConstants kAbgrH709Constants;  // BT.709
```
