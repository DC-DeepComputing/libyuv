---
name: libyuv/omit-redundant-parentheses-around-standalone-operands
description: When writing binary, bitwise, or arithmetic expressions, omit redundant parentheses around standalone variable identifiers and operands where operator precedence is unambiguous
rubric: readability
severity: nit
---

**Rule.** Do not surround bare single variable identifiers or standalone
operands with redundant parentheses in binary, bitwise, or arithmetic
expressions (e.g., write `(x << 4) | x` or `x | (y << 5)` instead of `(x << 4) |
(x)` or `(x) | (y << 5)`). Reserve parentheses for compound subexpressions or
operations where operator precedence would otherwise be ambiguous or
non-obvious.

**Why.** Parenthesizing isolated variable identifiers adds visual noise and
clutter without altering operator precedence, evaluation order, or readability.

## Examples

Bad:

```cpp
// Redundant parentheses around standalone variables b, b0, g0:
b = STATIC_CAST(uint8_t, (b << 4) | (b));
uint8_t out = STATIC_CAST(uint8_t, (b0 << 4) | (b0));
uint16_t packed = STATIC_CAST(uint16_t, (b0) | (g0 << 5) | (r0 << 11));
```

Good:

```cpp
// Standalone variable operands are unparenthesized:
b = STATIC_CAST(uint8_t, (b << 4) | b);
uint8_t out = STATIC_CAST(uint8_t, (b0 << 4) | b0);
uint16_t packed = STATIC_CAST(uint16_t, b0 | (g0 << 5) | (r0 << 11));
```
