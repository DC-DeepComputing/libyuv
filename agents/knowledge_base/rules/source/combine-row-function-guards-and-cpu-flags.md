---
name: libyuv/source/combine-row-function-guards-and-cpu-flags
description: When initializing multiple SIMD row function pointers for the same CPU architecture, combine preprocessor feature guards and use a single TestCpuFlag check
rubric: readability
severity: nit
---

**Rule.** When initializing multiple related row conversion or scaling function
pointers (such as Y and UV row kernels) for the same CPU architecture, combine
their preprocessor feature macros in a single `#if defined(...) && defined(...)`
guard. Perform a single `TestCpuFlag(...)` check inside the block to assign all
related function pointers rather than repeating separate preprocessor guards and
CPU flag tests for each pointer.

**Why.** Consolidating related SIMD row kernel pointer setup eliminates
redundant runtime CPU feature checks, reduces preprocessor boilerplate, and
keeps architecture dispatch logic structured and maintainable across conversion
routines.

## Examples

Bad:

```cpp
#if defined(HAS_BGRATOYROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    BGRAToYRow = BGRAToYRow_Any_MMI;
    if (IS_ALIGNED(width, 8)) {
      BGRAToYRow = BGRAToYRow_MMI;
    }
  }
#endif
#if defined(HAS_BGRATOUVROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    BGRAToUVRow = BGRAToUVRow_Any_MMI;
    if (IS_ALIGNED(width, 16)) {
      BGRAToUVRow = BGRAToUVRow_MMI;
    }
  }
#endif
```

Good:

```cpp
#if defined(HAS_BGRATOYROW_MMI) && defined(HAS_BGRATOUVROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    BGRAToYRow = BGRAToYRow_Any_MMI;
    BGRAToUVRow = BGRAToUVRow_Any_MMI;
    if (IS_ALIGNED(width, 8)) {
      BGRAToYRow = BGRAToYRow_MMI;
    }
    if (IS_ALIGNED(width, 16)) {
      BGRAToUVRow = BGRAToUVRow_MMI;
    }
  }
#endif
```
