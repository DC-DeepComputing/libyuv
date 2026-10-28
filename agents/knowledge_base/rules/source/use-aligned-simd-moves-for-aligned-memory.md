---
name: libyuv/source/use-aligned-simd-moves-for-aligned-memory
description: In x86 SIMD assembly routines, use aligned move instructions like movdqa or vmovdqa instead of unaligned moves for aligned memory and vector constants
rubric: readability
severity: should_fix
---

**Rule.** In x86 SIMD assembly routines and intrinsics (SSE, AVX, AVX2,
AVX-512), use aligned move instructions and intrinsics (such as movdqa, vmovdqa,
movaps, vmovaps, vmovdqa32, vmovdqa64, or _mm_load_si128/_mm256_load_si256)
rather than unaligned moves (movdqu, vmovdqu, movups, vmovups, or
_mm_loadu_si128/_mm256_loadu_si256) when loading or storing memory and static
constant tables (such as uvec8, lvec8, or SIMD_ALIGNED structures) that are
guaranteed to be aligned. Reserve unaligned moves for source and destination
buffer pointers that may not satisfy vector alignment.

**Why.** Static vector constants, shuffle masks, and aligned scratch buffers are
declared with 16-byte or 32-byte alignment invariants. Using aligned move
instructions on explicitly aligned memory documents and enforces alignment
invariants in assembly code, avoids unaligned load penalties on older x86 CPU
architectures, and maintains consistency across libyuv SIMD assembly routines.

## Examples

Bad:

```cpp
// Bad: Loading 16-byte aligned static constant mask using unaligned move
"movdqu      %5,%%xmm6                     \n"  // %[kShuf0]

// Bad: Uses unaligned move for explicitly aligned constant mask or buffer
alignas(32) static const uint8_t kShuffleMask[32] = { ... };
"vmovdqu     %0,%%ymm5                     \n"
```

Good:

```cpp
// Good: Loading 16-byte aligned static constant mask using aligned move
"movdqa      %5,%%xmm6                     \n"  // %[kShuf0]

// Good: Uses aligned move instruction for explicitly aligned constant mask or buffer
alignas(32) static const uint8_t kShuffleMask[32] = { ... };
"vmovdqa     %0,%%ymm5                     \n"
```
