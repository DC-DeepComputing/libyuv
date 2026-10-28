---
name: libyuv/use-named-operands-in-inline-assembly
description: When writing inline assembly with named operands, reference operands consistently using named placeholders like %[name] rather than positional numeric indices
rubric: readability
severity: nit
---

**Rule.** In inline assembly blocks that define named operands ([name]), refer
to operands consistently using named placeholders (such as %[name] or %w[name])
rather than positional numeric indices (such as %0, %2, or %w2).

**Why.** Positional operand numbers in inline assembly are fragile and break
silently when inputs, outputs, or clobbers are added, removed, or reordered.
Using named placeholders makes assembly instructions self-documenting and
resilient to operand list refactoring.

## Examples

Bad:

```cpp
__asm__ volatile(
    "whilelt     p0.b, wzr, %w2                       \n"
    "ld1b        {z0.b}, p0/z, [%[src]]               \n"
    "st1b        {z0.b}, p0, [%[dst]]                 \n"
    : [dst] "+r"(dst)
    : [src] "r"(src), [width] "r"(width)
    : "p0", "z0", "memory");
```

Good:

```cpp
__asm__ volatile(
    "whilelt     p0.b, wzr, %w[width]                 \n"
    "ld1b        {z0.b}, p0/z, [%[src]]               \n"
    "st1b        {z0.b}, p0, [%[dst]]                 \n"
    : [dst] "+r"(dst)
    : [src] "r"(src), [width] "r"(width)
    : "p0", "z0", "memory");
```
