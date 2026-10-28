---
name: libyuv/const-for-immutable-local-variables
description: When declaring local variables, limits, masks, or pointers that are initialized and not modified thereafter, qualify them with const
rubric: readability
severity: nit
---

**Rule.** Declare local variables, limits, masks, and constants as `const` (or
`const auto` / `const auto&` / `const T*`) when their values or referenced data
are initialized and not modified thereafter within their scope. Do not leave
immutable function-local variables non-const.

**Why.** Marking unmodified local variables as `const` documents programmer
intent, catches accidental reassignment or mutation errors at compile time, aids
compiler optimizations, and follows the style guideline requirement to use
`const` where possible.

## Examples

Bad:

```cpp
// Example 1: Local computation variables
int shift = __builtin_clz(scale) - 15;
int step = (width + 15) / 16;

// Example 2: Constant limit passed to inline assembly
uint16_t limit = 0x3ff0;
asm(YUVTORGB_SETUP
    "dup      v22.8h, %w[limit]                  \n"
    : [limit] "r"(limit));
```

Good:

```cpp
// Example 1: Local computation variables
const int shift = __builtin_clz(scale) - 15;
const int step = (width + 15) / 16;

// Example 2: Constant limit passed to inline assembly
const uint16_t limit = 0x3ff0;
asm(YUVTORGB_SETUP
    "dup      v22.8h, %w[limit]                  \n"
    : [limit] "r"(limit));
```
