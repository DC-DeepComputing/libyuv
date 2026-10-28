---
name: libyuv/keep-format-and-color-space-acronyms-uppercase
description: When naming constants, variables, or functions containing pixel formats or color spaces, keep acronyms uppercase (e.g., kABGRToUVJ, ARGB) instead of capitalizing as single words
rubric: readability
severity: should_fix
---

**Rule.** Keep pixel format and color-space acronyms in all-uppercase letters
within identifier names, constants, and function names (e.g., use
kABGRToUVJCoefficients rather than kAbgrToUvj..., and kARGBToRGB24RowIndices
rather than kArgbToRgb24...). Do not capitalize format acronyms as single words
(such as Argb, Abgr, Rgb, Uvj, or Yuv) in mixed-case kCamelCase or PascalCase
identifiers.

**Why.** In libyuv, format names and color spaces (ARGB, ABGR, RGBA, BGRA,
RGB24, RAW, UV, UVJ, YUV) designate specific FourCC memory layouts and channel
orders. Keeping acronyms fully capitalized maintains consistency across row
conversion functions, lookup tables, and SIMD constants, making format
conversions immediately recognizable.

## Examples

Bad:

```cpp
// Bad: format and color-space acronyms capitalized as single words
static const int16_t kAbgrToUvjArr[] = { ... };
static const uint8_t kShuffleMaskArgbToRgba[] = { ... };
```

Good:

```cpp
// Good: format and color-space acronyms kept fully uppercase
static const int16_t kABGRToUVJCoefficients[] = { ... };
static const uint8_t kShuffleMaskARGBToRGBA[] = { ... };
```
