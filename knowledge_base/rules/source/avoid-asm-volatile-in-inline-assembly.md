---
name: libyuv/source/avoid-asm-volatile-in-inline-assembly
description: When writing inline assembly blocks, use asm(...) instead of asm volatile(...) and explicitly declare all input, output, and clobber constraints.
rubric: readability
severity: should_fix
---

**Rule.** Do not use `asm volatile(...)`; use `asm(...)` for inline assembly
blocks and explicitly declare all input, output, and register/memory clobber
constraints instead.

**Why.** The `volatile` qualifier on inline assembly is ignored by certain
link-time code generation and profile-guided optimization pipelines (such as
pgolite) and interferes with automated formatting tools. Explicitly declaring
all input, output, and clobber constraints enables the compiler to properly
model data dependencies and side effects without `volatile`.

## Examples

Bad:

```cpp
asm volatile(
    "cnth     %[vl]                                   \n"
    "ptrue    p0.b                                    \n"
    : [vl] "=&r"(vl)
    : [src] "r"(src), [dst] "r"(dst)
    : "memory", "p0");
```

Good:

```cpp
asm("cnth     %[vl]                                   \n"
    "ptrue    p0.b                                    \n"
    : [vl] "=&r"(vl)
    : [src] "r"(src), [dst] "r"(dst)
    : "memory", "p0");
```
