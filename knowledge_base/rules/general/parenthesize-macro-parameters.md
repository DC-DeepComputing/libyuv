---
name: libyuv/parenthesize-macro-parameters
description: When defining preprocessor macro expressions, parenthesize parameter references and outer expressions to prevent precedence bugs, but omit redundant parentheses around single-token literals
rubric: readability
severity: should_fix
---

**Rule.** In function-like preprocessor macro definitions, enclose each
parameter reference in parentheses within expressions (e.g., write `(size) * 2`
rather than `size * 2`) and enclose the full macro expression in outer
parentheses to preserve operator precedence. However, omit redundant parentheses
around single-token literals or constants (e.g., write `0xff` or `0` rather than
`(0xff)` or `(0)`).

**Why.** Preprocessor macro arguments expand via verbatim textual substitution
without preserving expression precedence boundaries, so unparenthesized
parameters or outer expressions can bind unexpectedly to adjacent operators in
compound arguments or calling code. Conversely, single-token literals and
constants have no internal operators, so wrapping them in individual parentheses
adds unnecessary visual noise without providing additional precedence safety.

## Examples

Bad:

```cpp
// Parameter `size` is not parenthesized, risking precedence bugs with compound args.
#define ALIGN_BUFFER_END_16(var, size) \
  uint8_t* var##_mem = reinterpret_cast<uint8_t*>(malloc(((size * 2) + 4095 + 63) & ~4095));

// Redundant parentheses around single-token numeric literals.
#define UVMASK(C) (((C) > 127) ? (0xff) : (0))
#define CLAMP255(v) (((v) > (255)) ? (255) : (((v) < (0)) ? (0) : (v)))
```

Good:

```cpp
// Parameter `size` is safely enclosed in parentheses.
#define ALIGN_BUFFER_END_16(var, size) \
  uint8_t* var##_mem = reinterpret_cast<uint8_t*>(malloc((((size) * 2) + 4095 + 63) & ~4095));

// Parameters and outer expressions are parenthesized, but literals omit redundant parentheses.
#define UVMASK(C) ((C) > 127 ? 0xff : 0)
#define CLAMP255(v) ((v) > 255 ? 255 : ((v) < 0 ? 0 : (v)))
```
