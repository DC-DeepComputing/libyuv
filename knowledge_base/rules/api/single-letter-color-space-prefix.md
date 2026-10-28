---
name: libyuv/api/single-letter-color-space-prefix
description: When naming color-space conversion functions, matrix constants, FourCCs, or test helpers, use standard single-letter prefixes (I/J, H/F, U/V) instead of ad-hoc names or un-prefixed identifiers
rubric: readability
severity: should_fix
---

**Rule.** When naming color-space conversion functions, matrix constants (e.g.,
struct ArgbConstants, struct YuvConstants), FourCC definitions, or test helpers,
follow libyuv's standard single-letter color-space prefixes: 'I' (BT.601
limited), 'J' (BT.601 full/JPEG), 'H' (BT.709 limited/HD), 'F' (BT.709 full),
'U' (BT.2020 limited/UHD), and 'V' (BT.2020 full). Do not use multi-letter or
numeric designations such as 'Rec2020', 'BT2020', or '709' in function names or
test references. Additionally, for BT.2020 identifiers, explicitly prefix
limited-range variants with 'U' (e.g., kArgbU2020Constants, FOURCC_U420) and
full-range variants with 'V' (e.g., kArgbV2020Constants, FOURCC_V420); do not
omit the 'U' prefix on limited-range BT.2020 identifiers.

**Why.** libyuv standardizes single-letter prefixes across its public API,
internal kernels, matrix constants, and unit tests to maintain concise, uniform
identifiers and avoid leading or embedded digits in C symbol names. Omitting
range prefixes (such as dropping 'U' from limited-range BT.2020 constants) or
using ad-hoc names (like 'Rec2020' or 'BT709') causes ambiguity between
full-range and limited-range variants and breaks naming consistency across
formats.

## Examples

Bad:

```cpp
// Bad: ad-hoc or numeric color space names in functions/helpers
void YUVRec2020ToRGB(int y, int u, int v, int* r, int* g, int* b);
void YUVBT709ToRGBReference(int y, int u, int v, int* r, int* g, int* b);

// Bad: omitting 'U' prefix on limited-range BT.2020 matrix constants
LIBYUV_API extern const struct ArgbConstants kArgb2020Constants;  // BT.2020 limited
LIBYUV_API extern const struct ArgbConstants kAbgr2020Constants;
LIBYUV_API extern const struct ArgbConstants kArgbV2020Constants;  // BT.2020 full
```

Good:

```cpp
// Good: standard single-letter prefixes ('U' for BT.2020 limited, 'H' for BT.709 limited)
void YUVUToRGB(int y, int u, int v, int* r, int* g, int* b);
void YUVHToRGBReference(int y, int u, int v, int* r, int* g, int* b);

// Good: explicit 'U' prefix for limited range and 'V' for full range
LIBYUV_API extern const struct ArgbConstants kArgbU2020Constants;  // BT.2020 limited
LIBYUV_API extern const struct ArgbConstants kAbgrU2020Constants;
LIBYUV_API extern const struct ArgbConstants kArgbV2020Constants;  // BT.2020 full
```
