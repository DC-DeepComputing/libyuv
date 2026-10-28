---
name: libyuv/source/qualify-extended-inline-asm-volatile
description: When writing GCC or Clang extended inline assembly, qualify asm statements with volatile (asm volatile) to prevent compilers from eliminating them as dead code
rubric: correctness
severity: should_fix
---

**Rule.** When writing GCC or Clang extended inline assembly blocks in SIMD row,
scale, conversion, or utility kernels, always qualify the `asm` statement with
`volatile` (`asm volatile(...)` or `__asm__ __volatile__(...)`). Do not omit the
`volatile` qualifier even when output operands are specified.

**Why.** Compilers such as GCC and Clang can treat non-volatile extended inline
assembly blocks with output operands as pure functions with no side effects. If
the modified output variables (such as mutated pointer registers or width
counters) are not subsequently read or returned by the enclosing function, the
compiler may optimize away the entire assembly block as dead code or unsafely
reorder it.

## Examples

Bad:

```cpp
// Non-volatile inline assembly can be optimized away by GCC/Clang if outputs are unused.
asm(YUVTORGB_SETUP
    "dup      v22.8h, %w[limit]                  \n"
    "1:                                          \n"
    "subs     %w[width], %w[width], #8           \n"
    "b.gt     1b                                 \n"
    : [src_y] "+r"(src_y),
      [dst_ar30] "+r"(dst_ar30),
      [width] "+r"(width)
    : [limit] "r"(limit)
    : "cc", "memory", "v22");
```

Good:

```cpp
// Qualify with volatile to guarantee execution and prevent dead-code elimination.
asm volatile(YUVTORGB_SETUP
             "dup      v22.8h, %w[limit]                  \n"
             "1:                                          \n"
             "subs     %w[width], %w[width], #8           \n"
             "b.gt     1b                                 \n"
             : [src_y] "+r"(src_y),
               [dst_ar30] "+r"(dst_ar30),
               [width] "+r"(width)
             : [limit] "r"(limit)
             : "cc", "memory", "v22");
```
