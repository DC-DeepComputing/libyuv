---
name: libyuv/source/prefer-direct-multiplications-in-c-fallbacks
description: When writing arithmetic in C fallback or reference loops, use direct multiplications for linear combinations rather than complex manual algebraic factorizations
rubric: safety
severity: should_fix
---

**Rule.** In C fallback and reference loops (such as image scaling,
interpolation, and row filters), write linear combinations and weighted averages
directly using explicit multiplications rather than manual algebraic
factorizations or shift-and-add tricks. Keep the mathematical expressions direct
and straightforward to maintain readability and enable compiler
auto-vectorization.

**Why.** Modern CPUs feature fast, pipelined hardware multipliers. Manual
algebraic factorizations that pre-sum shared terms to eliminate multiplications
introduce complex intermediate data dependencies that obscure mathematical
intent and prevent compiler auto-vectorizers from generating efficient SIMD
code.

## Examples

Bad:

```cpp
uint16_t a0a1b0b1 = s[x + 0] + s[x + 1] + t[x + 0] + t[x + 1];
uint16_t a0b1_2 = 2 * (s[x + 0] + t[x + 1]);
uint16_t a1b0_2 = 2 * (s[x + 1] + t[x + 0]);
d[2 * x + 0] = (s[x + 0] * 8 + a1b0_2 + a0a1b0b1 + 8) >> 4;
d[2 * x + 1] = (s[x + 1] * 8 + a0b1_2 + a0a1b0b1 + 8) >> 4;
```

Good:

```cpp
d[2 * x + 0] =
    (s[x + 0] * 9 + s[x + 1] * 3 + t[x + 0] * 3 + t[x + 1] * 1 + 8) >> 4;
d[2 * x + 1] =
    (s[x + 0] * 3 + s[x + 1] * 9 + t[x + 0] * 1 + t[x + 1] * 3 + 8) >> 4;
```
