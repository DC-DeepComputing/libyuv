---
name: libyuv/api/row-function-width-parameter-naming
description: When declaring or implementing row processing functions (*Row_*), name the horizontal element or pixel count parameter width rather than count across all architecture variants
rubric: readability
severity: nit
---

**Rule.** In row function declarations and definitions (*Row_*) and remainder
wrappers (*Row_Any_*), maintain consistent parameter names across all
architecture variants. Use width (or dst_width / src_width for scaling kernels)
rather than count for horizontal element or pixel count parameters.

**Why.** Inconsistent parameter naming between header declarations,
implementations, and sibling architecture variants creates confusion, causes
declaration-parameter-name mismatches, and breaks uniformity across SIMD ports
and dispatch wrappers.

## Examples

Bad:

```cpp
// Bad: Inconsistent parameter naming across architecture variants, using count instead of width.
void SetRow_C(uint8_t* dst, uint8_t v8, int width);
void SetRow_MSA(uint8_t* dst, uint8_t v8, int count);

void ARGBToRGB24Row_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
void ARGBToRGB24Row_NEON(const uint8_t* src_argb, uint8_t* dst_rgb, int count);
```

Good:

```cpp
// Good: Consistent parameter naming ('width') across all architecture variants.
void SetRow_C(uint8_t* dst, uint8_t v8, int width);
void SetRow_MSA(uint8_t* dst, uint8_t v8, int width);

void ARGBToRGB24Row_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
void ARGBToRGB24Row_NEON(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
```
