---
name: libyuv/source/cast-bit-count-intrinsic-operands
description: When passing operands to bit-count intrinsics like __builtin_clz or __builtin_ctz in bit-width-dependent shift formulas, explicitly cast operands to fixed-width int32_t or uint32_t
rubric: correctness
severity: should_fix
---

**Rule.** Explicitly cast operands to fixed-width types (such as `int32_t` or
`uint32_t`) when passing them to bit-count intrinsics like `__builtin_clz` or
`__builtin_ctz` in shift or bit-offset calculations that depend on a fixed
32-bit width. Do not pass uncast variables whose type or width may vary across
architectures or typedefs.

**Why.** Bit-count builtins like `__builtin_clz` operate on standard `unsigned
int`, whose size is platform-dependent. In formulas that compute shift amounts
relative to a fixed bit width (such as `15 - clz(x)` or `23 - clz(x)`), passing
an uncast operand can cause incorrect shift counts or cross-platform
inconsistencies if operand width or promotion changes.

## Examples

Bad:

```cpp
const int shift = 15 - __builtin_clz(scale);
```

Good:

```cpp
const int shift = 15 - __builtin_clz((int32_t)scale);
```
