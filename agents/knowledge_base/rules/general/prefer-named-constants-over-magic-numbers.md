---
name: libyuv/prefer-named-constants-over-magic-numbers
description: When defining constants, expressions, step sizes, or alignment masks, define and use named constants instead of repeating raw magic numbers.
rubric: readability
severity: should_fix
---

**Rule.** Define and use named `const` or `constexpr` identifiers (e.g.,
`kSimdSize`, `kBlockSize`) instead of repeating raw numeric literals (magic
numbers) in constant definitions, array sizing, or arithmetic expressions. When
a component dimension, SIMD step size, or limit has a defined constant, derive
related constants and expressions directly from it. Express bitwise operations
and alignment masks in terms of the named constant (such as `~(kSimdSize - 1)`
and `kSimdSize - 1`) rather than duplicating raw numeric masks.

**Why.** Duplicating magic numbers or hardcoding raw values across expressions
and constant definitions obscures intent, risks subtle synchronization bugs if
buffer, block, or SIMD alignment sizes change, and harms maintainability.

## Examples

Bad:

```cpp
// Hardcoding raw magic numbers in definitions and mask expressions:
const int kSimdSize = 64;
const int kBlockSize = (65536 - 64);

int remainder = count & (kBlockSize - 1) & ~63;
if (remainder) {
  Process_SIMD(src, remainder);
  src += remainder;
}
remainder = count & 63;
```

Good:

```cpp
// Defining named constants and deriving expressions and masks consistently:
const int kSimdSize = 64;
const int kBlockSize = (65536 - kSimdSize);

int remainder = count & (kBlockSize - 1) & ~(kSimdSize - 1);
if (remainder) {
  Process_SIMD(src, remainder);
  src += remainder;
}
remainder = count & (kSimdSize - 1);
```
