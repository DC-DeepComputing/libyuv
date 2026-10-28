---
name: libyuv/source/simd-assembly-post-increment-byte-offsets
description: When advancing pointers with post-increment addressing in SIMD assembly for multi-byte pixels, calculate all displacements in total bytes rather than element counts
rubric: correctness
severity: should_fix
---

**Rule.** When advancing source or destination pointers using post-increment
load or store instructions in SIMD inline assembly for multi-byte pixel formats
(such as 16-bit uint16_t or 32-bit formats), compute all intermediate step
offsets and loop-step displacements in total bytes rather than element counts.
Pass byte displacements as registers or byte-scaled immediate offsets so that
pointer advancement accounts for element width (e.g., 2N bytes for N 16-bit
elements).

**Why.** Assembly post-increment memory operations (such as ld1, ld2, st1)
advance address registers by byte counts. Confusing element counts with byte
offsets causes incorrect address calculations, resulting in misaligned reads,
wrong pixel subsampling, or out-of-bounds memory accesses.

## Examples

Bad:

```cpp
// Bad: Advancing pointers by element counts instead of byte offsets for uint16_t pixels (2 bytes/elem).
"ld2        {v0.4h, v1.4h}, [%0], #2       \n"
"ld2        {v4.4h, v5.4h}, [%0], #6       \n"  // 2 + 6 = 8 bytes instead of 16 bytes for 8 uint16 pixels
```

Good:

```cpp
// Good: Advance pointers using total byte displacements (e.g., 2 + 14 = 16 bytes for 8 uint16 pixels).
"ld2        {v0.4h, v1.4h}, [%0], %4       \n"
"ld2        {v4.4h, v5.4h}, [%0], %5       \n"
: ...
: "r"(2LL), "r"(14LL)
```
