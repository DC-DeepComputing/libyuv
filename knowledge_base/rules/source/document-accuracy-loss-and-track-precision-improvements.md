---
name: libyuv/source/document-accuracy-loss-and-track-precision-improvements
description: When using fixed-point approximations, clamped conversion coefficients, or reduced-precision math, document the accuracy error and add a tracking TODO for precision improvements
rubric: readability
severity: nit
---

**Rule.** When using fixed-point integer approximations or clamping color
conversion matrix coefficients that cause known accuracy loss, document the
expected error magnitude in a comment and add a tracking TODO to explore future
precision improvements. Explain the exact derivation or clamping rationale
alongside the coefficient definition.

**Why.** Fitting continuous floating-point conversion coefficients into bounded
fixed-point integer multipliers (such as 8-bit SIMD registers) can introduce
noticeable color or numerical error. Documenting the discrepancy and tracking
future precision work ensures maintainers understand accuracy tradeoffs and know
which paths need improvement.

## Examples

Bad:

```cpp
// Clamping floating-point coefficient to 8-bit fixed-point range with no explanation or tracking.
#define UB -128 /* max(-128, round(-2.142 * 64)) */
```

Good:

```cpp
// TODO(fbarchard): Improve accuracy; the B channel is off by 7% due to 8-bit clamping.
#define UB -128 /* max(-128, round(-2.142 * 64)) */
```
