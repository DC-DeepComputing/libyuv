/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/row.h"
#include "libyuv/convert_from_argb.h"  // For ArgbConstants

// This module is for Visual C 32/64 bit
#if !defined(LIBYUV_DISABLE_X86) && \
    (defined(__x86_64__) || defined(__i386__) || \
     defined(_M_X64) || defined(_M_X86))

#include <emmintrin.h>
#include <tmmintrin.h>  // For _mm_maddubs_epi16
#include <immintrin.h>  // For AVX2 intrinsics

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

#if defined(__clang__) || defined(__GNUC__)
#define LIBYUV_TARGET_AVX2 __attribute__((target("avx2")))
#define LIBYUV_TARGET_AVX512BW __attribute__((target("avx512bw,avx512vl,avx512f")))
#else
#define LIBYUV_TARGET_AVX2
#define LIBYUV_TARGET_AVX512BW
#endif

#ifdef HAS_MERGEUVROW_AVX2
LIBYUV_TARGET_AVX2
void MergeUVRow_AVX2(const uint8_t* src_u,
                     const uint8_t* src_v,
                     uint8_t* dst_uv,
                     int width) {
  while (width >= 16) {
    __m128i u = _mm_loadu_si128((const __m128i*)src_u);
    __m128i v = _mm_loadu_si128((const __m128i*)src_v);
    __m256i y_u = _mm256_cvtepu8_epi16(u);
    __m256i y_v = _mm256_cvtepu8_epi16(v);
    y_v = _mm256_slli_epi16(y_v, 8);
    __m256i uv = _mm256_or_si256(y_u, y_v);
    _mm256_storeu_si256((__m256i*)dst_uv, uv);
    src_u += 16;
    src_v += 16;
    dst_uv += 32;
    width -= 16;
  }
}
#endif

#ifdef HAS_HALFMERGEUVROW_AVX2
LIBYUV_TARGET_AVX2
void HalfMergeUVRow_AVX2(const uint8_t* src_u,
                         int src_stride_u,
                         const uint8_t* src_v,
                         int src_stride_v,
                         uint8_t* dst_uv,
                         int width) {
  __m256i ones = _mm256_set1_epi8(1);
  __m256i zero = _mm256_setzero_si256();
  while (width >= 32) {
    __m256i u0 = _mm256_loadu_si256((const __m256i*)src_u);
    __m256i v0 = _mm256_loadu_si256((const __m256i*)src_v);
    __m256i u1 = _mm256_loadu_si256((const __m256i*)(src_u + src_stride_u));
    __m256i v1 = _mm256_loadu_si256((const __m256i*)(src_v + src_stride_v));
    __m256i sum_u0 = _mm256_maddubs_epi16(u0, ones);
    __m256i sum_v0 = _mm256_maddubs_epi16(v0, ones);
    __m256i sum_u1 = _mm256_maddubs_epi16(u1, ones);
    __m256i sum_v1 = _mm256_maddubs_epi16(v1, ones);
    __m256i sum_u = _mm256_add_epi16(sum_u0, sum_u1);
    __m256i sum_v = _mm256_add_epi16(sum_v0, sum_v1);
    sum_u = _mm256_srli_epi16(sum_u, 1);
    sum_v = _mm256_srli_epi16(sum_v, 1);
    sum_u = _mm256_avg_epu16(sum_u, zero);
    sum_v = _mm256_avg_epu16(sum_v, zero);
    __m256i pack_u = _mm256_packus_epi16(sum_u, sum_u);
    __m256i pack_v = _mm256_packus_epi16(sum_v, sum_v);
    __m256i uv = _mm256_unpacklo_epi8(pack_u, pack_v);
    _mm256_storeu_si256((__m256i*)dst_uv, uv);
    src_u += 32;
    src_v += 32;
    dst_uv += 32;
    width -= 32;
  }
}
#endif

#ifdef HAS_RAWTORGB24ROW_AVX2
LIBYUV_TARGET_AVX2
void RAWToRGB24Row_AVX2(const uint8_t* src_raw, uint8_t* dst_rgb24, int width) {
  __m256i shuf0 = _mm256_setr_epi8(
      2, 1, 0, 5, 4, 3, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1,
      2, 1, 0, 5, 4, 3, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1);
  __m256i shuf1 = _mm256_setr_epi8(
      2, 7, 6, 5, 10, 9, 8, 13, -1, -1, -1, -1, -1, -1, -1, -1,
      2, 7, 6, 5, 10, 9, 8, 13, -1, -1, -1, -1, -1, -1, -1, -1);
  __m256i shuf2 = _mm256_setr_epi8(
      8, 7, 12, 11, 10, 15, 14, 13, -1, -1, -1, -1, -1, -1, -1, -1,
      8, 7, 12, 11, 10, 15, 14, 13, -1, -1, -1, -1, -1, -1, -1, -1);

  while (width >= 16) {
    __m256i ymm0 = _mm256_castsi128_si256(_mm_loadu_si128((const __m128i*)src_raw));
    ymm0 = _mm256_inserti128_si256(ymm0, _mm_loadu_si128((const __m128i*)(src_raw + 24)), 1);

    __m256i ymm1 = _mm256_castsi128_si256(_mm_loadu_si128((const __m128i*)(src_raw + 4)));
    ymm1 = _mm256_inserti128_si256(ymm1, _mm_loadu_si128((const __m128i*)(src_raw + 28)), 1);

    __m256i ymm2 = _mm256_castsi128_si256(_mm_loadu_si128((const __m128i*)(src_raw + 8)));
    ymm2 = _mm256_inserti128_si256(ymm2, _mm_loadu_si128((const __m128i*)(src_raw + 32)), 1);

    ymm0 = _mm256_shuffle_epi8(ymm0, shuf0);
    ymm1 = _mm256_shuffle_epi8(ymm1, shuf1);
    ymm2 = _mm256_shuffle_epi8(ymm2, shuf2);

    _mm_storel_epi64((__m128i*)dst_rgb24, _mm256_castsi256_si128(ymm0));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 8), _mm256_castsi256_si128(ymm1));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 16), _mm256_castsi256_si128(ymm2));

    _mm_storel_epi64((__m128i*)(dst_rgb24 + 24), _mm256_extracti128_si256(ymm0, 1));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 32), _mm256_extracti128_si256(ymm1, 1));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 40), _mm256_extracti128_si256(ymm2, 1));

    src_raw += 48;
    dst_rgb24 += 48;
    width -= 16;
  }
}
#endif

#ifdef HAS_RAWTOARGBROW_AVX2
LIBYUV_TARGET_AVX2
void RAWToARGBRow_AVX2(const uint8_t* src_raw, uint8_t* dst_argb, int width) {
  __m256i ymm_alpha = _mm256_set1_epi32(0xff000000);
  __m128i shuf = _mm_setr_epi8(2, 1, 0, -1, 5, 4, 3, -1, 8, 7, 6, -1, 11, 10, 9, -1);
  __m256i ymm_shuf = _mm256_broadcastsi128_si256(shuf);

  while (width >= 8) {
    __m256i ymm0 = _mm256_castsi128_si256(_mm_loadu_si128((const __m128i*)src_raw));
    ymm0 = _mm256_inserti128_si256(ymm0, _mm_loadu_si128((const __m128i*)(src_raw + 12)), 1);

    ymm0 = _mm256_shuffle_epi8(ymm0, ymm_shuf);
    ymm0 = _mm256_or_si256(ymm0, ymm_alpha);
    _mm256_storeu_si256((__m256i*)dst_argb, ymm0);

    src_raw += 24;
    dst_argb += 32;
    width -= 8;
  }
}
#endif

#ifdef HAS_RGB24TOARGBROW_AVX2
LIBYUV_TARGET_AVX2
void RGB24ToARGBRow_AVX2(const uint8_t* src_rgb24, uint8_t* dst_argb, int width) {
  __m256i ymm_alpha = _mm256_set1_epi32(0xff000000);
  __m128i shuf = _mm_setr_epi8(0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11, -1);
  __m256i ymm_shuf = _mm256_broadcastsi128_si256(shuf);

  while (width >= 8) {
    __m256i ymm0 = _mm256_castsi128_si256(_mm_loadu_si128((const __m128i*)src_rgb24));
    ymm0 = _mm256_inserti128_si256(ymm0, _mm_loadu_si128((const __m128i*)(src_rgb24 + 12)), 1);

    ymm0 = _mm256_shuffle_epi8(ymm0, ymm_shuf);
    ymm0 = _mm256_or_si256(ymm0, ymm_alpha);
    _mm256_storeu_si256((__m256i*)dst_argb, ymm0);

    src_rgb24 += 24;
    dst_argb += 32;
    width -= 8;
  }
}
#endif

#ifdef HAS_RAWTORGBAROW_AVX2
LIBYUV_TARGET_AVX2
void RAWToRGBARow_AVX2(const uint8_t* src_raw, uint8_t* dst_rgba, int width) {
  __m256i ymm_alpha = _mm256_set1_epi32(0x000000ff);
  __m128i shuf = _mm_setr_epi8(-1, 2, 1, 0, -1, 5, 4, 3, -1, 8, 7, 6, -1, 11, 10, 9);
  __m256i ymm_shuf = _mm256_broadcastsi128_si256(shuf);

  while (width >= 8) {
    __m256i ymm0 = _mm256_castsi128_si256(_mm_loadu_si128((const __m128i*)src_raw));
    ymm0 = _mm256_inserti128_si256(ymm0, _mm_loadu_si128((const __m128i*)(src_raw + 12)), 1);

    ymm0 = _mm256_shuffle_epi8(ymm0, ymm_shuf);
    ymm0 = _mm256_or_si256(ymm0, ymm_alpha);
    _mm256_storeu_si256((__m256i*)dst_rgba, ymm0);

    src_raw += 24;
    dst_rgba += 32;
    width -= 8;
  }
}
#endif

#ifdef HAS_RAWTORGB24ROW_AVX512BW
LIBYUV_TARGET_AVX512BW
void RAWToRGB24Row_AVX512BW(const uint8_t* src_raw, uint8_t* dst_rgb24, int width) {
  __m128i shuf_128_0 = _mm_setr_epi8(
      2, 1, 0, 5, 4, 3, 8, 7, 10, 15, 14, 13, -1, -1, -1, -1);
  __m128i shuf_128_1 = _mm_setr_epi8(
      2, 7, 6, 5, 10, 9, 8, 13, 12, 17, 16, 21, -1, -1, -1, -1);
  __m128i shuf_128_2 = _mm_setr_epi8(
      8, 7, 12, 11, 10, 15, 14, 13, 18, 23, 22, 21, -1, -1, -1, -1);

  __m512i shuf0 = _mm512_broadcast_i32x4(shuf_128_0);
  __m512i shuf1 = _mm512_broadcast_i32x4(shuf_128_1);
  __m512i shuf2 = _mm512_broadcast_i32x4(shuf_128_2);

  while (width >= 32) {
    __m128i x0 = _mm_loadu_si128((const __m128i*)src_raw);
    __m128i x1 = _mm_loadu_si128((const __m128i*)(src_raw + 24));
    __m128i x2 = _mm_loadu_si128((const __m128i*)(src_raw + 48));
    __m128i x3 = _mm_loadu_si128((const __m128i*)(src_raw + 72));
    __m512i zmm0 = _mm512_castsi128_si512(x0);
    zmm0 = _mm512_inserti32x4(zmm0, x1, 1);
    zmm0 = _mm512_inserti32x4(zmm0, x2, 2);
    zmm0 = _mm512_inserti32x4(zmm0, x3, 3);

    x0 = _mm_loadu_si128((const __m128i*)(src_raw + 4));
    x1 = _mm_loadu_si128((const __m128i*)(src_raw + 28));
    x2 = _mm_loadu_si128((const __m128i*)(src_raw + 52));
    x3 = _mm_loadu_si128((const __m128i*)(src_raw + 76));
    __m512i zmm1 = _mm512_castsi128_si512(x0);
    zmm1 = _mm512_inserti32x4(zmm1, x1, 1);
    zmm1 = _mm512_inserti32x4(zmm1, x2, 2);
    zmm1 = _mm512_inserti32x4(zmm1, x3, 3);

    x0 = _mm_loadu_si128((const __m128i*)(src_raw + 8));
    x1 = _mm_loadu_si128((const __m128i*)(src_raw + 32));
    x2 = _mm_loadu_si128((const __m128i*)(src_raw + 56));
    x3 = _mm_loadu_si128((const __m128i*)(src_raw + 80));
    __m512i zmm2 = _mm512_castsi128_si512(x0);
    zmm2 = _mm512_inserti32x4(zmm2, x1, 1);
    zmm2 = _mm512_inserti32x4(zmm2, x2, 2);
    zmm2 = _mm512_inserti32x4(zmm2, x3, 3);

    zmm0 = _mm512_shuffle_epi8(zmm0, shuf0);
    zmm1 = _mm512_shuffle_epi8(zmm1, shuf1);
    zmm2 = _mm512_shuffle_epi8(zmm2, shuf2);

    _mm_storel_epi64((__m128i*)dst_rgb24, _mm512_castsi512_si128(zmm0));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 8), _mm512_castsi512_si128(zmm1));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 16), _mm512_castsi512_si128(zmm2));

    _mm_storel_epi64((__m128i*)(dst_rgb24 + 24), _mm512_extracti32x4_epi32(zmm0, 1));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 32), _mm512_extracti32x4_epi32(zmm1, 1));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 40), _mm512_extracti32x4_epi32(zmm2, 1));

    _mm_storel_epi64((__m128i*)(dst_rgb24 + 48), _mm512_extracti32x4_epi32(zmm0, 2));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 56), _mm512_extracti32x4_epi32(zmm1, 2));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 64), _mm512_extracti32x4_epi32(zmm2, 2));

    _mm_storel_epi64((__m128i*)(dst_rgb24 + 72), _mm512_extracti32x4_epi32(zmm0, 3));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 80), _mm512_extracti32x4_epi32(zmm1, 3));
    _mm_storel_epi64((__m128i*)(dst_rgb24 + 88), _mm512_extracti32x4_epi32(zmm2, 3));

    src_raw += 96;
    dst_rgb24 += 96;
    width -= 32;
  }
}
#endif

#if defined(HAS_RAWTOARGBROW_AVX512BW) || defined(HAS_RAWTORGBAROW_AVX512BW) || defined(HAS_RGB24TOARGBROW_AVX512BW)
LIBYUV_TARGET_AVX512BW
static void RGBToARGBRow_AVX512BW(const uint8_t* src_raw, uint8_t* dst_argb, const __m128i* shuffler, uint32_t alpha_mask, int width) {
  __m512i zmm_alpha = _mm512_set1_epi32(alpha_mask);
  __m512i zmm_shuf = _mm512_broadcast_i32x4(_mm_loadu_si128(shuffler));

  while (width >= 16) {
    __m128i x0 = _mm_loadu_si128((const __m128i*)src_raw);
    __m128i x1 = _mm_loadu_si128((const __m128i*)(src_raw + 12));
    __m128i x2 = _mm_loadu_si128((const __m128i*)(src_raw + 24));
    __m128i x3 = _mm_loadu_si128((const __m128i*)(src_raw + 36));
    __m512i zmm0 = _mm512_castsi128_si512(x0);
    zmm0 = _mm512_inserti32x4(zmm0, x1, 1);
    zmm0 = _mm512_inserti32x4(zmm0, x2, 2);
    zmm0 = _mm512_inserti32x4(zmm0, x3, 3);

    zmm0 = _mm512_shuffle_epi8(zmm0, zmm_shuf);
    zmm0 = _mm512_or_si512(zmm0, zmm_alpha);
    _mm512_storeu_si512((__m512i*)dst_argb, zmm0);
    src_raw += 48;
    dst_argb += 64;
    width -= 16;
  }
}

#ifdef HAS_RAWTOARGBROW_AVX512BW
LIBYUV_TARGET_AVX512BW
void RAWToARGBRow_AVX512BW(const uint8_t* src_raw, uint8_t* dst_argb, int width) {
  __m128i shuf = _mm_setr_epi8(2, 1, 0, -1, 5, 4, 3, -1, 8, 7, 6, -1, 11, 10, 9, -1);
  RGBToARGBRow_AVX512BW(src_raw, dst_argb, &shuf, 0xff000000, width);
}
#endif

#ifdef HAS_RGB24TOARGBROW_AVX512BW
LIBYUV_TARGET_AVX512BW
void RGB24ToARGBRow_AVX512BW(const uint8_t* src_rgb24, uint8_t* dst_argb, int width) {
  __m128i shuf = _mm_setr_epi8(0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11, -1);
  RGBToARGBRow_AVX512BW(src_rgb24, dst_argb, &shuf, 0xff000000, width);
}
#endif

#ifdef HAS_RAWTORGBAROW_AVX512BW
LIBYUV_TARGET_AVX512BW
void RAWToRGBARow_AVX512BW(const uint8_t* src_raw, uint8_t* dst_rgba, int width) {
  __m128i shuf = _mm_setr_epi8(-1, 2, 1, 0, -1, 5, 4, 3, -1, 8, 7, 6, -1, 11, 10, 9);
  RGBToARGBRow_AVX512BW(src_raw, dst_rgba, &shuf, 0x000000ff, width);
}
#endif
#endif

#ifdef HAS_MERGEUVROW_AVX512BW
LIBYUV_TARGET_AVX512BW
void MergeUVRow_AVX512BW(const uint8_t* src_u,
                         const uint8_t* src_v,
                         uint8_t* dst_uv,
                         int width) {
  while (width >= 32) {
    __m256i u = _mm256_loadu_si256((const __m256i*)src_u);
    __m256i v = _mm256_loadu_si256((const __m256i*)src_v);
    __m512i z_u = _mm512_cvtepu8_epi16(u);
    __m512i z_v = _mm512_cvtepu8_epi16(v);
    z_v = _mm512_slli_epi16(z_v, 8);
    __m512i uv = _mm512_or_si512(z_u, z_v);
    _mm512_storeu_si512((__m512i*)dst_uv, uv);
    src_u += 32;
    src_v += 32;
    dst_uv += 64;
    width -= 32;
  }
}
#endif

#if ((defined(_MSC_VER) && !defined(__clang__)) || defined(LIBYUV_ENABLE_ROWWIN))

// Read 8 UV from 444
#define READYUV444                                    \
  xmm3 = _mm_loadl_epi64((__m128i*)u_buf);            \
  xmm1 = _mm_loadl_epi64((__m128i*)(u_buf + offset)); \
  xmm3 = _mm_unpacklo_epi8(xmm3, xmm1);               \
  u_buf += 8;                                         \
  xmm4 = _mm_loadl_epi64((__m128i*)y_buf);            \
  xmm4 = _mm_unpacklo_epi8(xmm4, xmm4);               \
  y_buf += 8;

// Read 8 UV from 444, With 8 Alpha.
#define READYUVA444                                   \
  xmm3 = _mm_loadl_epi64((__m128i*)u_buf);            \
  xmm1 = _mm_loadl_epi64((__m128i*)(u_buf + offset)); \
  xmm3 = _mm_unpacklo_epi8(xmm3, xmm1);               \
  u_buf += 8;                                         \
  xmm4 = _mm_loadl_epi64((__m128i*)y_buf);            \
  xmm4 = _mm_unpacklo_epi8(xmm4, xmm4);               \
  y_buf += 8;                                         \
  xmm5 = _mm_loadl_epi64((__m128i*)a_buf);            \
  a_buf += 8;

// Read 4 UV from 422, upsample to 8 UV.
#define READYUV422                                        \
  xmm3 = _mm_cvtsi32_si128(*(uint32_t*)u_buf);            \
  xmm1 = _mm_cvtsi32_si128(*(uint32_t*)(u_buf + offset)); \
  xmm3 = _mm_unpacklo_epi8(xmm3, xmm1);                   \
  xmm3 = _mm_unpacklo_epi16(xmm3, xmm3);                  \
  u_buf += 4;                                             \
  xmm4 = _mm_loadl_epi64((__m128i*)y_buf);                \
  xmm4 = _mm_unpacklo_epi8(xmm4, xmm4);                   \
  y_buf += 8;

// Read 4 UV from 422, upsample to 8 UV.  With 8 Alpha.
#define READYUVA422                                       \
  xmm3 = _mm_cvtsi32_si128(*(uint32_t*)u_buf);            \
  xmm1 = _mm_cvtsi32_si128(*(uint32_t*)(u_buf + offset)); \
  xmm3 = _mm_unpacklo_epi8(xmm3, xmm1);                   \
  xmm3 = _mm_unpacklo_epi16(xmm3, xmm3);                  \
  u_buf += 4;                                             \
  xmm4 = _mm_loadl_epi64((__m128i*)y_buf);                \
  xmm4 = _mm_unpacklo_epi8(xmm4, xmm4);                   \
  y_buf += 8;                                             \
  xmm5 = _mm_loadl_epi64((__m128i*)a_buf);                \
  a_buf += 8;

// Convert 8 pixels: 8 UV and 8 Y.
#define YUVTORGB(yuvconstants)                                      \
  xmm3 = _mm_sub_epi8(xmm3, _mm_set1_epi8((char)0x80));             \
  xmm4 = _mm_mulhi_epu16(xmm4, *(__m128i*)yuvconstants->kYToRgb);   \
  xmm4 = _mm_add_epi16(xmm4, *(__m128i*)yuvconstants->kYBiasToRgb); \
  xmm0 = _mm_maddubs_epi16(*(__m128i*)yuvconstants->kUVToB, xmm3);  \
  xmm1 = _mm_maddubs_epi16(*(__m128i*)yuvconstants->kUVToG, xmm3);  \
  xmm2 = _mm_maddubs_epi16(*(__m128i*)yuvconstants->kUVToR, xmm3);  \
  xmm0 = _mm_adds_epi16(xmm4, xmm0);                                \
  xmm1 = _mm_subs_epi16(xmm4, xmm1);                                \
  xmm2 = _mm_adds_epi16(xmm4, xmm2);                                \
  xmm0 = _mm_srai_epi16(xmm0, 6);                                   \
  xmm1 = _mm_srai_epi16(xmm1, 6);                                   \
  xmm2 = _mm_srai_epi16(xmm2, 6);                                   \
  xmm0 = _mm_packus_epi16(xmm0, xmm0);                              \
  xmm1 = _mm_packus_epi16(xmm1, xmm1);                              \
  xmm2 = _mm_packus_epi16(xmm2, xmm2);

// Store 8 ARGB values.
#define STOREARGB                                    \
  xmm0 = _mm_unpacklo_epi8(xmm0, xmm1);              \
  xmm2 = _mm_unpacklo_epi8(xmm2, xmm5);              \
  xmm1 = _mm_loadu_si128(&xmm0);                     \
  xmm0 = _mm_unpacklo_epi16(xmm0, xmm2);             \
  xmm1 = _mm_unpackhi_epi16(xmm1, xmm2);             \
  _mm_storeu_si128((__m128i*)dst_argb, xmm0);        \
  _mm_storeu_si128((__m128i*)(dst_argb + 16), xmm1); \
  dst_argb += 32;

#if defined(HAS_ARGBTOYROW_AVX2)
LIBYUV_TARGET_AVX2
void ARGBToYMatrixRow_AVX2(const uint8_t* src_argb,
                           uint8_t* dst_y,
                           int width,
                           const struct ArgbConstants* c) {
  __m256i ymm5 = _mm256_set1_epi8((char)0x80);
  __m128i kRGBToY = _mm_loadu_si128((const __m128i*)c->kRGBToY);
  __m256i ymm4 = _mm256_broadcastsi128_si256(kRGBToY);
  __m128i kAddY = _mm_loadu_si128((const __m128i*)c->kAddY);
  __m256i ymm7 = _mm256_broadcastsi128_si256(kAddY);
  __m256i ymm6 = _mm256_maddubs_epi16(ymm4, ymm5);
  ymm6 = _mm256_hadd_epi16(ymm6, ymm6);
  ymm7 = _mm256_sub_epi16(ymm7, ymm6);
  __m256i perm_mask = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);

  while (width > 0) {
    __m256i ymm0 = _mm256_loadu_si256((const __m256i*)src_argb);
    __m256i ymm1 = _mm256_loadu_si256((const __m256i*)(src_argb + 32));
    __m256i ymm2 = _mm256_loadu_si256((const __m256i*)(src_argb + 64));
    __m256i ymm3 = _mm256_loadu_si256((const __m256i*)(src_argb + 96));
    src_argb += 128;

    ymm0 = _mm256_sub_epi8(ymm0, ymm5);
    ymm1 = _mm256_sub_epi8(ymm1, ymm5);
    ymm2 = _mm256_sub_epi8(ymm2, ymm5);
    ymm3 = _mm256_sub_epi8(ymm3, ymm5);

    ymm0 = _mm256_maddubs_epi16(ymm4, ymm0);
    ymm1 = _mm256_maddubs_epi16(ymm4, ymm1);
    ymm2 = _mm256_maddubs_epi16(ymm4, ymm2);
    ymm3 = _mm256_maddubs_epi16(ymm4, ymm3);

    ymm0 = _mm256_hadd_epi16(ymm0, ymm1);
    ymm2 = _mm256_hadd_epi16(ymm2, ymm3);

    ymm0 = _mm256_add_epi16(ymm0, ymm7);
    ymm2 = _mm256_add_epi16(ymm2, ymm7);

    ymm0 = _mm256_srli_epi16(ymm0, 8);
    ymm2 = _mm256_srli_epi16(ymm2, 8);

    ymm0 = _mm256_packus_epi16(ymm0, ymm2);
    ymm0 = _mm256_permutevar8x32_epi32(ymm0, perm_mask);

    _mm256_storeu_si256((__m256i*)dst_y, ymm0);
    dst_y += 32;
    width -= 32;
  }
}

LIBYUV_TARGET_AVX2
void ARGBToYRow_AVX2(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_argb, dst_y, width, &kArgbI601Constants);
}

LIBYUV_TARGET_AVX2
void ABGRToYRow_AVX2(const uint8_t* src_abgr, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_abgr, dst_y, width, &kAbgrI601Constants);
}

LIBYUV_TARGET_AVX2
void ARGBToYJRow_AVX2(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_argb, dst_y, width, &kArgbJPEGConstants);
}

LIBYUV_TARGET_AVX2
void ABGRToYJRow_AVX2(const uint8_t* src_abgr, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_abgr, dst_y, width, &kAbgrJPEGConstants);
}

LIBYUV_TARGET_AVX2
void RGBAToYJRow_AVX2(const uint8_t* src_rgba, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_rgba, dst_y, width, &kRgbaJPEGConstants);
}

LIBYUV_TARGET_AVX2
void RGBAToYRow_AVX2(const uint8_t* src_rgba, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_rgba, dst_y, width, &kRgbaI601Constants);
}

LIBYUV_TARGET_AVX2
void BGRAToYRow_AVX2(const uint8_t* src_bgra, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_bgra, dst_y, width, &kBgraI601Constants);
}
#endif

#endif  // ((defined(_MSC_VER) && !defined(__clang__)) || defined(LIBYUV_ENABLE_ROWWIN))

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // !defined(LIBYUV_DISABLE_X86)
