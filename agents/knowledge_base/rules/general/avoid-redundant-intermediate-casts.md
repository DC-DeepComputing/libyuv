---
name: libyuv/avoid-redundant-intermediate-casts
description: When computing compound arithmetic or bitwise expressions on narrow integer types, rely on default integer promotion and cast only the final result
rubric: readability
severity: nit
---

**Rule.** In compound arithmetic or bitwise expressions involving narrow integer
types (such as `uint8_t` or `uint16_t`), do not cast intermediate operands or
subexpressions to narrow types. Rely on standard integer promotion during
evaluation and cast only the final combined result once.

**Why.** In C and C++, integer operands narrower than `int` are automatically
promoted to `int` in arithmetic and bitwise expressions. Intermediate casts to
narrow types are redundant, add visual clutter, and can introduce subtle
truncation issues before subsequent operations.

## Examples

Bad:

```cpp
uint8_t g = STATIC_CAST(uint8_t, src_rgb565[0] >> 5) |
            STATIC_CAST(uint8_t, (src_rgb565[1] & 0x07) << 3);
```

Good:

```cpp
uint8_t g = STATIC_CAST(
    uint8_t, (src_rgb565[0] >> 5) | ((src_rgb565[1] & 0x07) << 3));
```
