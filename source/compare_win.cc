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

// This module is for Visual C 32/64 bit, and for clang with
// LIBYUV_ENABLE_ROWWIN, which is also enabled for MemorySanitizer.
#if !defined(LIBYUV_DISABLE_X86) &&                                 \
    (defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || \
     defined(_M_IX86)) &&                                           \
    ((defined(_MSC_VER) && !defined(__clang__)) ||                  \
     defined(LIBYUV_ENABLE_ROWWIN))

#include <immintrin.h>  // For AVX2 and AVX512 intrinsics

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// Target attributes allow gcc and clang to build each function for its
// instruction set regardless of the baseline.
#if defined(__clang__) || defined(__GNUC__)
#define LIBYUV_TARGET_AVX2 __attribute__((target("avx2")))
#define LIBYUV_TARGET_AVX512BW __attribute__((target("avx512bw,avx512f")))
#else
#define LIBYUV_TARGET_AVX2
#define LIBYUV_TARGET_AVX512BW
#endif  // defined(__clang__) || defined(__GNUC__)

#if defined(HAS_HAMMINGDISTANCE_AVX2)
// Count is a multiple of 64.
LIBYUV_TARGET_AVX2
uint32_t HammingDistance_AVX2(const uint8_t* src_a,
                              const uint8_t* src_b,
                              int count) {
  const __m256i kNibbleMask = _mm256_set1_epi8(15);
  const __m256i kBitCount = _mm256_broadcastsi128_si256(
      _mm_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4));
  const __m256i zero = _mm256_setzero_si256();
  __m256i sum = _mm256_setzero_si256();
  __m128i sum128;
  int i;
  for (i = 0; i < count; i += 64) {
    __m256i x0 =
        _mm256_xor_si256(_mm256_loadu_si256((const __m256i*)(src_a + i)),
                         _mm256_loadu_si256((const __m256i*)(src_b + i)));
    __m256i x1 =
        _mm256_xor_si256(_mm256_loadu_si256((const __m256i*)(src_a + i + 32)),
                         _mm256_loadu_si256((const __m256i*)(src_b + i + 32)));
    __m256i c0 = _mm256_add_epi8(
        _mm256_shuffle_epi8(kBitCount, _mm256_and_si256(x0, kNibbleMask)),
        _mm256_shuffle_epi8(
            kBitCount,
            _mm256_and_si256(_mm256_srli_epi16(x0, 4), kNibbleMask)));
    __m256i c1 = _mm256_add_epi8(
        _mm256_shuffle_epi8(kBitCount, _mm256_and_si256(x1, kNibbleMask)),
        _mm256_shuffle_epi8(
            kBitCount,
            _mm256_and_si256(_mm256_srli_epi16(x1, 4), kNibbleMask)));
    sum = _mm256_add_epi64(sum, _mm256_sad_epu8(_mm256_add_epi8(c0, c1), zero));
  }
  sum128 = _mm_add_epi64(_mm256_castsi256_si128(sum),
                         _mm256_extracti128_si256(sum, 1));
  sum128 = _mm_add_epi64(sum128, _mm_unpackhi_epi64(sum128, sum128));
  _mm256_zeroupper();
  return (uint32_t)_mm_cvtsi128_si32(sum128);
}
#endif  // HAS_HAMMINGDISTANCE_AVX2

#if defined(HAS_HAMMINGDISTANCE_AVX512BW) || \
    defined(HAS_SUMSQUAREERROR_AVX512BW) || defined(HAS_HASHDJB2_AVX512BW)
// Mask of the low n bytes, for n = 0 to 64.
static __mmask64 LowBytesMask64(int n) {
  return (__mmask64)(n >= 64 ? ~0ULL : (1ULL << n) - 1);
}
#endif

#if defined(HAS_HAMMINGDISTANCE_AVX512BW)
// Process 64 bytes per loop and the remainder of 1 to 63 bytes with masked
// loads that read zeros, so any count is supported.
LIBYUV_TARGET_AVX512BW
uint32_t HammingDistance_AVX512BW(const uint8_t* src_a,
                                  const uint8_t* src_b,
                                  int count) {
  const __m512i kNibbleMask = _mm512_set1_epi8(15);
  const __m512i kBitCount = _mm512_broadcast_i32x4(
      _mm_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4));
  const __m512i zero = _mm512_setzero_si512();
  __m512i sum = _mm512_setzero_si512();
  while (count > 0) {
    const __mmask64 k = LowBytesMask64(count);
    __m512i x = _mm512_xor_si512(_mm512_maskz_loadu_epi8(k, src_a),
                                 _mm512_maskz_loadu_epi8(k, src_b));
    __m512i lo =
        _mm512_shuffle_epi8(kBitCount, _mm512_and_si512(x, kNibbleMask));
    __m512i hi = _mm512_shuffle_epi8(
        kBitCount, _mm512_and_si512(_mm512_srli_epi16(x, 4), kNibbleMask));
    sum = _mm512_add_epi64(sum, _mm512_sad_epu8(_mm512_add_epi8(lo, hi), zero));
    src_a += 64;
    src_b += 64;
    count -= 64;
  }
  return (uint32_t)_mm512_reduce_add_epi64(sum);
}
#endif  // HAS_HAMMINGDISTANCE_AVX512BW

#if defined(HAS_SUMSQUAREERROR_AVX2)
// Count is a multiple of 32.
LIBYUV_TARGET_AVX2
uint32_t SumSquareError_AVX2(const uint8_t* src_a,
                             const uint8_t* src_b,
                             int count) {
  const __m256i zero = _mm256_setzero_si256();
  __m256i sum = _mm256_setzero_si256();
  __m128i sum128;
  int i;
  for (i = 0; i < count; i += 32) {
    __m256i a = _mm256_loadu_si256((const __m256i*)(src_a + i));
    __m256i b = _mm256_loadu_si256((const __m256i*)(src_b + i));
    __m256i d = _mm256_or_si256(_mm256_subs_epu8(a, b), _mm256_subs_epu8(b, a));
    __m256i lo = _mm256_unpacklo_epi8(d, zero);
    __m256i hi = _mm256_unpackhi_epi8(d, zero);
    sum = _mm256_add_epi32(sum, _mm256_madd_epi16(lo, lo));
    sum = _mm256_add_epi32(sum, _mm256_madd_epi16(hi, hi));
  }
  sum128 = _mm_add_epi32(_mm256_castsi256_si128(sum),
                         _mm256_extracti128_si256(sum, 1));
  sum128 = _mm_add_epi32(sum128, _mm_shuffle_epi32(sum128, 0xee));
  sum128 = _mm_add_epi32(sum128, _mm_shuffle_epi32(sum128, 0x01));
  _mm256_zeroupper();
  return (uint32_t)_mm_cvtsi128_si32(sum128);
}
#endif  // HAS_SUMSQUAREERROR_AVX2

#if defined(HAS_SUMSQUAREERROR_AVX512BW)
// Process 64 bytes per loop and the remainder of 1 to 63 bytes with masked
// loads that read zeros, so any count is supported.
LIBYUV_TARGET_AVX512BW
uint32_t SumSquareError_AVX512BW(const uint8_t* src_a,
                                 const uint8_t* src_b,
                                 int count) {
  const __m512i zero = _mm512_setzero_si512();
  __m512i sum = _mm512_setzero_si512();
  while (count > 0) {
    const __mmask64 k = LowBytesMask64(count);
    __m512i a = _mm512_maskz_loadu_epi8(k, src_a);
    __m512i b = _mm512_maskz_loadu_epi8(k, src_b);
    __m512i d = _mm512_or_si512(_mm512_subs_epu8(a, b), _mm512_subs_epu8(b, a));
    __m512i lo = _mm512_unpacklo_epi8(d, zero);
    __m512i hi = _mm512_unpackhi_epi8(d, zero);
    sum = _mm512_add_epi32(sum, _mm512_madd_epi16(lo, lo));
    sum = _mm512_add_epi32(sum, _mm512_madd_epi16(hi, hi));
    src_a += 64;
    src_b += 64;
    count -= 64;
  }
  return (uint32_t)_mm512_reduce_add_epi32(sum);
}
#endif  // HAS_SUMSQUAREERROR_AVX512BW

#if defined(HAS_HASHDJB2_AVX2) || defined(HAS_HASHDJB2_AVX512BW)
// 33 ^ (63 - i) for byte i of 64. The last 32 are used for 32 bytes.
static const uint32_t kHashMul[64] = {
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
static const uint32_t kHash32x33 = 0x1137c401;  // 33 ^ 32
#endif

#if defined(HAS_HASHDJB2_AVX2)
// Count is a multiple of 32.
LIBYUV_TARGET_AVX2
uint32_t HashDjb2_AVX2(const uint8_t* src, int count, uint32_t seed) {
  const __m256i mul0 = _mm256_loadu_si256((const __m256i*)(kHashMul + 32));
  const __m256i mul1 = _mm256_loadu_si256((const __m256i*)(kHashMul + 40));
  const __m256i mul2 = _mm256_loadu_si256((const __m256i*)(kHashMul + 48));
  const __m256i mul3 = _mm256_loadu_si256((const __m256i*)(kHashMul + 56));
  int i;
  for (i = 0; i < count; i += 32) {
    __m256i s0 =
        _mm256_cvtepu8_epi32(_mm_loadl_epi64((const __m128i*)(src + i)));
    __m256i s1 =
        _mm256_cvtepu8_epi32(_mm_loadl_epi64((const __m128i*)(src + i + 8)));
    __m256i s2 =
        _mm256_cvtepu8_epi32(_mm_loadl_epi64((const __m128i*)(src + i + 16)));
    __m256i s3 =
        _mm256_cvtepu8_epi32(_mm_loadl_epi64((const __m128i*)(src + i + 24)));
    __m256i sum =
        _mm256_add_epi32(_mm256_add_epi32(_mm256_mullo_epi32(s0, mul0),
                                          _mm256_mullo_epi32(s1, mul1)),
                         _mm256_add_epi32(_mm256_mullo_epi32(s2, mul2),
                                          _mm256_mullo_epi32(s3, mul3)));
    __m128i sum128 = _mm_add_epi32(_mm256_castsi256_si128(sum),
                                   _mm256_extracti128_si256(sum, 1));
    sum128 = _mm_add_epi32(sum128, _mm_shuffle_epi32(sum128, 0x0e));
    sum128 = _mm_add_epi32(sum128, _mm_shuffle_epi32(sum128, 0x01));
    seed = seed * kHash32x33 + (uint32_t)_mm_cvtsi128_si32(sum128);
  }
  _mm256_zeroupper();
  return seed;
}
#endif  // HAS_HASHDJB2_AVX2

#if defined(HAS_HASHDJB2_AVX512BW)
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

// Sum of 64 bytes times 33 ^ (63 - i).
LIBYUV_TARGET_AVX512BW
static uint32_t HashSum64_AVX512BW(__m512i s) {
  __m512i p0 =
      _mm512_mullo_epi32(_mm512_cvtepu8_epi32(_mm512_castsi512_si128(s)),
                         _mm512_loadu_si512(kHashMul));
  __m512i p1 =
      _mm512_mullo_epi32(_mm512_cvtepu8_epi32(_mm512_extracti32x4_epi32(s, 1)),
                         _mm512_loadu_si512(kHashMul + 16));
  __m512i p2 =
      _mm512_mullo_epi32(_mm512_cvtepu8_epi32(_mm512_extracti32x4_epi32(s, 2)),
                         _mm512_loadu_si512(kHashMul + 32));
  __m512i p3 =
      _mm512_mullo_epi32(_mm512_cvtepu8_epi32(_mm512_extracti32x4_epi32(s, 3)),
                         _mm512_loadu_si512(kHashMul + 48));
  return (uint32_t)_mm512_reduce_add_epi32(
      _mm512_add_epi32(_mm512_add_epi32(p0, p1), _mm512_add_epi32(p2, p3)));
}

// hash = hash * 33 ^ 64 + sum(src[i] * 33 ^ (63 - i)) for each 64 bytes.
// The remainder of n = 1 to 63 bytes is loaded into the high n bytes with a
// masked load ending at src + count, so the same multipliers apply, and
// hash is multiplied by 33 ^ n.
LIBYUV_TARGET_AVX512BW
uint32_t HashDjb2_AVX512BW(const uint8_t* src, int count, uint32_t seed) {
  const uint32_t kHash64x33 = 0xf07f8801;  // 33 ^ 64
  int n;
  while (count >= 64) {
    seed = seed * kHash64x33 + HashSum64_AVX512BW(_mm512_loadu_si512(src));
    src += 64;
    count -= 64;
  }
  n = count;
  if (n) {
    const __mmask64 k = (__mmask64)~LowBytesMask64(64 - n);
    seed = seed * Pow33(n) +
           HashSum64_AVX512BW(_mm512_maskz_loadu_epi8(k, src + n - 64));
  }
  return seed;
}
#endif  // HAS_HASHDJB2_AVX512BW

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // !defined(LIBYUV_DISABLE_X86) && (x86 or x64) &&
        // (Visual C or LIBYUV_ENABLE_ROWWIN)
