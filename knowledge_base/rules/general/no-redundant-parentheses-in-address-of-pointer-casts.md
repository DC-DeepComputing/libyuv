---
name: libyuv/no-redundant-parentheses-in-address-of-pointer-casts
description: When casting the address of a variable or constant to a pointer type, omit redundant enclosing parentheses around the address-of expression
rubric: readability
severity: nit
---

**Rule.** Do not wrap address-of expressions in redundant parentheses within
pointer casts: write `(Type*)&var` or `reinterpret_cast<Type*>(&var)` instead of
`(Type*)(&var)`. Unary address-of and cast operators associate right-to-left at
the same precedence level, making inner parentheses unnecessary.

**Why.** Redundant parentheses around the address-of operator add visual clutter
without changing evaluation order or disambiguating syntax.

## Examples

Bad:

```cpp
return AR64Shuffle(src_ar64, src_stride_ar64, dst_ab64, dst_stride_ab64,
                   (const uint8_t*)(&kShuffleMaskAR64ToAB64), width, height);
```

Good:

```cpp
return AR64Shuffle(src_ar64, src_stride_ar64, dst_ab64, dst_stride_ab64,
                   (const uint8_t*)&kShuffleMaskAR64ToAB64, width, height);
```
