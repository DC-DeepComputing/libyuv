---
name: libyuv/source/cast-64-bit-intrinsic-truncation
description: When assigning 64-bit compiler intrinsic return values to narrower integer types like int, use an explicit cast to prevent MSVC truncation warnings
rubric: correctness
severity: should_fix
---

**Rule.** When assigning the return value of a 64-bit compiler intrinsic (such
as _xgetbv()) or wider integer expression to a narrower integer variable like
int or uint32_t, explicitly cast the result to the destination type. Do not rely
on implicit narrowing conversions, which trigger compiler truncation warnings
(such as MSVC C4244) and break builds configured with warnings treated as errors
(/WX). In files subject to linter checks forbidding C-style casts, use a C++
cast like static_cast<int> or append // NOLINT where project convention requires
a C cast.

**Why.** Compiler intrinsics like MSVC's _xgetbv() return an unsigned 64-bit
integer (unsigned __int64). Assigning the result directly to a 32-bit int causes
MSVC warning C4244 ('conversion from type1 to type2, possible loss of data'),
which breaks builds that treat warnings as errors (/WX). An explicit cast
documents the intentional truncation and silences the warning.

## Examples

Bad:

```cpp
// Assigning 64-bit intrinsic directly to int triggers MSVC C4244 under /WX
int xcr0 = 0;
xcr0 = _xgetbv(0);
```

Good:

```cpp
// Explicit cast prevents MSVC truncation warning
int xcr0 = 0;
xcr0 = (int)_xgetbv(0);  // NOLINT
```
