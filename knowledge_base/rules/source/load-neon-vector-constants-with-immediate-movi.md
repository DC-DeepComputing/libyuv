---
name: libyuv/source/load-neon-vector-constants-with-immediate-movi
description: In ARM NEON inline assembly, load constant vectors directly using immediate vector instructions (e.g. movi) instead of passing scalar constants via GPR inputs and duplicating with dup
rubric: safety
severity: nit
---

**Rule.** In ARM NEON and AArch64 inline assembly routines, load constant
vectors directly using immediate vector instructions (such as movi, mvni, or
fmov) when the constant fits the immediate encoding format. Do not pass scalar
constants through general-purpose register input constraints ("r") and broadcast
them with dup or vdup.

**Why.** Passing constant values via GPR input constraints forces the compiler
to allocate an unnecessary general-purpose register, emit scalar instructions or
memory loads to initialize it, and perform a cross-register-bank transfer into
SIMD registers via dup. Vector immediate instructions load constants directly
into the SIMD register file in a single instruction without GPR register
pressure or extra setup instructions.

## Examples

Bad:

```cpp
uint16_t alpha = 0xc000;
asm(
    "dup      v23.8h, %w[alpha]                  \n"
    "ld1      {v0.8h}, [%0], #16                 \n"
    "add      v0.8h, v0.8h, v23.8h               \n"
    :
    : "r"(src), [alpha] "r"(alpha));
```

Good:

```cpp
asm(
    "movi     v23.8h, #0xc0, lsl #8              \n"  // 0xc000
    "ld1      {v0.8h}, [%0], #16                 \n"
    "add      v0.8h, v0.8h, v23.8h               \n"
    :
    : "r"(src));
```
