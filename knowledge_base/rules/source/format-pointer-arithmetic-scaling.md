---
name: libyuv/source/format-pointer-arithmetic-scaling
description: When advancing pointers by scaled element counts or stride multipliers, write ptr += count * factor; with the variable first and no extra parentheses
rubric: readability
severity: nit
---

**Rule.** When advancing pointers by a scaled count or byte multiplier (such as
vector length, width, or stride times bytes per pixel), format the arithmetic as
`ptr += count * factor;` or `ptr -= count * factor;`. Place the variable count
or stride before the constant multiplier and omit redundant parentheses around
the product.

**Why.** libyuv codebase conventions standardize on `ptr += count * factor;`
across all conversion and scaling functions. Placing the variable count first
and avoiding superfluous parentheses maintains consistent readability across
SIMD and scalar row processing loops.

## Examples

Bad:

```cpp
// Redundant parentheses and constant preceding variable count
src_argb += (4 * vl);
dst_ar64 += (8 * vl);
src_ptr += (2 * src_stride);
```

Good:

```cpp
// Variable count preceding constant factor without redundant parentheses
src_argb += vl * 4;
dst_ar64 += vl * 8;
src_ptr += src_stride * 2;
```
