---
name: libyuv/capitalize-simd-and-cpu-acronyms-in-comments
description: When referencing CPU architectures or SIMD instruction set extensions in code comments, use uppercase acronyms such as MMI, MSA, NEON, AVX2, and SSE2.
rubric: readability
severity: nit
---

**Rule.** Spell CPU architecture and SIMD instruction set acronyms (e.g., MMI,
MSA, NEON, SVE, AVX2, SSE2, SSSE3, RVV, LASX) in uppercase in code comments.
Avoid using all-lowercase names like `mmi`, `msa`, or `neon` when referring to
instruction sets or hardware extensions in comments.

**Why.** Hardware architectures and SIMD instruction set extensions are standard
technical acronyms; capitalizing them consistently in comments maintains
professional documentation style and prevents ambiguity with function prefixes
or variable names.

## Examples

Bad:

```cpp
// mmi and msa version does direct RGB565 to YUV.
// neon implementation handles 16 pixels per iteration.
```

Good:

```cpp
// MMI and MSA version does direct RGB565 to YUV.
// NEON implementation handles 16 pixels per iteration.
```
