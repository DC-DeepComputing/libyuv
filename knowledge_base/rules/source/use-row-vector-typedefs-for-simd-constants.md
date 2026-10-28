---
name: libyuv/source/use-row-vector-typedefs-for-simd-constants
description: When defining SIMD constant tables and shuffle masks, use row.h vector typedefs like uvec8 or uvec32 instead of alignas or SIMD_ALIGNED scalar arrays
rubric: safety
severity: should_fix
---

**Rule.** When defining SIMD constant lookup tables, coefficients, and shuffle
masks, use libyuv's predefined vector typedefs from row.h (such as uvec8,
uvec16, uvec32, vec8, vec16, vec32, lvec8, ulvec8, or vecf32) rather than raw
scalar arrays annotated with alignas or SIMD_ALIGNED. For larger tables (such as
64-byte AVX-512 constants) where vector typedefs are unavailable, declare
standard unannotated arrays and load them using unaligned SIMD instructions
(such as vmovdqu, vmovdqu64, or _mm512_loadu_si512).

**Why.** alignas requires C11/C++11 alignment support and is not universally
supported across legacy and embedded toolchains targeted by libyuv. Meanwhile,
SIMD_ALIGNED only enforces memory alignment without applying compiler
vector_size attributes; without vector_size, compilers cannot determine the
vector layout for inline assembly operands and may emit instructions copying
array constants onto the stack at runtime. Using libyuv's row.h vector typedefs
ensures proper vector attributes, avoids unnecessary stack copies, and maintains
portability across all supported toolchains.

## Examples

Bad:

```cpp
// Bad: SIMD_ALIGNED scalar array lacks vector_size attribute and may trigger compiler stack copies
SIMD_ALIGNED(static const uint32_t kDjb2Multiplicands[]) = {
    0x0c3525e1, 0xa3476dc1, 0x3b4039a1, 0x4f5f0981,
};

// Bad: alignas is not universally supported across all toolchains in libyuv
alignas(64) static const uint8_t kMaskBG[64] = { ... };
// asm: "vmovdqa64 %3, %%zmm5\n"
```

Good:

```cpp
// Good: row.h vector typedefs apply vector_size attributes to optimize inline assembly and constant usage
static const uvec32 kDjb2Multiplicands[] = {
    {0x0c3525e1, 0xa3476dc1, 0x3b4039a1, 0x4f5f0981},
};

static const uvec8 kShuf0 = {0, 1, 3, 4, 5, 7, 8, 9, 128, 128, 128, 128, 128, 128, 128, 128};

// Good: declare standard array and load using unaligned vector instructions for 64-byte tables
static const uint8_t kMaskBG[64] = { ... };
// asm: "vmovdqu64 %3, %%zmm5\n"
```
