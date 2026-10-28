---
name: libyuv/source/direct-memory-operands-in-x86-vector-asm
description: In x86 SSE and AVX GCC inline assembly, pass memory operands directly to vector instructions instead of loading non-reused constants into scratch registers
rubric: safety
severity: nit
---

**Rule.** In x86/SSE/AVX GCC inline assembly, pass memory operands directly to
vector instructions (such as pand, por, pxor, pmulhuw, or vpand) when
referencing single-use constants or structure fields. Avoid loading the value
into a temporary scratch vector register with movdqa or vmovdqu unless the
constant is reused across multiple registers or operations.

**Why.** x86 vector arithmetic and bitwise instructions support direct memory
operands. Using memory operands directly eliminates unnecessary register moves,
frees scratch vector registers, reduces register pressure, and minimizes
instruction count in tight row loops.

## Examples

Bad:

```cpp
// Bad: Loads constant into scratch register %%xmm7 before single use
"movdqa     256(%[yuvconstants]),%%xmm7                     \n"
"pand       %%xmm7,%%xmm1                                   \n"
```

Good:

```cpp
// Good: Passes memory operand directly to vector instruction
"pand       256(%[yuvconstants]),%%xmm1                     \n"
```
