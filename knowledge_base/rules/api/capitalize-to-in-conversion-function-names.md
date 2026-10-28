---
name: libyuv/api/capitalize-to-in-conversion-function-names
description: When declaring or naming format and bit-depth conversion functions, row helpers, and test macros, capitalize 'To' in PascalCase (e.g., 16To8, I420ToARGB) instead of lowercase 'to'
rubric: readability
severity: nit
---

**Rule.** When declaring or defining format conversion, bit-depth conversion,
scaling functions, row helpers, and test macros, capitalize 'To' as an
independent word in PascalCase identifiers (e.g., '16To8', 'ARGBToI420',
'ScalePlaneVertical_16To8', 'Convert16To8'). Avoid lowercase 'to' (such as
'16to8', 'argbtoi420', or 'ARGBtoI420') across all public APIs, kernel helpers,
and unit tests.

**Why.** libyuv consistently treats 'To' as an independent capitalized token in
PascalCase across all public format conversion APIs, bit-depth transitions, row
processing kernels, and test macros. Capitalizing 'To' ensures uniform naming
across formats and bit depths and prevents naming discrepancies between API
declarations, implementations, and unit test suites.

## Examples

Bad:

```cpp
// Bad: lowercase 'to' in format conversion and bit-depth scaling helpers
int I420toARGB(const uint8_t* src_y, int src_stride_y,
               uint8_t* dst_argb, int dst_stride_argb,
               int width, int height);

void ScalePlaneVertical_16to8(const uint16_t* src_ptr,
                              uint8_t* dst_ptr,
                              int dst_width);
```

Good:

```cpp
// Good: capitalize 'To' in PascalCase conversion function and helper names
int I420ToARGB(const uint8_t* src_y, int src_stride_y,
               uint8_t* dst_argb, int dst_stride_argb,
               int width, int height);

void ScalePlaneVertical_16To8(const uint16_t* src_ptr,
                              uint8_t* dst_ptr,
                              int dst_width);
```
