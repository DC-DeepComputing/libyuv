---
name: libyuv/document-numeric-constant-derivation
description: When introducing non-obvious numeric constants, rounding biases, scale factors, or conversion offsets in assembly, intrinsics, or math routines, document their derivation and purpose in comments
rubric: readability
severity: should_fix
---

**Rule.** Document the mathematical derivation, formula, or purpose of
non-obvious numeric constants (such as fixed-point rounding biases, scaling
multipliers, and signed/unsigned conversion offsets like 0x8000 for
pmaddwd/vpmaddwd), especially when loading raw numeric literals in SIMD assembly
kernels, intrinsics, or math routines. Provide an explanatory comment detailing
how the value was derived (e.g., `// 32768 * 256 + 128 rounding constant` or `//
Bias unsigned words to signed range for vpmaddwd`) alongside the constant
definition or immediate operand.

**Why.** Raw integer literals and unexplained magic constants obscure arithmetic
algorithms, rounding behavior, and fixed-point precision choices. In SIMD
instructions (such as pmaddwd), signed inputs require biasing and adjusted
downstream rounding offsets. Explaining their mathematical derivation and
purpose allows maintainers to verify correctness, understand precision
boundaries, and adapt constants for new bit depths or platforms without
reverse-engineering the arithmetic.

## Examples

Bad:

```cpp
// Bad: Raw magic constants and offsets in SIMD assembly without derivation or explanation
"mov         $0x80008000,%%eax             \n"
"vmovd       %%eax,%%xmm4                  \n"
"vbroadcastss %%xmm4,%%ymm4                \n"
"mov         $8388736,%%eax                \n"
"vmovd       %%eax,%%xmm3                  \n"
"vbroadcastss %%xmm3,%%ymm3                \n"
```

Good:

```cpp
// Good: Clear comments explaining constant derivation and arithmetic purpose
"mov         $0x80008000,%%eax             \n"  // Bias unsigned words to signed range for vpmaddwd.
"vmovd       %%eax,%%xmm4                  \n"
"vbroadcastss %%xmm4,%%ymm4                \n"
"mov         $8388736,%%eax                \n"  // 32768 * 256 + 128 rounding constant.
"vmovd       %%eax,%%xmm3                  \n"
"vbroadcastss %%xmm3,%%ymm3                \n"
```
