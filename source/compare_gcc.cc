/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/compare_row.h"
#include "libyuv/row.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// This module is for GCC x86 and x64.
#if !defined(LIBYUV_DISABLE_X86) &&               \
    (defined(__x86_64__) || defined(__i386__)) && \
    !defined(LIBYUV_ENABLE_ROWWIN)

// "memory" clobber prevents the reads from being removed

#if defined(__x86_64__)
uint32_t HammingDistance_SSE42(const uint8_t* src_a,
                               const uint8_t* src_b,
                               int count) {
  uint64_t diff;

  asm volatile(
      "xor         %3,%3                         \n"
      "xor         %%r8,%%r8                     \n"
      "xor         %%r9,%%r9                     \n"
      "xor         %%r10,%%r10                   \n"

      // Process 32 bytes per loop.
      LABELALIGN
      "1:          \n"
      "mov         (%0),%%rcx                    \n"
      "mov         0x8(%0),%%rdx                 \n"
      "xor         (%1),%%rcx                    \n"
      "xor         0x8(%1),%%rdx                 \n"
      "popcnt      %%rcx,%%rcx                   \n"
      "popcnt      %%rdx,%%rdx                   \n"
      "mov         0x10(%0),%%rsi                \n"
      "mov         0x18(%0),%%rdi                \n"
      "xor         0x10(%1),%%rsi                \n"
      "xor         0x18(%1),%%rdi                \n"
      "popcnt      %%rsi,%%rsi                   \n"
      "popcnt      %%rdi,%%rdi                   \n"
      "add         $0x20,%0                      \n"
      "add         $0x20,%1                      \n"
      "add         %%rcx,%3                      \n"
      "add         %%rdx,%%r8                    \n"
      "add         %%rsi,%%r9                    \n"
      "add         %%rdi,%%r10                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"

      "add         %%r8, %3                      \n"
      "add         %%r9, %3                      \n"
      "add         %%r10, %3                     \n"
      : "+r"(src_a),  // %0
        "+r"(src_b),  // %1
        "+r"(count),  // %2
        "=&r"(diff)   // %3
      :
      : "cc", "memory", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10");

  return (uint32_t)(diff);
}
#else
uint32_t HammingDistance_SSE42(const uint8_t* src_a,
                               const uint8_t* src_b,
                               int count) {
  uint32_t diff = 0u;

  asm volatile(
      // Process 16 bytes per loop.
      LABELALIGN
      "1:          \n"
      "mov         (%0),%%ecx                    \n"
      "mov         0x4(%0),%%edx                 \n"
      "xor         (%1),%%ecx                    \n"
      "xor         0x4(%1),%%edx                 \n"
      "popcnt      %%ecx,%%ecx                   \n"
      "add         %%ecx,%3                      \n"
      "popcnt      %%edx,%%edx                   \n"
      "add         %%edx,%3                      \n"
      "mov         0x8(%0),%%ecx                 \n"
      "mov         0xc(%0),%%edx                 \n"
      "xor         0x8(%1),%%ecx                 \n"
      "xor         0xc(%1),%%edx                 \n"
      "popcnt      %%ecx,%%ecx                   \n"
      "add         %%ecx,%3                      \n"
      "popcnt      %%edx,%%edx                   \n"
      "add         %%edx,%3                      \n"
      "add         $0x10,%0                      \n"
      "add         $0x10,%1                      \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      : "+r"(src_a),  // %0
        "+r"(src_b),  // %1
        "+r"(count),  // %2
        "+r"(diff)    // %3
      :
      : "cc", "memory", "ecx", "edx");

  return diff;
}
#endif

static const vec8 kNibbleMask = {15, 15, 15, 15, 15, 15, 15, 15,
                                 15, 15, 15, 15, 15, 15, 15, 15};
static const vec8 kBitCount = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

uint32_t HammingDistance_SSSE3(const uint8_t* src_a,
                               const uint8_t* src_b,
                               int count) {
  uint32_t diff;

  asm volatile(
      "movdqa      %4,%%xmm2                     \n"
      "movdqa      %5,%%xmm3                     \n"
      "pxor        %%xmm0,%%xmm0                 \n"
      "pxor        %%xmm1,%%xmm1                 \n"
      "sub         %0,%1                         \n"

      LABELALIGN
      "1:          \n"
      "movdqu      (%0),%%xmm4                   \n"
      "movdqu      0x10(%0), %%xmm5              \n"
      "movdqu      (%0,%1), %%xmm6               \n"
      "pxor        %%xmm6, %%xmm4                \n"
      "movdqa      %%xmm4,%%xmm6                 \n"
      "pand        %%xmm2,%%xmm6                 \n"
      "psrlw       $0x4,%%xmm4                   \n"
      "movdqa      %%xmm3,%%xmm7                 \n"
      "pshufb      %%xmm6,%%xmm7                 \n"
      "pand        %%xmm2,%%xmm4                 \n"
      "movdqa      %%xmm3,%%xmm6                 \n"
      "pshufb      %%xmm4,%%xmm6                 \n"
      "paddb       %%xmm7,%%xmm6                 \n"
      "movdqu      0x10(%0,%1),%%xmm7            \n"
      "pxor        %%xmm7,%%xmm5                 \n"
      "add         $0x20,%0                      \n"
      "movdqa      %%xmm5,%%xmm4                 \n"
      "pand        %%xmm2,%%xmm5                 \n"
      "psrlw       $0x4,%%xmm4                   \n"
      "movdqa      %%xmm3,%%xmm7                 \n"
      "pshufb      %%xmm5,%%xmm7                 \n"
      "pand        %%xmm2,%%xmm4                 \n"
      "movdqa      %%xmm3,%%xmm5                 \n"
      "pshufb      %%xmm4,%%xmm5                 \n"
      "paddb       %%xmm7,%%xmm5                 \n"
      "paddb       %%xmm5,%%xmm6                 \n"
      "psadbw      %%xmm1,%%xmm6                 \n"
      "paddd       %%xmm6,%%xmm0                 \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"

      "pshufd      $0xaa,%%xmm0,%%xmm1           \n"
      "paddd       %%xmm1,%%xmm0                 \n"
      "movd        %%xmm0, %3                    \n"
      : "+r"(src_a),       // %0
        "+r"(src_b),       // %1
        "+r"(count),       // %2
        "=r"(diff)         // %3
      : "m"(kNibbleMask),  // %4
        "m"(kBitCount)     // %5
      : "cc", "memory", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");

  return diff;
}

#ifdef HAS_HAMMINGDISTANCE_AVX2
uint32_t HammingDistance_AVX2(const uint8_t* src_a,
                              const uint8_t* src_b,
                              int count) {
  uint32_t diff;

  asm volatile(
      "vbroadcastf128 %4,%%ymm2                  \n"
      "vbroadcastf128 %5,%%ymm3                  \n"
      "vpxor       %%ymm0,%%ymm0,%%ymm0          \n"
      "vpxor       %%ymm1,%%ymm1,%%ymm1          \n"
      "sub         %0,%1                         \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm4                   \n"
      "vmovdqu     0x20(%0), %%ymm5              \n"
      "vpxor       (%0,%1), %%ymm4, %%ymm4       \n"
      "vpand       %%ymm2,%%ymm4,%%ymm6          \n"
      "vpsrlw      $0x4,%%ymm4,%%ymm4            \n"
      "vpshufb     %%ymm6,%%ymm3,%%ymm6          \n"
      "vpand       %%ymm2,%%ymm4,%%ymm4          \n"
      "vpshufb     %%ymm4,%%ymm3,%%ymm4          \n"
      "vpaddb      %%ymm4,%%ymm6,%%ymm6          \n"
      "vpxor       0x20(%0,%1),%%ymm5,%%ymm4     \n"
      "add         $0x40,%0                      \n"
      "vpand       %%ymm2,%%ymm4,%%ymm5          \n"
      "vpsrlw      $0x4,%%ymm4,%%ymm4            \n"
      "vpshufb     %%ymm5,%%ymm3,%%ymm5          \n"
      "vpand       %%ymm2,%%ymm4,%%ymm4          \n"
      "vpshufb     %%ymm4,%%ymm3,%%ymm4          \n"
      "vpaddb      %%ymm5,%%ymm4,%%ymm4          \n"
      "vpaddb      %%ymm6,%%ymm4,%%ymm4          \n"
      "vpsadbw     %%ymm1,%%ymm4,%%ymm4          \n"
      "vpaddd      %%ymm0,%%ymm4,%%ymm0          \n"
      "sub         $0x40,%2                      \n"
      "jg          1b                            \n"

      "vpermq      $0xb1,%%ymm0,%%ymm1           \n"
      "vpaddd      %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xaa,%%ymm0,%%ymm1           \n"
      "vpaddd      %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovd       %%xmm0,%3                     \n"
      "vzeroupper  \n"
      : "+r"(src_a),       // %0
        "+r"(src_b),       // %1
        "+r"(count),       // %2
        "=r"(diff)         // %3
      : "m"(kNibbleMask),  // %4
        "m"(kBitCount)     // %5
      : "cc", "memory", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");

  return diff;
}
#endif  // HAS_HAMMINGDISTANCE_AVX2

uint32_t SumSquareError_SSE2(const uint8_t* src_a,
                             const uint8_t* src_b,
                             int count) {
  uint32_t sse;
  asm volatile(
      "pxor        %%xmm0,%%xmm0                 \n"
      "pxor        %%xmm5,%%xmm5                 \n"

      LABELALIGN
      "1:          \n"
      "movdqu      (%0),%%xmm1                   \n"
      "lea         0x10(%0),%0                   \n"
      "movdqu      (%1),%%xmm2                   \n"
      "lea         0x10(%1),%1                   \n"
      "movdqa      %%xmm1,%%xmm3                 \n"
      "psubusb     %%xmm2,%%xmm1                 \n"
      "psubusb     %%xmm3,%%xmm2                 \n"
      "por         %%xmm2,%%xmm1                 \n"
      "movdqa      %%xmm1,%%xmm2                 \n"
      "punpcklbw   %%xmm5,%%xmm1                 \n"
      "punpckhbw   %%xmm5,%%xmm2                 \n"
      "pmaddwd     %%xmm1,%%xmm1                 \n"
      "pmaddwd     %%xmm2,%%xmm2                 \n"
      "paddd       %%xmm1,%%xmm0                 \n"
      "paddd       %%xmm2,%%xmm0                 \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"

      "pshufd      $0xee,%%xmm0,%%xmm1           \n"
      "paddd       %%xmm1,%%xmm0                 \n"
      "pshufd      $0x1,%%xmm0,%%xmm1            \n"
      "paddd       %%xmm1,%%xmm0                 \n"
      "movd        %%xmm0,%3                     \n"
      : "+r"(src_a),  // %0
        "+r"(src_b),  // %1
        "+r"(count),  // %2
        "=r"(sse)     // %3
      :
      : "cc", "memory", "xmm0", "xmm1", "xmm2", "xmm3", "xmm5");
  return sse;
}

static const uvec32 kHash16x33 = {0x92d9e201, 0, 0, 0};  // 33 ^ 16
static const uvec32 kHashMul0 = {
    0x0c3525e1,  // 33 ^ 15
    0xa3476dc1,  // 33 ^ 14
    0x3b4039a1,  // 33 ^ 13
    0x4f5f0981,  // 33 ^ 12
};
static const uvec32 kHashMul1 = {
    0x30f35d61,  // 33 ^ 11
    0x855cb541,  // 33 ^ 10
    0x040a9121,  // 33 ^ 9
    0x747c7101,  // 33 ^ 8
};
static const uvec32 kHashMul2 = {
    0xec41d4e1,  // 33 ^ 7
    0x4cfa3cc1,  // 33 ^ 6
    0x025528a1,  // 33 ^ 5
    0x00121881,  // 33 ^ 4
};
static const uvec32 kHashMul3 = {
    0x00008c61,  // 33 ^ 3
    0x00000441,  // 33 ^ 2
    0x00000021,  // 33 ^ 1
    0x00000001,  // 33 ^ 0
};

uint32_t HashDjb2_SSE41(const uint8_t* src, int count, uint32_t seed) {
  uint32_t hash;
  asm volatile(
      "movd        %2,%%xmm0                     \n"
      "pxor        %%xmm7,%%xmm7                 \n"
      "movdqa      %4,%%xmm6                     \n"

      LABELALIGN
      "1:          \n"
      "movdqu      (%0),%%xmm1                   \n"
      "lea         0x10(%0),%0                   \n"
      "pmulld      %%xmm6,%%xmm0                 \n"
      "movdqa      %5,%%xmm5                     \n"
      "movdqa      %%xmm1,%%xmm2                 \n"
      "punpcklbw   %%xmm7,%%xmm2                 \n"
      "movdqa      %%xmm2,%%xmm3                 \n"
      "punpcklwd   %%xmm7,%%xmm3                 \n"
      "pmulld      %%xmm5,%%xmm3                 \n"
      "movdqa      %6,%%xmm5                     \n"
      "movdqa      %%xmm2,%%xmm4                 \n"
      "punpckhwd   %%xmm7,%%xmm4                 \n"
      "pmulld      %%xmm5,%%xmm4                 \n"
      "movdqa      %7,%%xmm5                     \n"
      "punpckhbw   %%xmm7,%%xmm1                 \n"
      "movdqa      %%xmm1,%%xmm2                 \n"
      "punpcklwd   %%xmm7,%%xmm2                 \n"
      "pmulld      %%xmm5,%%xmm2                 \n"
      "movdqa      %8,%%xmm5                     \n"
      "punpckhwd   %%xmm7,%%xmm1                 \n"
      "pmulld      %%xmm5,%%xmm1                 \n"
      "paddd       %%xmm4,%%xmm3                 \n"
      "paddd       %%xmm2,%%xmm1                 \n"
      "paddd       %%xmm3,%%xmm1                 \n"
      "pshufd      $0xe,%%xmm1,%%xmm2            \n"
      "paddd       %%xmm2,%%xmm1                 \n"
      "pshufd      $0x1,%%xmm1,%%xmm2            \n"
      "paddd       %%xmm2,%%xmm1                 \n"
      "paddd       %%xmm1,%%xmm0                 \n"
      "sub         $0x10,%1                      \n"
      "jg          1b                            \n"
      "movd        %%xmm0,%3                     \n"
      : "+r"(src),        // %0
        "+r"(count),      // %1
        "+rm"(seed),      // %2
        "=r"(hash)        // %3
      : "m"(kHash16x33),  // %4
        "m"(kHashMul0),   // %5
        "m"(kHashMul1),   // %6
        "m"(kHashMul2),   // %7
        "m"(kHashMul3)    // %8
      : "cc", "memory", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
  return hash;
}
#if defined(HAS_HAMMINGDISTANCE_AVX512BW) || \
    defined(HAS_SUMSQUAREERROR_AVX512BW) || defined(HAS_HASHDJB2_AVX512BW)
// Mask of the low n bytes, for n = 0 to 63.
static uint64_t LowBytesMask64(int n) {
  return n ? ~0ULL >> (64 - n) : 0ULL;
}
#endif

#ifdef HAS_HAMMINGDISTANCE_AVX512BW
// Process 64 bytes per loop and the remainder of 1 to 63 bytes with masked
// loads that read zeros, so any count is supported.
uint32_t HammingDistance_AVX512BW(const uint8_t* src_a,
                                  const uint8_t* src_b,
                                  int count) {
  uint32_t diff;
  const uint64_t mask = LowBytesMask64(count & 63);
  asm volatile(
      "vbroadcasti32x4 %[nibble],%%zmm2          \n"
      "vbroadcasti32x4 %[bitcount],%%zmm3        \n"
      "vpxord      %%zmm0,%%zmm0,%%zmm0          \n"
      "vpxord      %%zmm1,%%zmm1,%%zmm1          \n"
      "kmovq       %[mask],%%k1                  \n"
      "sub         %[src_a],%[src_b]             \n"
      "sub         $0x40,%[count]                \n"
      "jl          2f                            \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu8    (%[src_a]),%%zmm4             \n"
      "vpxorq      (%[src_a],%[src_b]),%%zmm4,%%zmm4 \n"
      "add         $0x40,%[src_a]                \n"
      "vpandq      %%zmm2,%%zmm4,%%zmm5          \n"
      "vpsrlw      $0x4,%%zmm4,%%zmm4            \n"
      "vpandq      %%zmm2,%%zmm4,%%zmm4          \n"
      "vpshufb     %%zmm5,%%zmm3,%%zmm5          \n"
      "vpshufb     %%zmm4,%%zmm3,%%zmm4          \n"
      "vpaddb      %%zmm5,%%zmm4,%%zmm4          \n"
      "vpsadbw     %%zmm1,%%zmm4,%%zmm4          \n"
      "vpaddq      %%zmm4,%%zmm0,%%zmm0          \n"
      "sub         $0x40,%[count]                \n"
      "jge         1b                            \n"

      // Remainder: 1 to 63 bytes, masked loads.
      "2:          \n"
      "add         $0x40,%[count]                \n"
      "je          3f                            \n"
      "vmovdqu8    (%[src_a]),%%zmm4%{%%k1%}%{z%} \n"
      "vmovdqu8    (%[src_a],%[src_b]),%%zmm5%{%%k1%}%{z%} \n"
      "vpxorq      %%zmm5,%%zmm4,%%zmm4          \n"
      "vpandq      %%zmm2,%%zmm4,%%zmm5          \n"
      "vpsrlw      $0x4,%%zmm4,%%zmm4            \n"
      "vpandq      %%zmm2,%%zmm4,%%zmm4          \n"
      "vpshufb     %%zmm5,%%zmm3,%%zmm5          \n"
      "vpshufb     %%zmm4,%%zmm3,%%zmm4          \n"
      "vpaddb      %%zmm5,%%zmm4,%%zmm4          \n"
      "vpsadbw     %%zmm1,%%zmm4,%%zmm4          \n"
      "vpaddq      %%zmm4,%%zmm0,%%zmm0          \n"

      // Add 8 qwords.
      "3:          \n"
      "vextracti64x4 $1,%%zmm0,%%ymm1            \n"
      "vpaddq      %%ymm1,%%ymm0,%%ymm0          \n"
      "vextracti128 $1,%%ymm0,%%xmm1             \n"
      "vpaddq      %%xmm1,%%xmm0,%%xmm0          \n"
      "vpshufd     $0xee,%%xmm0,%%xmm1           \n"
      "vpaddq      %%xmm1,%%xmm0,%%xmm0          \n"
      "vmovd       %%xmm0,%[diff]                \n"
      "vzeroupper  \n"
      : [src_a] "+r"(src_a), [src_b] "+r"(src_b), [count] "+r"(count),
        [diff] "=r"(diff)
      : [nibble] "m"(kNibbleMask), [bitcount] "m"(kBitCount), [mask] "m"(mask)
      : "cc", "memory", "k1", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
  return diff;
}
#endif  // HAS_HAMMINGDISTANCE_AVX512BW

#ifdef HAS_SUMSQUAREERROR_AVX512BW
// Process 64 bytes per loop and the remainder of 1 to 63 bytes with masked
// loads that read zeros, so any count is supported.
uint32_t SumSquareError_AVX512BW(const uint8_t* src_a,
                                 const uint8_t* src_b,
                                 int count) {
  uint32_t sse;
  const uint64_t mask = LowBytesMask64(count & 63);
  asm volatile(
      "vpxord      %%zmm0,%%zmm0,%%zmm0          \n"
      "vpxord      %%zmm5,%%zmm5,%%zmm5          \n"
      "kmovq       %[mask],%%k1                  \n"
      "sub         %[src_a],%[src_b]             \n"
      "sub         $0x40,%[count]                \n"
      "jl          2f                            \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu8    (%[src_a]),%%zmm1             \n"
      "vmovdqu8    (%[src_a],%[src_b]),%%zmm2    \n"
      "add         $0x40,%[src_a]                \n"
      "vpsubusb    %%zmm2,%%zmm1,%%zmm3          \n"  // abs difference trick
      "vpsubusb    %%zmm1,%%zmm2,%%zmm2          \n"
      "vporq       %%zmm3,%%zmm2,%%zmm1          \n"
      "vpunpcklbw  %%zmm5,%%zmm1,%%zmm2          \n"  // u16
      "vpunpckhbw  %%zmm5,%%zmm1,%%zmm1          \n"
      "vpmaddwd    %%zmm2,%%zmm2,%%zmm2          \n"  // square + hadd to u32
      "vpmaddwd    %%zmm1,%%zmm1,%%zmm1          \n"
      "vpaddd      %%zmm2,%%zmm0,%%zmm0          \n"
      "vpaddd      %%zmm1,%%zmm0,%%zmm0          \n"
      "sub         $0x40,%[count]                \n"
      "jge         1b                            \n"

      // Remainder: 1 to 63 bytes, masked loads.
      "2:          \n"
      "add         $0x40,%[count]                \n"
      "je          3f                            \n"
      "vmovdqu8    (%[src_a]),%%zmm1%{%%k1%}%{z%} \n"
      "vmovdqu8    (%[src_a],%[src_b]),%%zmm2%{%%k1%}%{z%} \n"
      "vpsubusb    %%zmm2,%%zmm1,%%zmm3          \n"
      "vpsubusb    %%zmm1,%%zmm2,%%zmm2          \n"
      "vporq       %%zmm3,%%zmm2,%%zmm1          \n"
      "vpunpcklbw  %%zmm5,%%zmm1,%%zmm2          \n"
      "vpunpckhbw  %%zmm5,%%zmm1,%%zmm1          \n"
      "vpmaddwd    %%zmm2,%%zmm2,%%zmm2          \n"
      "vpmaddwd    %%zmm1,%%zmm1,%%zmm1          \n"
      "vpaddd      %%zmm2,%%zmm0,%%zmm0          \n"
      "vpaddd      %%zmm1,%%zmm0,%%zmm0          \n"

      // Add 16 dwords.
      "3:          \n"
      "vextracti64x4 $1,%%zmm0,%%ymm1            \n"
      "vpaddd      %%ymm1,%%ymm0,%%ymm0          \n"
      "vextracti128 $1,%%ymm0,%%xmm1             \n"
      "vpaddd      %%xmm1,%%xmm0,%%xmm0          \n"
      "vpshufd     $0xee,%%xmm0,%%xmm1           \n"
      "vpaddd      %%xmm1,%%xmm0,%%xmm0          \n"
      "vpshufd     $0x1,%%xmm0,%%xmm1            \n"
      "vpaddd      %%xmm1,%%xmm0,%%xmm0          \n"
      "vmovd       %%xmm0,%[sse]                 \n"
      "vzeroupper  \n"
      : [src_a] "+r"(src_a), [src_b] "+r"(src_b), [count] "+r"(count),
        [sse] "=r"(sse)
      : [mask] "m"(mask)
      : "cc", "memory", "k1", "xmm0", "xmm1", "xmm2", "xmm3", "xmm5");
  return sse;
}
#endif  // HAS_SUMSQUAREERROR_AVX512BW

#ifdef HAS_HASHDJB2_AVX512BW
// 33 ^ (63 - i) for byte i of 64.
static const uint32_t kHashMul_AVX512BW[64] = {
    0xd8be0be1, 0xef4b93c1, 0x64579fa1, 0x6021af81,  // 33 ^ 63 .. 60
    0x40f94361, 0x193ddb41, 0x6d5ef721, 0x31dc1701,  // 33 ^ 59 .. 56
    0x0944bae1, 0xc23862c1, 0x15668ea1, 0xa38ebe81,  // 33 ^ 55 .. 52
    0x33807261, 0x301b2a41, 0x664e6621, 0x0319a601,  // 33 ^ 51 .. 48
    0xd18c69e1, 0xb8c631c1, 0x79f67da1, 0xae5ccd81,  // 33 ^ 47 .. 44
    0x0548a161, 0xc2197941, 0x7a3ed521, 0x13383501,  // 33 ^ 43 .. 40
    0x009518e1, 0xc1f500c1, 0xa1076ca1, 0xaf8bdc81,  // 33 ^ 39 .. 36
    0x0551d061, 0x3e38c841, 0x38304421, 0x1137c401,  // 33 ^ 35 .. 32
    0x655ec7e1, 0xccc4cfc1, 0x99995ba1, 0xd61beb81,  // 33 ^ 31 .. 28
    0x829bff61, 0x13791741, 0x2f22b321, 0xac185301,  // 33 ^ 27 .. 24
    0xcee976e1, 0xc8359ec1, 0x72ac4aa1, 0x510cfa81,  // 33 ^ 23 .. 20
    0xcc272e61, 0xb0da6641, 0xee162221, 0x92d9e201,  // 33 ^ 19 .. 16
    0x0c3525e1, 0xa3476dc1, 0x3b4039a1, 0x4f5f0981,  // 33 ^ 15 .. 12
    0x30f35d61, 0x855cb541, 0x040a9121, 0x747c7101,  // 33 ^ 11 .. 8
    0xec41d4e1, 0x4cfa3cc1, 0x025528a1, 0x00121881,  // 33 ^ 7 .. 4
    0x00008c61, 0x00000441, 0x00000021, 0x00000001,  // 33 ^ 3 .. 0
};
static const uint32_t kHash64x33 = 0xf07f8801;  // 33 ^ 64

static uint32_t Pow33(int n) {
  uint32_t r = 1u;
  uint32_t b = 33u;
  while (n) {
    if (n & 1) {
      r *= b;
    }
    b *= b;
    n >>= 1;
  }
  return r;
}

// hash = hash * 33 ^ 64 + sum(src[i] * 33 ^ (63 - i)) for each 64 bytes.
// The remainder of n = 1 to 63 bytes is loaded into the high n bytes with a
// masked load ending at src + count, so the same multipliers apply, and
// hash is multiplied by 33 ^ n.
#define HASH64_AVX512BW                          \
  "vpmulld     (%[mul]),%%zmm1,%%zmm1        \n" \
  "vpmulld     0x40(%[mul]),%%zmm2,%%zmm2    \n" \
  "vpmulld     0x80(%[mul]),%%zmm3,%%zmm3    \n" \
  "vpmulld     0xc0(%[mul]),%%zmm4,%%zmm4    \n" \
  "vpaddd      %%zmm2,%%zmm1,%%zmm1          \n" \
  "vpaddd      %%zmm4,%%zmm3,%%zmm3          \n" \
  "vpaddd      %%zmm3,%%zmm1,%%zmm1          \n" \
  "vextracti64x4 $1,%%zmm1,%%ymm2            \n" \
  "vpaddd      %%ymm2,%%ymm1,%%ymm1          \n" \
  "vextracti128 $1,%%ymm1,%%xmm2             \n" \
  "vpaddd      %%xmm2,%%xmm1,%%xmm1          \n" \
  "vpshufd     $0xee,%%xmm1,%%xmm2           \n" \
  "vpaddd      %%xmm2,%%xmm1,%%xmm1          \n" \
  "vpshufd     $0x1,%%xmm1,%%xmm2            \n" \
  "vpaddd      %%xmm2,%%xmm1,%%xmm1          \n" \
  "vpmulld     %%xmm6,%%xmm0,%%xmm0          \n" \
  "vpaddd      %%xmm1,%%xmm0,%%xmm0          \n"

uint32_t HashDjb2_AVX512BW(const uint8_t* src, int count, uint32_t seed) {
  const int n = count & 63;
  const uint64_t mask = n ? ~0ULL << (64 - n) : 0ULL;
  const uint32_t pow_n = Pow33(n);
  intptr_t width = count;
  asm volatile(
      "vmovd       %[seed],%%xmm0                \n"
      "vmovd       %[pow64],%%xmm6               \n"
      "kmovq       %[mask],%%k1                  \n"
      "sub         $0x40,%[width]                \n"
      "jl          2f                            \n"

      LABELALIGN
      "1:          \n"
      "vpmovzxbd   (%[src]),%%zmm1               \n"
      "vpmovzxbd   0x10(%[src]),%%zmm2           \n"
      "vpmovzxbd   0x20(%[src]),%%zmm3           \n"
      "vpmovzxbd   0x30(%[src]),%%zmm4           \n"
      "add         $0x40,%[src]                  \n"  //
      HASH64_AVX512BW
      "sub         $0x40,%[width]                \n"
      "jge         1b                            \n"

      // Remainder: 1 to 63 bytes, masked load into the high bytes.
      "2:          \n"
      "add         $0x40,%[width]                \n"
      "je          3f                            \n"
      "vmovdqu8    -0x40(%[src],%[width]),%%zmm5%{%%k1%}%{z%} \n"
      "vpmovzxbd   %%xmm5,%%zmm1                 \n"
      "vextracti32x4 $1,%%zmm5,%%xmm2            \n"
      "vpmovzxbd   %%xmm2,%%zmm2                 \n"
      "vextracti32x4 $2,%%zmm5,%%xmm3            \n"
      "vpmovzxbd   %%xmm3,%%zmm3                 \n"
      "vextracti32x4 $3,%%zmm5,%%xmm4            \n"
      "vpmovzxbd   %%xmm4,%%zmm4                 \n"
      "vmovd       %[pow_n],%%xmm6               \n"  //
      HASH64_AVX512BW

      "3:          \n"
      "vmovd       %%xmm0,%[seed]                \n"
      "vzeroupper  \n"
      : [src] "+r"(src), [width] "+r"(width), [seed] "+r"(seed)
      : [mul] "r"(kHashMul_AVX512BW), [pow64] "m"(kHash64x33),
        [pow_n] "m"(pow_n), [mask] "m"(mask)
      : "cc", "memory", "k1", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5",
        "xmm6");
  return seed;
}
#undef HASH64_AVX512BW
#endif  // HAS_HASHDJB2_AVX512BW

#endif  // !defined(LIBYUV_DISABLE_X86) && (x86 or x64) &&
        // !defined(LIBYUV_ENABLE_ROWWIN)

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
