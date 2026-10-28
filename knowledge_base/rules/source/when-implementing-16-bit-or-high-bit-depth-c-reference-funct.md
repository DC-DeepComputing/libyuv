---
name: libyuv/source/when-implementing-16-bit-or-high-bit-depth-c-reference-funct
description: When implementing 16-bit or high-bit-depth C reference functions (_16_C), mirror the arithmetic structure and expressions of corresponding 8-bit implementations rather than factoring terms
rubric: readability
severity: should_fix
---

**Rule.** When implementing 16-bit or high-bit-depth C reference functions (such
as `_16_C`), keep the arithmetic expressions, weighting formulas, and structural
layout identical to their 8-bit reference counterparts (`_C`). Do not introduce
ad-hoc algebraic factorizations or temporary sub-expression variables unless the
8-bit implementation also uses them.

**Why.** Maintaining identical arithmetic structure and layout across 8-bit and
16-bit portable C reference kernels ensures consistency, readability, and ease
of maintenance, and avoids subtle rounding or behavioral divergences while
allowing the compiler to perform common sub-expression optimizations.

## Examples

Bad:

```cpp
// In ScaleRowDown34_0_Box_16_C: ad-hoc algebraic factorization differing from 8-bit version
uint16_t a0a1b0b1 = s[x + 0] + s[x + 1] + t[x + 0] + t[x + 1];
uint16_t a0b1_2 = 2 * (s[x + 0] + t[x + 1]);
uint16_t a1b0_2 = 2 * (s[x + 1] + t[x + 0]);
d[2 * x + 0] = (s[x + 0] * 8 + a1b0_2 + a0a1b0b1 + 8) >> 4;
```

Good:

```cpp
// In ScaleRowDown34_0_Box_16_C: exact arithmetic structure mirrored from ScaleRowDown34_0_Box_C
d[2 * x + 0] =
    (s[x + 0] * 9 + s[x + 1] * 3 + t[x + 0] * 3 + t[x + 1] * 1 + 8) >> 4;
```
