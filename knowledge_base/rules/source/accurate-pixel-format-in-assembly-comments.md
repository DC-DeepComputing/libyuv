---
name: libyuv/source/accurate-pixel-format-in-assembly-comments
description: In inline assembly comments, accurately describe instruction semantics, data direction, FOURCC formats, and sample counts, and align comments directly with the instruction performing the operation
rubric: readability
severity: should_fix
---

**Rule.** When writing or refactoring inline assembly routines and step-by-step
annotations, ensure comments accurately describe instruction semantics, data
direction (e.g., distinguishing loads from stores or register sources from
destinations), and operands. Use exact libyuv FOURCC names and memory layouts
(e.g., ARGB rather than RGBA, and RGB24 vs. RAW) for vector loads and stores,
and specify exact arithmetic operations or sample counts (e.g., '// average of
4' rather than vague '// average') for subsampling, shifts, and reductions.
Additionally, when documenting register states, clamp limits (such as min or max
bounds), or constant initializations, place each inline comment directly on the
specific instruction that initializes or computes that value (e.g., zero or
min-clamp comments on pxor/vpxor, and max-clamp comments on shift or mask
instructions) rather than attaching them to adjacent setup instructions.

**Why.** Inline assembly kernels rely heavily on step-by-step comments to
clarify register operations, memory layouts, and data flow. Inaccurate FOURCC
names cause confusion about channel ordering and vector shuffle masks, inverted
load/store descriptions mislead maintainers about register state, vague
arithmetic comments obscure subsampling factors, and placing register or clamp
documentation on adjacent instructions misleads readers about intermediate
calculation steps during review and debugging.

## Examples

Bad:

```cpp
// Variant 1: Inverted load/store annotations and vague arithmetic step
"vld1.16    {q1, q2}, [%1]                 \n"  // store pixels
"vst1.16    {q1, q2}, [%0]!                \n"  // load row
"ld4         {v0.16b,v1.16b,v2.16b,v3.16b}, [%0], #64 \n"  // load 16 RGBA
"urshr       v0.8h, v0.8h, #2                          \n"  // average

// Variant 2: Misaligned clamp/register initialization comments
"pxor        %%xmm6,%%xmm6                 \n"
"pcmpeqb     %%xmm7,%%xmm7                 \n"  // 0 for min
"psrlw       $6,%%xmm7                     \n"  // 1023 for max
```

Good:

```cpp
// Variant 1: Accurate data direction, FOURCC naming, and specific sample count
"vld1.16    {q1, q2}, [%1]                 \n"  // load source pixels
"vst1.16    {q1, q2}, [%0]!                \n"  // store row and post-inc
"ld4         {v0.16b,v1.16b,v2.16b,v3.16b}, [%0], #64 \n"  // load 16 ARGB
"urshr       v0.8h, v0.8h, #2                          \n"  // average of 4

// Variant 2: Comments aligned directly with the computing instruction
"pxor        %%xmm6,%%xmm6                 \n"  // 0 for min
"pcmpeqb     %%xmm7,%%xmm7                 \n"
"psrlw       $6,%%xmm7                     \n"  // 1023 for max
```
