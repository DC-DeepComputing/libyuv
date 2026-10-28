---
name: libyuv/source/prefer-simd-arithmetic-over-bitwise-tricks
description: In SIMD assembly routines performing bias adjustments or arithmetic, prefer explicit instructions like psubb/vpsubb over equivalent bitwise tricks like pxor/vpxor
rubric: readability
severity: nit
---

**Rule.** In SIMD assembly routines and vector intrinsics, use explicit
arithmetic instructions (such as psubb, vpsubb, or _mm_sub_epi8 for bias
subtraction) rather than bitwise operations (such as pxor, vpxor with 0x80) when
latency and throughput are identical. Express calculations using direct
arithmetic instructions to make the mathematical intent clear.

**Why.** In 8-bit two's complement arithmetic, flipping the sign bit with XOR
0x80 mathematically equals subtracting 128 (e.g., converting unsigned chroma to
signed UV bias). Explicit arithmetic instructions communicate this calculation
directly to readers without any performance difference on modern CPUs.

## Examples

Bad:

```cpp
// Flipping sign bit with XOR 0x80 to subtract 128 bias
"vpxor       %%ymm13,%%ymm3,%%ymm3                              \n"
```

Good:

```cpp
// Explicit subtraction directly conveys mathematical intent (uv -= 128)
"vpsubb      %%ymm13,%%ymm3,%%ymm3                              \n"
```
