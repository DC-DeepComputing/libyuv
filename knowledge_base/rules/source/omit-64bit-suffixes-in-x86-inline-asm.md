---
name: libyuv/source/omit-64bit-suffixes-in-x86-inline-asm
description: In x86 GCC/Clang inline assembly, avoid 64-bit suffixes like subq or addq and use size-agnostic mnemonics like sub or add for 32-bit and 64-bit compatibility
rubric: correctness
severity: blocking
---

**Rule.** In GCC/Clang x86 inline assembly targeting both 32-bit and 64-bit
architectures (source/*_gcc.cc), do not use 64-bit-specific instruction suffixes
(such as subq, addq, cmpq, leaq, or movq on general-purpose registers). Instead,
use size-agnostic instruction mnemonics (such as sub, add, cmp, lea, mov, and,
or, xor) so the assembler automatically matches operand width across both 32-bit
and 64-bit builds.

**Why.** Instruction mnemonics with explicit 64-bit size suffixes like subq fail
to assemble or break execution on 32-bit x86 (i386) targets where 64-bit
general-purpose registers and quadword operations do not exist. Using
un-suffixed, size-agnostic mnemonics allows the assembler to automatically infer
register and pointer widths from operands on both 32-bit and 64-bit x86_64
builds.

## Examples

Bad:

```cpp
// Bad: 64-bit specific instruction suffixes break when assembling for 32-bit x86.
asm volatile(
    "subq        %0,%1                         \n"
    "subq        %0,%2                         \n"
    : "+r"(src_ptr), "+r"(dst_ptr)
    : "r"(offset)
);
```

Good:

```cpp
// Good: Size-agnostic mnemonics adapt automatically to 32-bit and 64-bit register widths.
asm volatile(
    "sub         %0,%1                         \n"
    "sub         %0,%2                         \n"
    : "+r"(src_ptr), "+r"(dst_ptr)
    : "r"(offset)
);
```
