---
name: libyuv/source/source-pass-power-of-two-alignment-to-is-aligned
description: When checking pointer or width alignment with IS_ALIGNED(p, a), pass the power-of-two boundary value rather than a bitmask like a - 1
rubric: correctness
severity: should_fix
---

**Rule.** When using IS_ALIGNED(p, a) to check pointer or width alignment, pass
the power-of-two alignment boundary (a, such as 8, 16, 32, or 64), not an
already-decremented bitmask (a - 1, such as 7, 15, 31, or 63). Unlike ANY*
row-dispatch macros in row_any.cc that take the bitmask, IS_ALIGNED subtracts 1
internally.

**Why.** IS_ALIGNED(p, a) is defined as (!((uintptr_t)(p) & ((a) - 1))). Passing
a bitmask like 7 causes the macro to compute (7) - 1 == 6 (0b110), checking bits
1 and 2 while ignoring bit 0, which incorrectly reports odd, unaligned values as
aligned.

## Examples

Bad:

```cpp
#if defined(HAS_HALFFLOATROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    HalfFloatRow =
        scale == 1.0f ? HalfFloat1Row_Any_NEON : HalfFloatRow_Any_NEON;
    if (IS_ALIGNED(width, 7)) {
      HalfFloatRow = scale == 1.0f ? HalfFloat1Row_NEON : HalfFloatRow_NEON;
    }
  }
#endif
```

Good:

```cpp
#if defined(HAS_HALFFLOATROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    HalfFloatRow =
        scale == 1.0f ? HalfFloat1Row_Any_NEON : HalfFloatRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      HalfFloatRow = scale == 1.0f ? HalfFloat1Row_NEON : HalfFloatRow_NEON;
    }
  }
#endif
```
