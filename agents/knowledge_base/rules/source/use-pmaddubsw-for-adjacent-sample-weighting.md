---
name: libyuv/source/use-pmaddubsw-for-adjacent-sample-weighting
description: When computing pairwise weighted sums for 8-bit or 16-bit scaling in x86 SIMD assembly, use (V)PMADDUBSW or (V)PMADDWD instead of manual unpack, shift, and add sequences
rubric: safety
severity: nit
---

**Rule.** In x86 SIMD inline assembly or intrinsics for row scaling,
interpolation, and filtering where adjacent samples are weighted and summed
(e.g., linear or bilinear scaling):
- For 8-bit samples, use (V)PMADDUBSW (_mm_maddubs_epi16 / _mm256_maddubs_epi16) with signed/unsigned coefficient pairs.
- For 16-bit samples (such as 10-bit and 12-bit pixel formats), use (V)PMADDWD (_mm_madd_epi16 / _mm256_madd_epi16) with precomputed coefficient vectors.
Avoid manual unpack (punpck*), shift (psllw), and add (paddw) instruction sequences.

**Why.** (V)PMADDUBSW and (V)PMADDWD perform fused multiply-accumulate on
adjacent sample pairs (multiplying unsigned 8-bit samples by signed 8-bit
weights into 16-bit words, or pairs of 16-bit words into 32-bit dwords) in a
single instruction. This significantly reduces instruction count, register
pressure, and pipeline latency compared to multi-instruction unpack, shift, and
add sequences.

## Examples

Bad:

```cpp
// 8-bit: Manual unpacking, shifting, and adding to calculate 3*near + 1*far
"vpunpcklbw   %%ymm0,%%ymm1,%%ymm1          \n"
"vpunpcklwd   %%ymm1,%%ymm1,%%ymm2          \n"
"vpsllw       $1,%%ymm1,%%ymm1              \n"
"vpaddw       %%ymm1,%%ymm2,%%ymm4          \n"

// 16-bit: Manual shift-and-add for 3:1 weighting
"vpunpcklwd   %%ymm1,%%ymm1,%%ymm2          \n"
"vpsllw       $1,%%ymm1,%%ymm1              \n"
"vpaddw       %%ymm1,%%ymm2,%%ymm5          \n"
```

Good:

```cpp
// 8-bit: Interleaving adjacent samples and using vpmaddubsw with coefficients (e.g. 3, 1)
"vpunpckldq   %%ymm1,%%ymm0,%%ymm0          \n"
"vpmaddubsw   %%ymm7,%%ymm0,%%ymm0          \n"  // 3*near + 1*far in 16-bit

// 16-bit: Pairwise multiply-accumulate using vpmaddwd with coefficient vector
static const lvec16 kLinearMadd31_16_AVX2 = {3, 1, 1, 3, 3, 1, 1, 3,
                                             3, 1, 1, 3, 3, 1, 1, 3};
"vpmaddwd     %%ymm3,%%ymm1,%%ymm0          \n"  // 3*near + far (lo)
"vpmaddwd     %%ymm3,%%ymm2,%%ymm1          \n"  // 3*near + far (hi)
"vpackssdw    %%ymm1,%%ymm0,%%ymm0          \n"
```
