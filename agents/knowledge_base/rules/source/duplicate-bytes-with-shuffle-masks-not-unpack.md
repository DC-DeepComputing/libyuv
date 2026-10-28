---
name: libyuv/source/duplicate-bytes-with-shuffle-masks-not-unpack
description: In x86 SSSE3/AVX2 SIMD kernels reordering and duplicating bytes, use pshufb/vpshufb with byte-duplicating masks instead of chaining shuffles with punpcklbw/punpckhbw unpack instructions
rubric: safety
severity: nit
---

**Rule.** In x86 SSSE3, AVX2, or AVX-512 SIMD kernels that reorder and duplicate
8-bit channels into 16-bit values, use pshufb/vpshufb (or
_mm_shuffle_epi8/_mm256_shuffle_epi8) with byte-duplicating shuffle masks for
low and high halves rather than chaining a reordering shuffle with
punpcklbw/punpckhbw unpack instructions.

**Why.** pshufb and vpshufb can simultaneously reorder and duplicate arbitrary
byte positions within a 128-bit lane in a single instruction. Using separate low
and high shuffle masks with replicated byte indices eliminates register copying
and subsequent punpcklbw/punpckhbw unpack instructions, reducing inner-loop
instruction count and register pressure.

## Examples

Bad:

```cpp
"vpshufb     %%ymm2,%%ymm0,%%ymm0          \n"
"vpunpckhbw  %%ymm0,%%ymm0,%%ymm1          \n"
"vpunpcklbw  %%ymm0,%%ymm0,%%ymm0          \n"
```

Good:

```cpp
// Using masks that duplicate byte indices, e.g. {2, 2, 1, 1, 0, 0, 3, 3, ...}
"vpshufb     %%ymm3,%%ymm0,%%ymm1          \n"
"vpshufb     %%ymm2,%%ymm0,%%ymm0          \n"
```
