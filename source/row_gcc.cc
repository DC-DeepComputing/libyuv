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

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// This module is for GCC x86 and x64.
#if !defined(LIBYUV_DISABLE_X86) &&               \
    (defined(__x86_64__) || defined(__i386__)) && \
    !defined(LIBYUV_ENABLE_ROWWIN)

#if !defined(LIBYUV_DISABLE_X86)

// Constants for ARGB

// JPeg full range.
static const uvec8 kARGBToYJ = {29u, 150u, 77u, 0u, 29u, 150u, 77u, 0u,
                                29u, 150u, 77u, 0u, 29u, 150u, 77u, 0u};


#endif

#if !defined(LIBYUV_DISABLE_X86)
// Constants for BGRA

// Constants for ABGR

// Constants for RGBA.
// 126 (7e) - (-109..110) = 16..235

static const uvec16 kSub128 = {0x8080u, 0x8080u, 0x8080u, 0x8080u,
                               0x8080u, 0x8080u, 0x8080u, 0x8080u};

#endif

#if !defined(LIBYUV_DISABLE_X86)

// Shuffle table for converting RGB24 to ARGB.
static const uvec8 kShuffleMaskRGB24ToARGB = {
    0u, 1u, 2u, 12u, 3u, 4u, 5u, 13u, 6u, 7u, 8u, 14u, 9u, 10u, 11u, 15u};

// Shuffle table for converting RAW to ARGB.
static const uvec8 kShuffleMaskRAWToARGB = {
    2u, 1u, 0u, 128u, 5u, 4u, 3u, 128u, 8u, 7u, 6u, 128u, 11u, 10u, 9u, 128u};
// Shuffle table for converting RAW to ARGB.  Last 12
static const uvec8 kShuffleMaskRAWToARGB_0 = {6u,  5u,   4u,  128u, 9u,  8u,
                                              7u,  128u, 12u, 11u,  10u, 128u,
                                              15u, 14u,  13u, 128u};

// Shuffle table for converting RAW to RGBA.
static const uvec8 kShuffleMaskRAWToRGBA = {
    128u, 2u, 1u, 0u, 128u, 5u, 4u, 3u, 128u, 8u, 7u, 6u, 128u, 11u, 10u, 9u};

// Shuffle table for converting RAW to RGB24.  First 8.
static const uvec8 kShuffleMaskRAWToRGB24_0 = {
    2u,   1u,   0u,   5u,   4u,   3u,   8u,   7u,
    128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u};

// Shuffle table for converting RAW to RGB24.  Middle 8.
static const uvec8 kShuffleMaskRAWToRGB24_1 = {
    2u,   7u,   6u,   5u,   10u,  9u,   8u,   13u,
    128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u};

// Shuffle table for converting RAW to RGB24.  Last 8.
static const uvec8 kShuffleMaskRAWToRGB24_2 = {
    8u,   7u,   12u,  11u,  10u,  15u,  14u,  13u,
    128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u};

// Shuffle table for converting ARGB to RGB24.
static const uvec8 kShuffleMaskARGBToRGB24 = {
    0u, 1u, 2u, 4u, 5u, 6u, 8u, 9u, 10u, 12u, 13u, 14u, 128u, 128u, 128u, 128u};

// Shuffle table for converting ARGB to RAW.
static const uvec8 kShuffleMaskARGBToRAW = {
    2u, 1u, 0u, 6u, 5u, 4u, 10u, 9u, 8u, 14u, 13u, 12u, 128u, 128u, 128u, 128u};

// Shuffle table for converting ARGBToRGB24 for I422ToRGB24.  First 8 + next 4
static const uvec8 kShuffleMaskARGBToRGB24_0 = {
    0u, 1u, 2u, 4u, 5u, 6u, 8u, 9u, 128u, 128u, 128u, 128u, 10u, 12u, 13u, 14u};

// YUY2 shuf 16 Y to 32 Y.
static const vec8 kShuffleYUY2Y = {0, 0, 2,  2,  4,  4,  6,  6,
                                   8, 8, 10, 10, 12, 12, 14, 14};

// YUY2 shuf 8 UV to 16 UV.
static const vec8 kShuffleYUY2UV = {1, 3,  1, 3,  5,  7,  5,  7,
                                    9, 11, 9, 11, 13, 15, 13, 15};

// UYVY shuf 16 Y to 32 Y.
static const vec8 kShuffleUYVYY = {1, 1, 3,  3,  5,  5,  7,  7,
                                   9, 9, 11, 11, 13, 13, 15, 15};

// UYVY shuf 8 UV to 16 UV.
static const vec8 kShuffleUYVYUV = {0, 2,  0, 2,  4,  6,  4,  6,
                                    8, 10, 8, 10, 12, 14, 12, 14};

// NV21 shuf 8 VU to 16 UV.
static const lvec8 kShuffleNV21 = {
    1, 0, 1, 0, 3, 2, 3, 2, 5, 4, 5, 4, 7, 6, 7, 6,
    1, 0, 1, 0, 3, 2, 3, 2, 5, 4, 5, 4, 7, 6, 7, 6,
};
#endif


#if !defined(LIBYUV_DISABLE_X86)


void RAWToARGBRow_AVX2(const uint8_t* src_raw, uint8_t* dst_argb, int width) {
  asm volatile(
      "vpcmpeqb    %%ymm6,%%ymm6,%%ymm6          \n"  // 0xff000000
      "vpslld      $0x18,%%ymm6,%%ymm6           \n"
      "vbroadcastf128 %3,%%ymm4                  \n"  //
      "vbroadcastf128 %4,%%ymm5                  \n"  //

      LABELALIGN  //
      "1:          \n"
      "vmovdqu     (%0),%%xmm0                   \n"  // first 12
      "vinserti128 $1,12(%0),%%ymm0,%%ymm0       \n"  // second 12
      "vmovdqu     24(%0),%%xmm1                 \n"  // third 12
      "vinserti128 $1,36(%0),%%ymm1,%%ymm1       \n"  // forth 12
      "vmovdqu     48(%0),%%xmm2                 \n"  // fifth 12
      "vinserti128 $1,60(%0),%%ymm2,%%ymm2       \n"  // sixth 12
      "vmovdqu     68(%0),%%xmm3                 \n"  // seventh 12
      "vinserti128 $1,80(%0),%%ymm3,%%ymm3       \n"  // eighth 12
      "lea         96(%0),%0                     \n"
      "vpshufb     %%ymm4,%%ymm0,%%ymm0          \n"
      "vpshufb     %%ymm4,%%ymm1,%%ymm1          \n"
      "vpshufb     %%ymm4,%%ymm2,%%ymm2          \n"
      "vpshufb     %%ymm5,%%ymm3,%%ymm3          \n"
      "vpor        %%ymm6,%%ymm0,%%ymm0          \n"
      "vpor        %%ymm6,%%ymm1,%%ymm1          \n"
      "vpor        %%ymm6,%%ymm2,%%ymm2          \n"
      "vpor        %%ymm6,%%ymm3,%%ymm3          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "vmovdqu     %%ymm2,0x40(%1)               \n"
      "vmovdqu     %%ymm3,0x60(%1)               \n"
      "lea         0x80(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_raw),                // %0
        "+r"(dst_argb),               // %1
        "+r"(width)                   // %2
      : "m"(kShuffleMaskRAWToARGB),   // %3
        "m"(kShuffleMaskRAWToARGB_0)  // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}

// Same code as RAWToARGB with different shuffler and A in low bits







#ifdef HAS_ARGBTORGB24ROW_AVX2
// vpermd for 12+12 to 24
static const lvec32 kPermdRGB24_AVX = {0, 1, 2, 4, 5, 6, 3, 7};

void ARGBToRGB24Row_AVX2(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm6                  \n"
      "vmovdqa     %4,%%ymm7                     \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x40(%0),%%ymm2               \n"
      "vmovdqu     0x60(%0),%%ymm3               \n"
      "lea         0x80(%0),%0                   \n"
      "vpshufb     %%ymm6,%%ymm0,%%ymm0          \n"  // xxx0yyy0
      "vpshufb     %%ymm6,%%ymm1,%%ymm1          \n"
      "vpshufb     %%ymm6,%%ymm2,%%ymm2          \n"
      "vpshufb     %%ymm6,%%ymm3,%%ymm3          \n"
      "vpermd      %%ymm0,%%ymm7,%%ymm0          \n"  // pack to 24 bytes
      "vpermd      %%ymm1,%%ymm7,%%ymm1          \n"
      "vpermd      %%ymm2,%%ymm7,%%ymm2          \n"
      "vpermd      %%ymm3,%%ymm7,%%ymm3          \n"
      "vpermq      $0x3f,%%ymm1,%%ymm4           \n"  // combine 24 + 8
      "vpor        %%ymm4,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vpermq      $0xf9,%%ymm1,%%ymm1           \n"  // combine 16 + 16
      "vpermq      $0x4f,%%ymm2,%%ymm4           \n"
      "vpor        %%ymm4,%%ymm1,%%ymm1          \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "vpermq      $0xfe,%%ymm2,%%ymm2           \n"  // combine 8 + 24
      "vpermq      $0x93,%%ymm3,%%ymm3           \n"
      "vpor        %%ymm3,%%ymm2,%%ymm2          \n"
      "vmovdqu     %%ymm2,0x40(%1)               \n"
      "lea         0x60(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),                     // %0
        "+r"(dst),                     // %1
        "+r"(width)                    // %2
      : "m"(kShuffleMaskARGBToRGB24),  // %3
        "m"(kPermdRGB24_AVX)           // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif

#ifdef HAS_ARGBTORGB24ROW_AVX512VBMI
// Shuffle table for converting ARGBToRGB24
static const ulvec8 kPermARGBToRGB24_0 = {
    0u,  1u,  2u,  4u,  5u,  6u,  8u,  9u,  10u, 12u, 13u,
    14u, 16u, 17u, 18u, 20u, 21u, 22u, 24u, 25u, 26u, 28u,
    29u, 30u, 32u, 33u, 34u, 36u, 37u, 38u, 40u, 41u};
static const ulvec8 kPermARGBToRGB24_1 = {
    10u, 12u, 13u, 14u, 16u, 17u, 18u, 20u, 21u, 22u, 24u,
    25u, 26u, 28u, 29u, 30u, 32u, 33u, 34u, 36u, 37u, 38u,
    40u, 41u, 42u, 44u, 45u, 46u, 48u, 49u, 50u, 52u};
static const ulvec8 kPermARGBToRGB24_2 = {
    21u, 22u, 24u, 25u, 26u, 28u, 29u, 30u, 32u, 33u, 34u,
    36u, 37u, 38u, 40u, 41u, 42u, 44u, 45u, 46u, 48u, 49u,
    50u, 52u, 53u, 54u, 56u, 57u, 58u, 60u, 61u, 62u};

void ARGBToRGB24Row_AVX512VBMI(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "vmovdqa     %3,%%ymm5                     \n"
      "vmovdqa     %4,%%ymm6                     \n"
      "vmovdqa     %5,%%ymm7                     \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x40(%0),%%ymm2               \n"
      "vmovdqu     0x60(%0),%%ymm3               \n"
      "lea         0x80(%0),%0                   \n"
      "vpermt2b    %%ymm1,%%ymm5,%%ymm0          \n"
      "vpermt2b    %%ymm2,%%ymm6,%%ymm1          \n"
      "vpermt2b    %%ymm3,%%ymm7,%%ymm2          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "vmovdqu     %%ymm2,0x40(%1)               \n"
      "lea         0x60(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),                // %0
        "+r"(dst),                // %1
        "+r"(width)               // %2
      : "m"(kPermARGBToRGB24_0),  // %3
        "m"(kPermARGBToRGB24_1),  // %4
        "m"(kPermARGBToRGB24_2)   // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm5", "xmm6", "xmm7");
}
#endif

#ifdef HAS_ARGBTORAWROW_AVX2
void ARGBToRAWRow_AVX2(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm6                  \n"
      "vmovdqa     %4,%%ymm7                     \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x40(%0),%%ymm2               \n"
      "vmovdqu     0x60(%0),%%ymm3               \n"
      "lea         0x80(%0),%0                   \n"
      "vpshufb     %%ymm6,%%ymm0,%%ymm0          \n"  // xxx0yyy0
      "vpshufb     %%ymm6,%%ymm1,%%ymm1          \n"
      "vpshufb     %%ymm6,%%ymm2,%%ymm2          \n"
      "vpshufb     %%ymm6,%%ymm3,%%ymm3          \n"
      "vpermd      %%ymm0,%%ymm7,%%ymm0          \n"  // pack to 24 bytes
      "vpermd      %%ymm1,%%ymm7,%%ymm1          \n"
      "vpermd      %%ymm2,%%ymm7,%%ymm2          \n"
      "vpermd      %%ymm3,%%ymm7,%%ymm3          \n"
      "vpermq      $0x3f,%%ymm1,%%ymm4           \n"  // combine 24 + 8
      "vpor        %%ymm4,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vpermq      $0xf9,%%ymm1,%%ymm1           \n"  // combine 16 + 16
      "vpermq      $0x4f,%%ymm2,%%ymm4           \n"
      "vpor        %%ymm4,%%ymm1,%%ymm1          \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "vpermq      $0xfe,%%ymm2,%%ymm2           \n"  // combine 8 + 24
      "vpermq      $0x93,%%ymm3,%%ymm3           \n"
      "vpor        %%ymm3,%%ymm2,%%ymm2          \n"
      "vmovdqu     %%ymm2,0x40(%1)               \n"
      "lea         0x60(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),                   // %0
        "+r"(dst),                   // %1
        "+r"(width)                  // %2
      : "m"(kShuffleMaskARGBToRAW),  // %3
        "m"(kPermdRGB24_AVX)         // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif



#ifdef HAS_ARGBTORGB565DITHERROW_AVX2
void ARGBToRGB565DitherRow_AVX2(const uint8_t* src,
                                uint8_t* dst,
                                uint32_t dither4,
                                int width) {
  asm volatile(
      "vbroadcastss %3,%%xmm6                    \n"
      "vpunpcklbw  %%xmm6,%%xmm6,%%xmm6          \n"
      "vpermq      $0xd8,%%ymm6,%%ymm6           \n"
      "vpunpcklwd  %%ymm6,%%ymm6,%%ymm6          \n"
      "vpcmpeqb    %%ymm3,%%ymm3,%%ymm3          \n"
      "vpsrld      $0x1b,%%ymm3,%%ymm3           \n"
      "vpcmpeqb    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpsrld      $0x1a,%%ymm4,%%ymm4           \n"
      "vpslld      $0x5,%%ymm4,%%ymm4            \n"
      "vpslld      $0xb,%%ymm3,%%ymm5            \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vpaddusb    %%ymm6,%%ymm0,%%ymm0          \n"
      "vpsrld      $0x5,%%ymm0,%%ymm2            \n"
      "vpsrld      $0x3,%%ymm0,%%ymm1            \n"
      "vpsrld      $0x8,%%ymm0,%%ymm0            \n"
      "vpand       %%ymm4,%%ymm2,%%ymm2          \n"
      "vpand       %%ymm3,%%ymm1,%%ymm1          \n"
      "vpand       %%ymm5,%%ymm0,%%ymm0          \n"
      "vpor        %%ymm2,%%ymm1,%%ymm1          \n"
      "vpor        %%ymm1,%%ymm0,%%ymm0          \n"
      "vpackusdw   %%ymm0,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "lea         0x20(%0),%0                   \n"
      "vmovdqu     %%xmm0,(%1)                   \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),    // %0
        "+r"(dst),    // %1
        "+r"(width)   // %2
      : "m"(dither4)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ARGBTORGB565DITHERROW_AVX2


#endif

/*

ARGBToAR30Row:

Red Blue
With the 8 bit value in the upper bits of a short, vpmulhuw by (1024+4) will
produce a 10 bit value in the low 10 bits of each 16 bit value. This is whats
wanted for the blue channel. The red needs to be shifted 4 left, so multiply by
(1024+4)*16 for red.

Alpha Green
Alpha and Green are already in the high bits so vpand can zero out the other
bits, keeping just 2 upper bits of alpha and 8 bit green. The same multiplier
could be used for Green - (1024+4) putting the 10 bit green in the lsb.  Alpha
would be a simple multiplier to shift it into position.  It wants a gap of 10
above the green.  Green is 10 bits, so there are 6 bits in the low short.  4
more are needed, so a multiplier of 4 gets the 2 bits into the upper 16 bits,
and then a shift of 4 is a multiply of 16, so (4*16) = 64.  Then shift the
result left 10 to position the A and G channels.
*/

// Shuffle table for converting RAW to RGB24.  Last 8.
static const uvec8 kShuffleRB30 = {128u, 0u, 128u, 2u,  128u, 4u,  128u, 6u,
                                   128u, 8u, 128u, 10u, 128u, 12u, 128u, 14u};

static const uvec8 kShuffleBR30 = {128u, 2u,  128u, 0u, 128u, 6u,  128u, 4u,
                                   128u, 10u, 128u, 8u, 128u, 14u, 128u, 12u};

static const uint32_t kMulRB10 = 1028 * 16 * 65536 + 1028;
static const uint32_t kMaskRB10 = 0x3ff003ff;
static const uint32_t kMaskAG10 = 0xc000ff00;
static const uint32_t kMulAG10 = 64 * 65536 + 1028;



#ifdef HAS_ARGBTOAR30ROW_AVX2
void ARGBToAR30Row_AVX2(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm2                  \n"  // shuffler for RB
      "vbroadcastss %4,%%ymm3                    \n"  // multipler for RB
      "vbroadcastss %5,%%ymm4                    \n"  // mask for R10 B10
      "vbroadcastss %6,%%ymm5                    \n"  // mask for AG
      "vbroadcastss %7,%%ymm6                    \n"  // multipler for AG
      "sub         %0,%1                         \n"

      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"  // fetch 8 ARGB pixels
      "vpshufb     %%ymm2,%%ymm0,%%ymm1          \n"  // R0B0
      "vpand       %%ymm5,%%ymm0,%%ymm0          \n"  // A0G0
      "vpmulhuw    %%ymm3,%%ymm1,%%ymm1          \n"  // X2 R16 X4  B10
      "vpmulhuw    %%ymm6,%%ymm0,%%ymm0          \n"  // X10 A2 X10 G10
      "vpand       %%ymm4,%%ymm1,%%ymm1          \n"  // X2 R10 X10 B10
      "vpslld      $10,%%ymm0,%%ymm0             \n"  // A2 x10 G10 x10
      "vpor        %%ymm1,%%ymm0,%%ymm0          \n"  // A2 R10 G10 B10
      "vmovdqu     %%ymm0,(%1,%0)                \n"  // store 8 AR30 pixels
      "add         $0x20,%0                      \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"

      : "+r"(src),          // %0
        "+r"(dst),          // %1
        "+r"(width)         // %2
      : "m"(kShuffleRB30),  // %3
        "m"(kMulRB10),      // %4
        "m"(kMaskRB10),     // %5
        "m"(kMaskAG10),     // %6
        "m"(kMulAG10)       // %7
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}
#endif

#ifdef HAS_ABGRTOAR30ROW_AVX2
void ABGRToAR30Row_AVX2(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm2                  \n"  // shuffler for RB
      "vbroadcastss %4,%%ymm3                    \n"  // multipler for RB
      "vbroadcastss %5,%%ymm4                    \n"  // mask for R10 B10
      "vbroadcastss %6,%%ymm5                    \n"  // mask for AG
      "vbroadcastss %7,%%ymm6                    \n"  // multipler for AG
      "sub         %0,%1                         \n"

      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"  // fetch 8 ABGR pixels
      "vpshufb     %%ymm2,%%ymm0,%%ymm1          \n"  // R0B0
      "vpand       %%ymm5,%%ymm0,%%ymm0          \n"  // A0G0
      "vpmulhuw    %%ymm3,%%ymm1,%%ymm1          \n"  // X2 R16 X4  B10
      "vpmulhuw    %%ymm6,%%ymm0,%%ymm0          \n"  // X10 A2 X10 G10
      "vpand       %%ymm4,%%ymm1,%%ymm1          \n"  // X2 R10 X10 B10
      "vpslld      $10,%%ymm0,%%ymm0             \n"  // A2 x10 G10 x10
      "vpor        %%ymm1,%%ymm0,%%ymm0          \n"  // A2 R10 G10 B10
      "vmovdqu     %%ymm0,(%1,%0)                \n"  // store 8 AR30 pixels
      "add         $0x20,%0                      \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"

      : "+r"(src),          // %0
        "+r"(dst),          // %1
        "+r"(width)         // %2
      : "m"(kShuffleBR30),  // %3  reversed shuffler
        "m"(kMulRB10),      // %4
        "m"(kMaskRB10),     // %5
        "m"(kMaskAG10),     // %6
        "m"(kMulAG10)       // %7
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}
#endif

static const uvec8 kShuffleARGBToABGR = {2,  1, 0, 3,  6,  5,  4,  7,
                                         10, 9, 8, 11, 14, 13, 12, 15};

static const uvec8 kShuffleARGBToAB64Lo = {2, 2, 1, 1, 0, 0, 3, 3,
                                           6, 6, 5, 5, 4, 4, 7, 7};
static const uvec8 kShuffleARGBToAB64Hi = {10, 10, 9,  9,  8,  8,  11, 11,
                                           14, 14, 13, 13, 12, 12, 15, 15};





#ifdef HAS_ARGBTOAR64ROW_AVX2
void ARGBToAR64Row_AVX2(const uint8_t* src_argb,
                        uint16_t* dst_ar64,
                        int width) {
  asm volatile(
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpunpckhbw  %%ymm0,%%ymm0,%%ymm1          \n"
      "vpunpcklbw  %%ymm0,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "lea         0x20(%0),%0                   \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),  // %0
        "+r"(dst_ar64),  // %1
        "+r"(width)      // %2
        ::"memory",
        "cc", "xmm0", "xmm1");
}
#endif

#ifdef HAS_ARGBTOAB64ROW_AVX2
void ARGBToAB64Row_AVX2(const uint8_t* src_argb,
                        uint16_t* dst_ab64,
                        int width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm2                  \n"
      "vbroadcastf128 %4,%%ymm3                  \n" LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpshufb     %%ymm3,%%ymm0,%%ymm1          \n"
      "vpshufb     %%ymm2,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "lea         0x20(%0),%0                   \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),             // %0
        "+r"(dst_ab64),             // %1
        "+r"(width)                 // %2
      : "m"(kShuffleARGBToAB64Lo),  // %3
        "m"(kShuffleARGBToAB64Hi)   // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif

#ifdef HAS_AR64TOARGBROW_AVX2
void AR64ToARGBRow_AVX2(const uint16_t* src_ar64,
                        uint8_t* dst_argb,
                        int width) {
  asm volatile(
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vpsrlw      $8,%%ymm0,%%ymm0              \n"
      "vpsrlw      $8,%%ymm1,%%ymm1              \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x40(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ar64),  // %0
        "+r"(dst_argb),  // %1
        "+r"(width)      // %2
        ::"memory",
        "cc", "xmm0", "xmm1");
}
#endif

#ifdef HAS_AB64TOARGBROW_AVX2
void AB64ToARGBRow_AVX2(const uint16_t* src_ab64,
                        uint8_t* dst_argb,
                        int width) {
      asm volatile("vbroadcastf128 %3,%%ymm2                  \n" LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vpsrlw      $8,%%ymm0,%%ymm0              \n"
      "vpsrlw      $8,%%ymm1,%%ymm1              \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpshufb     %%ymm2,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x40(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_ab64),          // %0
                 "+r"(dst_argb),          // %1
                 "+r"(width)              // %2
               : "m"(kShuffleARGBToABGR)  // %3
               : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif

// clang-format off

// TODO(mraptis): Consider passing R, G, B multipliers as parameter.
// round parameter is register containing value to add before shift.
#define RGBTOY(round)                            \
  "1:                                        \n" \
  "movdqu    (%0),%%xmm0                     \n" \
  "movdqu    0x10(%0),%%xmm1                 \n" \
  "movdqu    0x20(%0),%%xmm2                 \n" \
  "movdqu    0x30(%0),%%xmm3                 \n" \
  "psubb     %%xmm5,%%xmm0                   \n" \
  "psubb     %%xmm5,%%xmm1                   \n" \
  "psubb     %%xmm5,%%xmm2                   \n" \
  "psubb     %%xmm5,%%xmm3                   \n" \
  "movdqu    %%xmm4,%%xmm6                   \n" \
  "pmaddubsw %%xmm0,%%xmm6                   \n" \
  "movdqu    %%xmm4,%%xmm0                   \n" \
  "pmaddubsw %%xmm1,%%xmm0                   \n" \
  "movdqu    %%xmm4,%%xmm1                   \n" \
  "pmaddubsw %%xmm2,%%xmm1                   \n" \
  "movdqu    %%xmm4,%%xmm2                   \n" \
  "pmaddubsw %%xmm3,%%xmm2                   \n" \
  "lea       0x40(%0),%0                     \n" \
  "phaddw    %%xmm0,%%xmm6                   \n" \
  "phaddw    %%xmm2,%%xmm1                   \n" \
  "prefetcht0 1280(%0)                       \n" \
  "paddw     %%" #round ",%%xmm6             \n" \
  "paddw     %%" #round ",%%xmm1             \n" \
  "psrlw     $0x8,%%xmm6                     \n" \
  "psrlw     $0x8,%%xmm1                     \n" \
  "packuswb  %%xmm1,%%xmm6                   \n" \
  "movdqu    %%xmm6,(%1)                     \n" \
  "lea       0x10(%1),%1                     \n" \
  "sub       $0x10,%2                        \n" \
  "jg        1b                              \n"

#define RGBTOY_AVX2(round)                                       \
  "1:                                        \n"                 \
  "vmovdqu    (%0),%%ymm0                    \n"                 \
  "vmovdqu    0x20(%0),%%ymm1                \n"                 \
  "vmovdqu    0x40(%0),%%ymm2                \n"                 \
  "vmovdqu    0x60(%0),%%ymm3                \n"                 \
  "vpsubb     %%ymm5, %%ymm0, %%ymm0         \n"                 \
  "vpsubb     %%ymm5, %%ymm1, %%ymm1         \n"                 \
  "vpsubb     %%ymm5, %%ymm2, %%ymm2         \n"                 \
  "vpsubb     %%ymm5, %%ymm3, %%ymm3         \n"                 \
  "vpmaddubsw %%ymm0,%%ymm4,%%ymm0           \n"                 \
  "vpmaddubsw %%ymm1,%%ymm4,%%ymm1           \n"                 \
  "vpmaddubsw %%ymm2,%%ymm4,%%ymm2           \n"                 \
  "vpmaddubsw %%ymm3,%%ymm4,%%ymm3           \n"                 \
  "lea       0x80(%0),%0                     \n"                 \
  "vphaddw    %%ymm1,%%ymm0,%%ymm0           \n" /* mutates. */  \
  "vphaddw    %%ymm3,%%ymm2,%%ymm2           \n"                 \
  "prefetcht0 1280(%0)                       \n"                 \
  "vpaddw     %%" #round ",%%ymm0,%%ymm0     \n" /* Add 16 */    \
  "vpaddw     %%" #round ",%%ymm2,%%ymm2     \n"                 \
  "vpsrlw     $0x8,%%ymm0,%%ymm0             \n"                 \
  "vpsrlw     $0x8,%%ymm2,%%ymm2             \n"                 \
  "vpackuswb  %%ymm2,%%ymm0,%%ymm0           \n" /* mutates. */  \
  "vpermd     %%ymm0,%%ymm6,%%ymm0           \n" /* unmutate. */ \
  "vmovdqu    %%ymm0,(%1)                    \n"                 \
  "lea       0x20(%1),%1                     \n"                 \
  "sub       $0x20,%2                        \n"                 \
  "jg        1b                              \n"

// clang-format on





#if defined(HAS_ARGBTOYROW_AVX2) || defined(HAS_ABGRTOYROW_AVX2) || \
    defined(HAS_ARGBEXTRACTALPHAROW_AVX2)
// vpermd for vphaddw + vpackuswb vpermd.
#endif

#ifdef HAS_ARGBTOYROW_AVX2

// Convert 32 ARGB pixels (128 bytes) to 32 Y values.
#ifdef HAS_ARGBTOYROW_AVX2
void ARGBToYRow_AVX2(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_argb, dst_y, width, &kArgbI601Constants);
}
#endif
#endif  // HAS_ARGBTOYROW_AVX2

#ifdef HAS_ABGRTOYROW_AVX2
// Convert 32 ABGR pixels (128 bytes) to 32 Y values.
#ifdef HAS_ARGBTOYROW_AVX2
void ABGRToYRow_AVX2(const uint8_t* src_abgr, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_abgr, dst_y, width, &kAbgrI601Constants);
}
#endif
#endif  // HAS_ABGRTOYROW_AVX2

#ifdef HAS_ARGBTOYJROW_AVX2
// Convert 32 ARGB pixels (128 bytes) to 32 Y values.
#ifdef HAS_ARGBTOYROW_AVX2
void ARGBToYJRow_AVX2(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_argb, dst_y, width, &kArgbJPEGConstants);
}
#endif

#endif  // HAS_ARGBTOYJROW_AVX2

#ifdef HAS_ABGRTOYJROW_AVX2
// Convert 32 ABGR pixels (128 bytes) to 32 Y values.
#ifdef HAS_ARGBTOYROW_AVX2
void ABGRToYJRow_AVX2(const uint8_t* src_abgr, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_abgr, dst_y, width, &kAbgrJPEGConstants);
}
#endif
#endif  // HAS_ABGRTOYJROW_AVX2

#ifdef HAS_RGBATOYJROW_AVX2
// Convert 32 ARGB pixels (128 bytes) to 32 Y values.
#ifdef HAS_ARGBTOYROW_AVX2
void RGBAToYJRow_AVX2(const uint8_t* src_rgba, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_rgba, dst_y, width, &kRgbaJPEGConstants);
}
#endif
#endif  // HAS_RGBATOYJROW_AVX2

#if defined(HAS_ARGBTOYROW_AVX2) || defined(HAS_ARGBTOUV444ROW_AVX2) || \
    defined(HAS_ARGBEXTRACTALPHAROW_AVX2)
// vpermd for vphaddw + vpackuswb vpermd.
static const lvec32 kPermdARGBToY_AVX = {0, 4, 1, 5, 2, 6, 3, 7};
#endif


#ifdef HAS_ARGBTOYROW_AVX2
void ARGBToYMatrixRow_AVX2(const uint8_t* src_argb,
                           uint8_t* dst_y,
                           int width,
                           const struct ArgbConstants* c) {
  asm volatile(
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpsllw      $15,%%ymm5,%%ymm5             \n"
      "vpacksswb   %%ymm5,%%ymm5,%%ymm5          \n"
      "vbroadcastf128 0(%3),%%ymm4               \n"
      "vbroadcastf128 0x60(%3),%%ymm7            \n"
      "vpmaddubsw  %%ymm5,%%ymm4,%%ymm6          \n"
      "vphaddw     %%ymm6,%%ymm6,%%ymm6          \n"
      "vpsubw      %%ymm6,%%ymm7,%%ymm7          \n"
      "vmovdqa     %4,%%ymm6                     \n"
      LABELALIGN ""
      RGBTOY_AVX2(ymm7)
      "vzeroupper  \n"
      : "+r"(src_argb),         // %0
        "+r"(dst_y),            // %1
        "+r"(width)             // %2
      : "r"(c),                 // %3
        "m"(kPermdARGBToY_AVX)  // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif

#if defined(HAS_ARGBTOYROW_AVX512BW) || defined(HAS_ARGBTOUV444ROW_AVX512BW) || defined(HAS_ARGBTOUVROW_AVX512BW)
static const uint32_t kPermdARGBToY_AVX512BW[16] = {0, 4, 8, 12, 1, 5, 9, 13,
                                                    2, 6, 10, 14, 3, 7, 11, 15};
#endif

#if defined(HAS_ARGBTOUVROW_AVX512BW) || defined(HAS_ARGBTOUVJROW_AVX512BW)
static const uint32_t kPermdARGBToUV_AVX512BW[16] = {
    0, 1, 4, 5, 8, 9, 12, 13, 2, 3, 6, 7, 10, 11, 14, 15};
#endif

#ifdef HAS_ARGBTOYROW_AVX512BW
void ARGBToYMatrixRow_AVX512BW(const uint8_t* src_argb,
                               uint8_t* dst_y,
                               int width,
                               const struct ArgbConstants* c) {
  asm volatile(
      "vpternlogd  $0xff,%%zmm16,%%zmm16,%%zmm16 \n"
      "vpsllw      $15,%%zmm16,%%zmm5            \n"
      "vpacksswb   %%zmm5,%%zmm5,%%zmm5          \n"
      "vpsrlw      $15,%%zmm16,%%zmm16           \n" // zmm16 = 1
      "vbroadcasti64x4 0(%3),%%zmm4              \n"
      "vbroadcasti64x4 0x60(%3),%%zmm7           \n"
      "vpmaddubsw  %%zmm5,%%zmm4,%%zmm6          \n"
      "vpmaddwd    %%zmm16,%%zmm6,%%zmm6         \n"
      "vpackssdw   %%zmm6,%%zmm6,%%zmm6          \n"
      "vpsubw      %%zmm6,%%zmm7,%%zmm7          \n"
      "vmovups     %4,%%zmm6                     \n"
      LABELALIGN
      "1:          \n"
      "vmovups     (%0),%%zmm0                   \n"
      "vmovups     0x40(%0),%%zmm1               \n"
      "vmovups     0x80(%0),%%zmm2               \n"
      "vmovups     0xc0(%0),%%zmm3               \n"
      "vpsubb      %%zmm5,%%zmm0,%%zmm0          \n"
      "vpsubb      %%zmm5,%%zmm1,%%zmm1          \n"
      "vpsubb      %%zmm5,%%zmm2,%%zmm2          \n"
      "vpsubb      %%zmm5,%%zmm3,%%zmm3          \n"
      "vpmaddubsw  %%zmm0,%%zmm4,%%zmm0          \n"
      "vpmaddubsw  %%zmm1,%%zmm4,%%zmm1          \n"
      "vpmaddubsw  %%zmm2,%%zmm4,%%zmm2          \n"
      "vpmaddubsw  %%zmm3,%%zmm4,%%zmm3          \n"
      "lea         0x100(%0),%0                  \n"
      "vpmaddwd    %%zmm16,%%zmm0,%%zmm0         \n"
      "vpmaddwd    %%zmm16,%%zmm1,%%zmm1         \n"
      "vpackssdw   %%zmm1,%%zmm0,%%zmm0          \n"
      "vpmaddwd    %%zmm16,%%zmm2,%%zmm2         \n"
      "vpmaddwd    %%zmm16,%%zmm3,%%zmm3         \n"
      "vpackssdw   %%zmm3,%%zmm2,%%zmm2          \n"
      "vpaddw      %%zmm7,%%zmm0,%%zmm0          \n"
      "vpaddw      %%zmm7,%%zmm2,%%zmm2          \n"
      "vpsrlw      $0x8,%%zmm0,%%zmm0            \n"
      "vpsrlw      $0x8,%%zmm2,%%zmm2            \n"
      "vpackuswb   %%zmm2,%%zmm0,%%zmm0          \n"
      "vpermd      %%zmm0,%%zmm6,%%zmm0          \n"
      "vmovups     %%zmm0,(%1)                   \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x40,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),  // %0
        "+r"(dst_y),     // %1
        "+r"(width)      // %2
      : "r"(c),          // %3
        "m"(kPermdARGBToY_AVX512BW) // %4
      : "memory", "cc", "zmm0", "zmm1", "zmm2", "zmm3", "zmm4", "zmm5", "zmm6",
        "zmm7", "zmm16");
}
#endif


#ifdef HAS_ARGBTOUV444ROW_AVX2

void ARGBToUV444MatrixRow_AVX2(const uint8_t* src_argb,
                               uint8_t* dst_u,
                               uint8_t* dst_v,
                               int width,
                               const struct ArgbConstants* c) {
  asm volatile(
      "vmovdqa     0x20(%4),%%ymm3               \n"  // kRGBToU
      "vmovdqa     0x40(%4),%%ymm4               \n"  // kRGBToV
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"  // 0x8000
      "vpsllw      $15,%%ymm5,%%ymm5             \n"
      "vmovdqa     %5,%%ymm7                     \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x40(%0),%%ymm2               \n"
      "vmovdqu     0x60(%0),%%ymm6               \n"
      "vpmaddubsw  %%ymm3,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm3,%%ymm1,%%ymm1          \n"
      "vpmaddubsw  %%ymm3,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm3,%%ymm6,%%ymm6          \n"
      "vphaddw     %%ymm1,%%ymm0,%%ymm0          \n"  // mutates
      "vphaddw     %%ymm6,%%ymm2,%%ymm2          \n"
      "vpsubw      %%ymm0,%%ymm5,%%ymm0          \n"
      "vpsubw      %%ymm2,%%ymm5,%%ymm2          \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x8,%%ymm2,%%ymm2            \n"
      "vpackuswb   %%ymm2,%%ymm0,%%ymm0          \n"  // mutates
      "vpermd      %%ymm0,%%ymm7,%%ymm0          \n"  // unmutate.
      "vmovdqu     %%ymm0,(%1)                   \n"

      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x40(%0),%%ymm2               \n"
      "vmovdqu     0x60(%0),%%ymm6               \n"
      "vpmaddubsw  %%ymm4,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm4,%%ymm1,%%ymm1          \n"
      "vpmaddubsw  %%ymm4,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm4,%%ymm6,%%ymm6          \n"
      "vphaddw     %%ymm1,%%ymm0,%%ymm0          \n"  // mutates
      "vphaddw     %%ymm6,%%ymm2,%%ymm2          \n"
      "vpsubw      %%ymm0,%%ymm5,%%ymm0          \n"
      "vpsubw      %%ymm2,%%ymm5,%%ymm2          \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x8,%%ymm2,%%ymm2            \n"
      "vpackuswb   %%ymm2,%%ymm0,%%ymm0          \n"  // mutates
      "vpermd      %%ymm0,%%ymm7,%%ymm0          \n"  // unmutate.
      "vmovdqu     %%ymm0,(%1,%2,1)              \n"
      "lea         0x80(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"
      "subl        $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),  // %0
        "+r"(dst_u),     // %1
        "+r"(dst_v),     // %2
#if defined(__i386__)
        "+m"(width)  // %3
#else
        "+rm"(width)  // %3
#endif
      : "r"(c),                 // %4
        "m"(kPermdARGBToY_AVX)  // %5
      : "memory", "cc", "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6",
        "ymm7");
}
#endif  // HAS_ARGBTOUV444ROW_AVX2

#ifdef HAS_ARGBTOUV444ROW_AVX512BW

void ARGBToUV444MatrixRow_AVX512BW(const uint8_t* src_argb,
                                   uint8_t* dst_u,
                                   uint8_t* dst_v,
                                   int width,
                                   const struct ArgbConstants* c) {
  asm volatile(
      "vbroadcasti64x4 0x20(%4),%%zmm3               \n"  // kRGBToU
      "vbroadcasti64x4 0x40(%4),%%zmm4               \n"  // kRGBToV
      "vpternlogd  $0xff,%%zmm16,%%zmm16,%%zmm16 \n"  // -1
      "vpsllw      $15,%%zmm16,%%zmm5            \n"  // 0x8000
      "vmovups     %5,%%zmm7                     \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:          \n"
      "vmovups     (%0),%%zmm0                   \n"
      "vmovups     0x40(%0),%%zmm1               \n"
      "vmovups     0x80(%0),%%zmm2               \n"
      "vmovups     0xc0(%0),%%zmm6               \n"
      "vpmaddubsw  %%zmm3,%%zmm0,%%zmm0          \n"
      "vpmaddubsw  %%zmm3,%%zmm1,%%zmm1          \n"
      "vpmaddubsw  %%zmm3,%%zmm2,%%zmm2          \n"
      "vpmaddubsw  %%zmm3,%%zmm6,%%zmm6          \n"
      "vpmaddwd    %%zmm16,%%zmm0,%%zmm0         \n"
      "vpmaddwd    %%zmm16,%%zmm1,%%zmm1         \n"
      "vpmaddwd    %%zmm16,%%zmm2,%%zmm2         \n"
      "vpmaddwd    %%zmm16,%%zmm6,%%zmm6         \n"
      "vpackssdw   %%zmm1,%%zmm0,%%zmm0          \n"  // mutates
      "vpackssdw   %%zmm6,%%zmm2,%%zmm2          \n"
      "vpsubw      %%zmm5,%%zmm0,%%zmm0          \n"
      "vpsubw      %%zmm5,%%zmm2,%%zmm2          \n"
      "vpsrlw      $0x8,%%zmm0,%%zmm0            \n"
      "vpsrlw      $0x8,%%zmm2,%%zmm2            \n"
      "vpackuswb   %%zmm2,%%zmm0,%%zmm0          \n"  // mutates
      "vpermd      %%zmm0,%%zmm7,%%zmm0          \n"  // unmutate.
      "vmovups     %%zmm0,(%1)                   \n"

      "vmovups     (%0),%%zmm0                   \n"
      "vmovups     0x40(%0),%%zmm1               \n"
      "vmovups     0x80(%0),%%zmm2               \n"
      "vmovups     0xc0(%0),%%zmm6               \n"
      "vpmaddubsw  %%zmm4,%%zmm0,%%zmm0          \n"
      "vpmaddubsw  %%zmm4,%%zmm1,%%zmm1          \n"
      "vpmaddubsw  %%zmm4,%%zmm2,%%zmm2          \n"
      "vpmaddubsw  %%zmm4,%%zmm6,%%zmm6          \n"
      "vpmaddwd    %%zmm16,%%zmm0,%%zmm0         \n"
      "vpmaddwd    %%zmm16,%%zmm1,%%zmm1         \n"
      "vpmaddwd    %%zmm16,%%zmm2,%%zmm2         \n"
      "vpmaddwd    %%zmm16,%%zmm6,%%zmm6         \n"
      "vpackssdw   %%zmm1,%%zmm0,%%zmm0          \n"  // mutates
      "vpackssdw   %%zmm6,%%zmm2,%%zmm2          \n"
      "vpsubw      %%zmm5,%%zmm0,%%zmm0          \n"
      "vpsubw      %%zmm5,%%zmm2,%%zmm2          \n"
      "vpsrlw      $0x8,%%zmm0,%%zmm0            \n"
      "vpsrlw      $0x8,%%zmm2,%%zmm2            \n"
      "vpackuswb   %%zmm2,%%zmm0,%%zmm0          \n"  // mutates
      "vpermd      %%zmm0,%%zmm7,%%zmm0          \n"  // unmutate.
      "vmovups     %%zmm0,(%1,%2,1)              \n"
      "lea         0x100(%0),%0                  \n"
      "lea         0x40(%1),%1                   \n"
      "subl        $0x40,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),  // %0
        "+r"(dst_u),     // %1
        "+r"(dst_v),     // %2
#if defined(__i386__)
        "+m"(width)  // %3
#else
        "+rm"(width)  // %3
#endif
      : "r"(c),                      // %4
        "m"(kPermdARGBToY_AVX512BW)  // %5
      : "memory", "cc", "zmm0", "zmm1", "zmm2", "zmm3", "zmm4", "zmm5", "zmm6",
        "zmm7", "zmm16");
}
#endif  // HAS_ARGBTOUV444ROW_AVX512BW

#if !defined(LIBYUV_DISABLE_X86)

// ARGBARGB to AARRGGBB shuffle
static const lvec8 kShuffleAARRGGBB = {
    0, 4, 1, 5, 2, 6, 3, 7, 8, 12, 9, 13, 10, 14, 11, 15,
    0, 4, 1, 5, 2, 6, 3, 7, 8, 12, 9, 13, 10, 14, 11, 15,
};

// 8x2 -> 4x1 ARGB pixels converted to 4 U and 4 V
// ARGBToUV does rounding average of 4 ARGB pixels

#endif

#ifdef HAS_ARGBTOUVROW_AVX2

// 16x2 -> 8x1 ARGB pixels converted to 8 U and 8 V
// ARGBToUV does rounding average of 4 ARGB pixels
void ARGBToUVMatrixRow_AVX2(const uint8_t* src_argb,
                            int src_stride_argb,
                            uint8_t* dst_u,
                            uint8_t* dst_v,
                            int width,
                            const struct ArgbConstants* c) {
  asm volatile(
      "vbroadcastf128 0x20(%5),%%ymm4           \n"  // RGBToU
      "vbroadcastf128 0x40(%5),%%ymm5           \n"  // RGBToV
      "vpcmpeqb    %%ymm6,%%ymm6,%%ymm6          \n"  // 0x0101
      "vpabsb      %%ymm6,%%ymm6                 \n"
      "vmovdqa     %6,%%ymm7                     \n"  // kShuffleAARRGGBB
      "sub         %1,%2                         \n"

      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"  // Read 16x2 ARGB Pixels
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x00(%0,%4,1),%%ymm2          \n"
      "vmovdqu     0x20(%0,%4,1),%%ymm3          \n"
      "vpshufb     %%ymm7,%%ymm0,%%ymm0          \n"  // aarrggbb
      "vpshufb     %%ymm7,%%ymm1,%%ymm1          \n"
      "vpshufb     %%ymm7,%%ymm2,%%ymm2          \n"
      "vpshufb     %%ymm7,%%ymm3,%%ymm3          \n"
      "vpmaddubsw  %%ymm6,%%ymm0,%%ymm0          \n"  // 16x2 -> 8x2
      "vpmaddubsw  %%ymm6,%%ymm1,%%ymm1          \n"
      "vpmaddubsw  %%ymm6,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm6,%%ymm3,%%ymm3          \n"
      "vpaddw      %%ymm0,%%ymm2,%%ymm0          \n"  // 8x2 -> 8x1
      "vpaddw      %%ymm1,%%ymm3,%%ymm1          \n"
      "vpxor       %%ymm2,%%ymm2,%%ymm2          \n"  // 0 for vpavgw
      "vpsrlw      $1,%%ymm0,%%ymm0              \n"
      "vpsrlw      $1,%%ymm1,%%ymm1              \n"
      "vpavgw      %%ymm2,%%ymm0,%%ymm0          \n"
      "vpavgw      %%ymm2,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"  // mutates
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"  // 8 ARGB Pixels

      "vpsllw      $15,%%ymm6,%%ymm2             \n"  // 0x8000
      "vpmaddubsw  %%ymm5,%%ymm0,%%ymm1          \n"  // 8 V
      "vpmaddubsw  %%ymm4,%%ymm0,%%ymm0          \n"  // 8 U
      "vphaddw     %%ymm1,%%ymm0,%%ymm0          \n"  // uuuuvvvv uuuuvvvv
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"  // uuuuuuuu vvvvvvvv
      "vpsubw      %%ymm0,%%ymm2,%%ymm0          \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm0,%%ymm0,%%ymm0          \n"  // mutates 8U8u- 8V8v
      "vmovq       %%xmm0,(%1)                   \n"  // Write 8 U's
      "vextractf128 $0x1,%%ymm0,%%xmm0           \n"  // Copy V to low 8 bytes
      "vmovq       %%xmm0,0x00(%1,%2,1)          \n"  // Write 8 V's

      "lea         0x40(%0),%0                   \n"
      "lea         0x8(%1),%1                    \n"
      "subl        $0x10,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),  // %0
        "+r"(dst_u),     // %1
        "+r"(dst_v),     // %2
#if defined(__i386__)
        "+m"(width)  // %3
#else
        "+rm"(width)  // %3
#endif
      : "r"((intptr_t)(src_stride_argb)),  // %4
        "r"(c),                            // %5
        "m"(kShuffleAARRGGBB)              // %6
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ARGBTOUVROW_AVX2

// RGB to BT601 coefficients
// UB   0.875 coefficient = 112
// UG -0.5781 coefficient = -74
// UR -0.2969 coefficient = -38
// VB -0.1406 coefficient = -18
// VG -0.7344 coefficient = -94
// VR   0.875 coefficient = 112



#ifdef HAS_ARGBTOYROW_AVX2
void RGBAToYRow_AVX2(const uint8_t* src_rgba, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_rgba, dst_y, width, &kRgbaI601Constants);
}
#endif

#ifdef HAS_ARGBTOYROW_AVX2
void BGRAToYRow_AVX2(const uint8_t* src_bgra, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX2(src_bgra, dst_y, width, &kBgraI601Constants);
}
#endif


#ifdef HAS_ARGBTOYROW_AVX512BW
void ARGBToYRow_AVX512BW(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX512BW(src_argb, dst_y, width, &kArgbI601Constants);
}
#endif

#ifdef HAS_ARGBTOYROW_AVX512BW
void ARGBToYJRow_AVX512BW(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX512BW(src_argb, dst_y, width, &kArgbJPEGConstants);
}
#endif

#ifdef HAS_ARGBTOYROW_AVX512BW
void ABGRToYRow_AVX512BW(const uint8_t* src_abgr, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX512BW(src_abgr, dst_y, width, &kAbgrI601Constants);
}
#endif

#ifdef HAS_ARGBTOYROW_AVX512BW
void ABGRToYJRow_AVX512BW(const uint8_t* src_abgr, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX512BW(src_abgr, dst_y, width, &kAbgrJPEGConstants);
}
#endif

#ifdef HAS_ARGBTOYROW_AVX512BW
void RGBAToYRow_AVX512BW(const uint8_t* src_rgba, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX512BW(src_rgba, dst_y, width, &kRgbaI601Constants);
}
#endif

#ifdef HAS_ARGBTOYROW_AVX512BW
void RGBAToYJRow_AVX512BW(const uint8_t* src_rgba, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX512BW(src_rgba, dst_y, width, &kRgbaJPEGConstants);
}
#endif

#ifdef HAS_ARGBTOYROW_AVX512BW
void BGRAToYRow_AVX512BW(const uint8_t* src_bgra, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_AVX512BW(src_bgra, dst_y, width, &kBgraI601Constants);
}
#endif

#ifdef HAS_ARGBTOUV444ROW_AVX2
void ARGBToUV444Row_AVX2(const uint8_t* src_argb,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width) {
  ARGBToUV444MatrixRow_AVX2(src_argb, dst_u, dst_v, width, &kArgbI601Constants);
}
#endif  // HAS_ARGBTOUV444ROW_AVX2

#ifdef HAS_ARGBTOUV444ROW_AVX512BW
void ARGBToUV444Row_AVX512BW(const uint8_t* src_argb,
                             uint8_t* dst_u,
                             uint8_t* dst_v,
                             int width) {
  ARGBToUV444MatrixRow_AVX512BW(src_argb, dst_u, dst_v, width,
                                &kArgbI601Constants);
}
#endif  // HAS_ARGBTOUV444ROW_AVX512BW


#ifdef HAS_ARGBTOUVROW_AVX2
void ARGBToUVRow_AVX2(const uint8_t* src_argb,
                      int src_stride_argb,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width) {
  ARGBToUVMatrixRow_AVX2(src_argb, src_stride_argb, dst_u, dst_v, width,
                         &kArgbI601Constants);
}

void ABGRToUVRow_AVX2(const uint8_t* src_abgr,
                      int src_stride_abgr,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width) {
  ARGBToUVMatrixRow_AVX2(src_abgr, src_stride_abgr, dst_u, dst_v, width,
                         &kAbgrI601Constants);
}
#endif  // HAS_ARGBTOUVROW_AVX2


#ifdef HAS_ARGBTOUVJ444ROW_AVX2
void ARGBToUVJ444Row_AVX2(const uint8_t* src_argb,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width) {
  ARGBToUV444MatrixRow_AVX2(src_argb, dst_u, dst_v, width, &kArgbJPEGConstants);
}
#endif  // HAS_ARGBTOUVJ444ROW_AVX2

#ifdef HAS_ARGBTOUVJ444ROW_AVX512BW
void ARGBToUVJ444Row_AVX512BW(const uint8_t* src_argb,
                              uint8_t* dst_u,
                              uint8_t* dst_v,
                              int width) {
  ARGBToUV444MatrixRow_AVX512BW(src_argb, dst_u, dst_v, width,
                                &kArgbJPEGConstants);
}
#endif  // HAS_ARGBTOUVJ444ROW_AVX512BW



#ifdef HAS_ARGBTOUVJROW_AVX2
void ARGBToUVJRow_AVX2(const uint8_t* src_argb,
                       int src_stride_argb,
                       uint8_t* dst_u,
                       uint8_t* dst_v,
                       int width) {
  ARGBToUVMatrixRow_AVX2(src_argb, src_stride_argb, dst_u, dst_v, width,
                         &kArgbJPEGConstants);
}
#endif  // HAS_ARGBTOUVJROW_AVX2

#ifdef HAS_ABGRTOUVJROW_AVX2
void ABGRToUVJRow_AVX2(const uint8_t* src_abgr,
                       int src_stride_abgr,
                       uint8_t* dst_u,
                       uint8_t* dst_v,
                       int width) {
  ARGBToUVMatrixRow_AVX2(src_abgr, src_stride_abgr, dst_u, dst_v, width,
                         &kAbgrJPEGConstants);
}
#endif  // HAS_ABGRTOUVJROW_AVX2

#ifdef HAS_ARGBTOUVROW_AVX512BW

// 32x2 -> 16x1 ARGB pixels converted to 16 U and 16 V
// ARGBToUV does rounding average of 4 ARGB pixels

void ARGBToUVMatrixRow_AVX512BW(const uint8_t* src_argb,
                                int src_stride_argb,
                                uint8_t* dst_u,
                                uint8_t* dst_v,
                                int width,
                                const struct ArgbConstants* c) {
  asm volatile(
      "vbroadcasti64x4 0x20(%5),%%zmm4               \n"  // RGBToU
      "vbroadcasti64x4 0x40(%5),%%zmm5               \n"  // RGBToV
      "vpternlogd  $0xff,%%zmm16,%%zmm16,%%zmm16 \n"
      "vpabsb      %%zmm16,%%zmm6                \n"  // 0x0101
      "vpsllw      $15,%%zmm16,%%zmm17           \n"  // 0x8000
      "vbroadcasti64x4 %6,%%zmm7                     \n"  // kShuffleAARRGGBB
      "vmovups     %7,%%zmm18                    \n"  // kPermdARGBToY_AVX512BW
      "vmovups     %8,%%zmm19                    \n"  // kPermdARGBToUV_AVX512BW
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:          \n"
      "vmovups     (%0),%%zmm0                   \n"  // Read 32x2 ARGB Pixels
      "vmovups     0x40(%0),%%zmm1               \n"
      "vmovups     0x00(%0,%4,1),%%zmm2          \n"
      "vmovups     0x40(%0,%4,1),%%zmm3          \n"
      "vpshufb     %%zmm7,%%zmm0,%%zmm0          \n"  // aarrggbb
      "vpshufb     %%zmm7,%%zmm1,%%zmm1          \n"
      "vpshufb     %%zmm7,%%zmm2,%%zmm2          \n"
      "vpshufb     %%zmm7,%%zmm3,%%zmm3          \n"
      "vpmaddubsw  %%zmm6,%%zmm0,%%zmm0          \n"  // 32x2 -> 16x2
      "vpmaddubsw  %%zmm6,%%zmm1,%%zmm1          \n"
      "vpmaddubsw  %%zmm6,%%zmm2,%%zmm2          \n"
      "vpmaddubsw  %%zmm6,%%zmm3,%%zmm3          \n"
      "vpaddw      %%zmm0,%%zmm2,%%zmm0          \n"  // 16x2 -> 16x1
      "vpaddw      %%zmm1,%%zmm3,%%zmm1          \n"
      "vpxorq      %%zmm2,%%zmm2,%%zmm2          \n"  // 0 for vpavgw
      "vpsrlw      $1,%%zmm0,%%zmm0              \n"
      "vpsrlw      $1,%%zmm1,%%zmm1              \n"
      "vpavgw      %%zmm2,%%zmm0,%%zmm0          \n"
      "vpavgw      %%zmm2,%%zmm1,%%zmm1          \n"
      "vpackuswb   %%zmm1,%%zmm0,%%zmm0          \n"  // mutates
      "vpermd      %%zmm0,%%zmm19,%%zmm0         \n"  // unscramble pixels

      "vpmaddubsw  %%zmm4,%%zmm0,%%zmm1          \n"  // 16 U
      "vpmaddubsw  %%zmm5,%%zmm0,%%zmm0          \n"  // 16 V
      "vpmaddwd    %%zmm16,%%zmm1,%%zmm1         \n"
      "vpmaddwd    %%zmm16,%%zmm0,%%zmm0         \n"
      "vpackssdw   %%zmm0,%%zmm1,%%zmm0          \n"  // mutates (U in lower, V in upper)
      "vpaddw      %%zmm17,%%zmm0,%%zmm0         \n"
      "vpsrlw      $0x8,%%zmm0,%%zmm0            \n"
      "vpackuswb   %%zmm0,%%zmm0,%%zmm0          \n"  // mutates
      "vpermd      %%zmm0,%%zmm18,%%zmm0         \n"  // unmutate

      "vmovdqu     %%xmm0,(%1)                   \n"  // Write 16 U's
      "vextracti32x4 $0x1,%%zmm0,%%xmm0          \n"
      "vmovdqu     %%xmm0,0x00(%1,%2,1)          \n"  // Write 16 V's

      "lea         0x80(%0),%0                   \n"
      "lea         0x10(%1),%1                   \n"
      "subl        $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),  // %0
        "+r"(dst_u),     // %1
        "+r"(dst_v),     // %2
#if defined(__i386__)
        "+m"(width)  // %3
#else
        "+rm"(width)  // %3
#endif
      : "r"((intptr_t)(src_stride_argb)),  // %4
        "r"(c),                            // %5
        "m"(kShuffleAARRGGBB),             // %6
        "m"(kPermdARGBToY_AVX512BW),       // %7
        "m"(kPermdARGBToUV_AVX512BW)       // %8
      : "memory", "cc", "zmm0", "zmm1", "zmm2", "zmm3", "zmm4", "zmm5", "zmm6",
        "zmm7", "zmm16", "zmm17", "zmm18", "zmm19");
}

void ARGBToUVRow_AVX512BW(const uint8_t* src_argb,
                          int src_stride_argb,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width) {
  ARGBToUVMatrixRow_AVX512BW(src_argb, src_stride_argb, dst_u, dst_v, width,
                             &kArgbI601Constants);
}

void ABGRToUVRow_AVX512BW(const uint8_t* src_abgr,
                          int src_stride_abgr,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width) {
  ARGBToUVMatrixRow_AVX512BW(src_abgr, src_stride_abgr, dst_u, dst_v, width,
                             &kAbgrI601Constants);
}

#ifdef HAS_ARGBTOUVJROW_AVX512BW
void ARGBToUVJRow_AVX512BW(const uint8_t* src_argb,
                           int src_stride_argb,
                           uint8_t* dst_u,
                           uint8_t* dst_v,
                           int width) {
  ARGBToUVMatrixRow_AVX512BW(src_argb, src_stride_argb, dst_u, dst_v, width,
                             &kArgbJPEGConstants);
}
#endif  // HAS_ARGBTOUVJROW_AVX512BW

#ifdef HAS_ABGRTOUVJROW_AVX512BW
void ABGRToUVJRow_AVX512BW(const uint8_t* src_abgr,
                           int src_stride_abgr,
                           uint8_t* dst_u,
                           uint8_t* dst_v,
                           int width) {
  ARGBToUVMatrixRow_AVX512BW(src_abgr, src_stride_abgr, dst_u, dst_v, width,
                             &kAbgrJPEGConstants);
}
#endif  // HAS_ABGRTOUVJROW_AVX512BW
#endif  // HAS_ARGBTOUVROW_AVX512BW





// Read 16 UV from 444
#define READYUV444_AVX2                                               \
  "vmovdqu    (%[u_buf]),%%xmm3                                   \n" \
  "vmovdqu    0x00(%[u_buf],%[v_buf],1),%%xmm1                    \n" \
  "lea        0x10(%[u_buf]),%[u_buf]                             \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                                 \n" \
  "vpermq     $0xd8,%%ymm1,%%ymm1                                 \n" \
  "vpunpcklbw %%ymm1,%%ymm3,%%ymm3                                \n" \
  "vmovdqu    (%[y_buf]),%%xmm4                                   \n" \
  "vpermq     $0xd8,%%ymm4,%%ymm4                                 \n" \
  "vpunpcklbw %%ymm4,%%ymm4,%%ymm4                                \n" \
  "lea        0x10(%[y_buf]),%[y_buf]                             \n"

// Read 8 UV from 422, upsample to 16 UV.
#define READYUV422_AVX2                                               \
  "vmovq      (%[u_buf]),%%xmm3                                   \n" \
  "vmovq      0x00(%[u_buf],%[v_buf],1),%%xmm1                    \n" \
  "lea        0x8(%[u_buf]),%[u_buf]                              \n" \
  "vpunpcklbw %%ymm1,%%ymm3,%%ymm3                                \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                                 \n" \
  "vpunpcklwd %%ymm3,%%ymm3,%%ymm3                                \n" \
  "vmovdqu    (%[y_buf]),%%xmm4                                   \n" \
  "vpermq     $0xd8,%%ymm4,%%ymm4                                 \n" \
  "vpunpcklbw %%ymm4,%%ymm4,%%ymm4                                \n" \
  "lea        0x10(%[y_buf]),%[y_buf]                             \n"

#define READYUV422_AVX512BW                                           \
  "vmovdqu    (%[u_buf]),%%xmm3                                   \n" \
  "vmovdqu    0x00(%[u_buf],%[v_buf],1),%%xmm1                    \n" \
  "vpermq     %%zmm3,%%zmm16,%%zmm3                               \n" \
  "vpermq     %%zmm1,%%zmm16,%%zmm1                               \n" \
  "lea        0x10(%[u_buf]),%[u_buf]                             \n" \
  "vpunpcklbw %%zmm1,%%zmm3,%%zmm3                                \n" \
  "vpermq     $0xd8,%%zmm3,%%zmm3                                 \n" \
  "vpunpcklwd %%zmm3,%%zmm3,%%zmm3                                \n" \
  "vmovups    (%[y_buf]),%%ymm4                                   \n" \
  "vpermq     %%zmm4,%%zmm17,%%zmm4                               \n" \
  "vpermq     $0xd8,%%zmm4,%%zmm4                                 \n" \
  "vpunpcklbw %%zmm4,%%zmm4,%%zmm4                                \n" \
  "lea        0x20(%[y_buf]),%[y_buf]                             \n"

// Read 8 UV from 210, upsample to 16 UV
// TODO(fbarchard): Consider vpshufb to replace pack/unpack
// TODO(fbarchard): Consider vunpcklpd to combine the 2 registers into 1.
#define READYUV210_AVX2                                            \
  "vmovdqu    (%[u_buf]),%%xmm3                                \n" \
  "vmovdqu    0x00(%[u_buf],%[v_buf],1),%%xmm1                 \n" \
  "lea        0x10(%[u_buf]),%[u_buf]                          \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                              \n" \
  "vpermq     $0xd8,%%ymm1,%%ymm1                              \n" \
  "vpunpcklwd %%ymm1,%%ymm3,%%ymm3                             \n" \
  "vpsraw     $2,%%ymm3,%%ymm3                                 \n" \
  "vpackuswb  %%ymm3,%%ymm3,%%ymm3                             \n" \
  "vpunpcklwd %%ymm3,%%ymm3,%%ymm3                             \n" \
  "vmovdqu    (%[y_buf]),%%ymm4                                \n" \
  "vpsllw     $6,%%ymm4,%%ymm2                                 \n" \
  "vpsrlw     $4,%%ymm4,%%ymm4                                 \n" \
  "vpaddw     %%ymm2,%%ymm4,%%ymm4                             \n" \
  "lea        0x20(%[y_buf]),%[y_buf]                          \n"

// Read 8 UV from 210, upsample to 16 UV. With 16 Alpha.
#define READYUVA210_AVX2                                           \
  "vmovdqu    (%[u_buf]),%%xmm3                                \n" \
  "vmovdqu    0x00(%[u_buf],%[v_buf],1),%%xmm1                 \n" \
  "lea        0x10(%[u_buf]),%[u_buf]                          \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                              \n" \
  "vpermq     $0xd8,%%ymm1,%%ymm1                              \n" \
  "vpunpcklwd %%ymm1,%%ymm3,%%ymm3                             \n" \
  "vpsraw     $2,%%ymm3,%%ymm3                                 \n" \
  "vpackuswb  %%ymm3,%%ymm3,%%ymm3                             \n" \
  "vpunpcklwd %%ymm3,%%ymm3,%%ymm3                             \n" \
  "vmovdqu    (%[y_buf]),%%ymm4                                \n" \
  "vpsllw     $6,%%ymm4,%%ymm2                                 \n" \
  "vpsrlw     $4,%%ymm4,%%ymm4                                 \n" \
  "vpaddw     %%ymm2,%%ymm4,%%ymm4                             \n" \
  "lea        0x20(%[y_buf]),%[y_buf]                          \n" \
  "vmovdqu    (%[a_buf]),%%ymm5                                \n" \
  "vpsraw     $2,%%ymm5,%%ymm5                                 \n" \
  "vpackuswb  %%ymm5,%%ymm5,%%ymm5                             \n" \
  "lea        0x20(%[a_buf]),%[a_buf]                          \n"

// Read 16 UV from 410
#define READYUV410_AVX2                                            \
  "vmovdqu    (%[u_buf]),%%ymm3                                \n" \
  "vmovdqu    0x00(%[u_buf],%[v_buf],1),%%ymm2                 \n" \
  "lea        0x20(%[u_buf]),%[u_buf]                          \n" \
  "vpsraw     $2,%%ymm3,%%ymm3                                 \n" \
  "vpsraw     $2,%%ymm2,%%ymm2                                 \n" \
  "vpunpckhwd %%ymm2,%%ymm3,%%ymm1                             \n" \
  "vpunpcklwd %%ymm2,%%ymm3,%%ymm3                             \n" \
  "vpackuswb  %%ymm1,%%ymm3,%%ymm3                             \n" \
  "vmovdqu    (%[y_buf]),%%ymm4                                \n" \
  "vpsllw     $6,%%ymm4,%%ymm2                                 \n" \
  "vpsrlw     $4,%%ymm4,%%ymm4                                 \n" \
  "vpaddw     %%ymm2,%%ymm4,%%ymm4                             \n" \
  "lea        0x20(%[y_buf]),%[y_buf]                          \n"

// Read 8 UV from 212 12 bit, upsample to 16 UV
#define READYUV212_AVX2                                            \
  "vmovdqu    (%[u_buf]),%%xmm3                                \n" \
  "vmovdqu    0x00(%[u_buf],%[v_buf],1),%%xmm1                 \n" \
  "lea        0x10(%[u_buf]),%[u_buf]                          \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                              \n" \
  "vpermq     $0xd8,%%ymm1,%%ymm1                              \n" \
  "vpunpcklwd %%ymm1,%%ymm3,%%ymm3                             \n" \
  "vpsraw     $0x4,%%ymm3,%%ymm3                               \n" \
  "vpackuswb  %%ymm3,%%ymm3,%%ymm3                             \n" \
  "vpunpcklwd %%ymm3,%%ymm3,%%ymm3                             \n" \
  "vmovdqu    (%[y_buf]),%%ymm4                                \n" \
  "vpsllw     $4,%%ymm4,%%ymm2                                 \n" \
  "vpsrlw     $8,%%ymm4,%%ymm4                                 \n" \
  "vpaddw     %%ymm2,%%ymm4,%%ymm4                             \n" \
  "lea        0x20(%[y_buf]),%[y_buf]                          \n"

// Read 16 UV from 410. With 16 Alpha.
#define READYUVA410_AVX2                                           \
  "vmovdqu    (%[u_buf]),%%ymm3                                \n" \
  "vmovdqu    0x00(%[u_buf],%[v_buf],1),%%ymm2                 \n" \
  "lea        0x20(%[u_buf]),%[u_buf]                          \n" \
  "vpsraw     $2,%%ymm3,%%ymm3                                 \n" \
  "vpsraw     $2,%%ymm2,%%ymm2                                 \n" \
  "vpunpckhwd %%ymm2,%%ymm3,%%ymm1                             \n" \
  "vpunpcklwd %%ymm2,%%ymm3,%%ymm3                             \n" \
  "vpackuswb  %%ymm1,%%ymm3,%%ymm3                             \n" \
  "vmovdqu    (%[y_buf]),%%ymm4                                \n" \
  "vpsllw     $6,%%ymm4,%%ymm2                                 \n" \
  "vpsrlw     $4,%%ymm4,%%ymm4                                 \n" \
  "vpaddw     %%ymm2,%%ymm4,%%ymm4                             \n" \
  "lea        0x20(%[y_buf]),%[y_buf]                          \n" \
  "vmovdqu    (%[a_buf]),%%ymm5                                \n" \
  "vpsraw     $2,%%ymm5,%%ymm5                                 \n" \
  "vpackuswb  %%ymm5,%%ymm5,%%ymm5                             \n" \
  "lea        0x20(%[a_buf]),%[a_buf]                          \n"

// Read 16 UV from 444.  With 16 Alpha.
#define READYUVA444_AVX2                                              \
  "vmovdqu    (%[u_buf]),%%xmm3                                   \n" \
  "vmovdqu    0x00(%[u_buf],%[v_buf],1),%%xmm1                    \n" \
  "lea        0x10(%[u_buf]),%[u_buf]                             \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                                 \n" \
  "vpermq     $0xd8,%%ymm1,%%ymm1                                 \n" \
  "vpunpcklbw %%ymm1,%%ymm3,%%ymm3                                \n" \
  "vmovdqu    (%[y_buf]),%%xmm4                                   \n" \
  "vpermq     $0xd8,%%ymm4,%%ymm4                                 \n" \
  "vpunpcklbw %%ymm4,%%ymm4,%%ymm4                                \n" \
  "lea        0x10(%[y_buf]),%[y_buf]                             \n" \
  "vmovdqu    (%[a_buf]),%%xmm5                                   \n" \
  "vpermq     $0xd8,%%ymm5,%%ymm5                                 \n" \
  "lea        0x10(%[a_buf]),%[a_buf]                             \n"

// Read 8 UV from 422, upsample to 16 UV.  With 16 Alpha.
#define READYUVA422_AVX2                                              \
  "vmovq      (%[u_buf]),%%xmm3                                   \n" \
  "vmovq      0x00(%[u_buf],%[v_buf],1),%%xmm1                    \n" \
  "lea        0x8(%[u_buf]),%[u_buf]                              \n" \
  "vpunpcklbw %%ymm1,%%ymm3,%%ymm3                                \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                                 \n" \
  "vpunpcklwd %%ymm3,%%ymm3,%%ymm3                                \n" \
  "vmovdqu    (%[y_buf]),%%xmm4                                   \n" \
  "vpermq     $0xd8,%%ymm4,%%ymm4                                 \n" \
  "vpunpcklbw %%ymm4,%%ymm4,%%ymm4                                \n" \
  "lea        0x10(%[y_buf]),%[y_buf]                             \n" \
  "vmovdqu    (%[a_buf]),%%xmm5                                   \n" \
  "vpermq     $0xd8,%%ymm5,%%ymm5                                 \n" \
  "lea        0x10(%[a_buf]),%[a_buf]                             \n"

// Read 8 UV from NV12, upsample to 16 UV.
#define READNV12_AVX2                                                 \
  "vmovdqu    (%[uv_buf]),%%xmm3                                  \n" \
  "lea        0x10(%[uv_buf]),%[uv_buf]                           \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                                 \n" \
  "vpunpcklwd %%ymm3,%%ymm3,%%ymm3                                \n" \
  "vmovdqu    (%[y_buf]),%%xmm4                                   \n" \
  "vpermq     $0xd8,%%ymm4,%%ymm4                                 \n" \
  "vpunpcklbw %%ymm4,%%ymm4,%%ymm4                                \n" \
  "lea        0x10(%[y_buf]),%[y_buf]                             \n"

// Read 8 VU from NV21, upsample to 16 UV.
#define READNV21_AVX2                                                 \
  "vmovdqu    (%[vu_buf]),%%xmm3                                  \n" \
  "lea        0x10(%[vu_buf]),%[vu_buf]                           \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                                 \n" \
  "vpshufb     %[kShuffleNV21], %%ymm3, %%ymm3                    \n" \
  "vmovdqu    (%[y_buf]),%%xmm4                                   \n" \
  "vpermq     $0xd8,%%ymm4,%%ymm4                                 \n" \
  "vpunpcklbw %%ymm4,%%ymm4,%%ymm4                                \n" \
  "lea        0x10(%[y_buf]),%[y_buf]                             \n"

// Read 4 UV from P210, upsample to 8 UV
#define READP210_AVX2                                                 \
  "vmovdqu    (%[uv_buf]),%%ymm3                                  \n" \
  "lea        0x20(%[uv_buf]),%[uv_buf]                           \n" \
  "vpsrlw     $0x8,%%ymm3,%%ymm3                                  \n" \
  "vpackuswb  %%ymm3,%%ymm3,%%ymm3                                \n" \
  "vpunpcklwd %%ymm3,%%ymm3,%%ymm3                                \n" \
  "vmovdqu    (%[y_buf]),%%ymm4                                   \n" \
  "lea        0x20(%[y_buf]),%[y_buf]                             \n"

// Read 8 UV from P410
#define READP410_AVX2                                                 \
  "vmovdqu    (%[uv_buf]),%%ymm3                                  \n" \
  "vmovdqu    0x20(%[uv_buf]),%%ymm1                              \n" \
  "lea        0x40(%[uv_buf]),%[uv_buf]                           \n" \
  "vpsrlw     $0x8,%%ymm3,%%ymm3                                  \n" \
  "vpsrlw     $0x8,%%ymm1,%%ymm1                                  \n" \
  "vpackuswb  %%ymm1,%%ymm3,%%ymm3                                \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                                 \n" \
  "vmovdqu    (%[y_buf]),%%ymm4                                   \n" \
  "lea        0x20(%[y_buf]),%[y_buf]                             \n"

// Read 8 YUY2 with 16 Y and upsample 8 UV to 16 UV.
// ymm6 kShuffleYUY2Y,
// ymm7 kShuffleYUY2UV
#define READYUY2_AVX2                                                 \
  "vmovdqu    (%[yuy2_buf]),%%ymm1                                \n" \
  "vpshufb    %%ymm6,%%ymm1,%%ymm4                                \n" \
  "vpshufb    %%ymm7,%%ymm1,%%ymm3                                \n" \
  "lea        0x20(%[yuy2_buf]),%[yuy2_buf]                       \n"

// Read 8 UYVY with 16 Y and upsample 8 UV to 16 UV.
// ymm6 kShuffleUYVYY,
// ymm7 kShuffleUYVYUV
#define READUYVY_AVX2                                                 \
  "vmovdqu    (%[uyvy_buf]),%%ymm1                                \n" \
  "vpshufb    %%ymm6,%%ymm1,%%ymm4                                \n" \
  "vpshufb    %%ymm7,%%ymm1,%%ymm3                                \n" \
  "lea        0x20(%[uyvy_buf]),%[uyvy_buf]                       \n"

// TODO(fbarchard): Remove broadcastb
#if defined(__x86_64__)
#define YUVTORGB_SETUP_AVX2(yuvconstants)                             \
  "vpcmpeqb    %%xmm13,%%xmm13,%%xmm13                            \n" \
  "vmovdqa     (%[yuvconstants]),%%ymm8                           \n" \
  "vpsllw      $7,%%xmm13,%%xmm13                                 \n" \
  "vmovdqa     32(%[yuvconstants]),%%ymm9                         \n" \
  "vpbroadcastb %%xmm13,%%ymm13                                   \n" \
  "vmovdqa     64(%[yuvconstants]),%%ymm10                        \n" \
  "vmovdqa     96(%[yuvconstants]),%%ymm11                        \n" \
  "vmovdqa     128(%[yuvconstants]),%%ymm12                       \n"

#define YUVTORGB_SETUP_AVX512BW(yuvconstants)                         \
  "vpcmpeqb   %%xmm13,%%xmm13,%%xmm13                             \n" \
  "movdqa     (%[yuvconstants]),%%xmm8                            \n" \
  "vpbroadcastq %%xmm8, %%zmm8                                    \n" \
  "vpsllw     $7,%%xmm13,%%xmm13                                  \n" \
  "vpbroadcastb %%xmm13,%%zmm13                                   \n" \
  "movq       32(%[yuvconstants]),%%xmm9                          \n" \
  "vpbroadcastq %%xmm9,%%zmm9                                     \n" \
  "movq       64(%[yuvconstants]),%%xmm10                         \n" \
  "vpbroadcastq %%xmm10,%%zmm10                                   \n" \
  "movq       96(%[yuvconstants]),%%xmm11                         \n" \
  "vpbroadcastq %%xmm11,%%zmm11                                   \n" \
  "movq       128(%[yuvconstants]),%%xmm12                        \n" \
  "vpbroadcastq %%xmm12,%%zmm12                                   \n" \
  "vmovups    (%[quadsplitperm]),%%zmm16                          \n" \
  "vmovups    (%[dquadsplitperm]),%%zmm17                         \n" \
  "vmovups    (%[unperm]),%%zmm18                                 \n"

#define YUVTORGB16_AVX2(yuvconstants)                                 \
  "vpsubb      %%ymm13,%%ymm3,%%ymm3                              \n" \
  "vpmulhuw    %%ymm11,%%ymm4,%%ymm4                              \n" \
  "vpmaddubsw  %%ymm3,%%ymm8,%%ymm0                               \n" \
  "vpmaddubsw  %%ymm3,%%ymm9,%%ymm1                               \n" \
  "vpmaddubsw  %%ymm3,%%ymm10,%%ymm2                              \n" \
  "vpaddw      %%ymm4,%%ymm12,%%ymm4                              \n" \
  "vpaddsw     %%ymm4,%%ymm0,%%ymm0                               \n" \
  "vpsubsw     %%ymm1,%%ymm4,%%ymm1                               \n" \
  "vpaddsw     %%ymm4,%%ymm2,%%ymm2                               \n"

#define YUVTORGB16_AVX512BW(yuvconstants)                             \
  "vpsubb      %%zmm13,%%zmm3,%%zmm3                              \n" \
  "vpmulhuw    %%zmm11,%%zmm4,%%zmm4                              \n" \
  "vpmaddubsw  %%zmm3,%%zmm8,%%zmm0                               \n" \
  "vpmaddubsw  %%zmm3,%%zmm9,%%zmm1                               \n" \
  "vpmaddubsw  %%zmm3,%%zmm10,%%zmm2                              \n" \
  "vpaddw      %%zmm4,%%zmm12,%%zmm4                              \n" \
  "vpaddsw     %%zmm4,%%zmm0,%%zmm0                               \n" \
  "vpsubsw     %%zmm1,%%zmm4,%%zmm1                               \n" \
  "vpaddsw     %%zmm4,%%zmm2,%%zmm2                               \n"

#define YUVTORGB_REGS_AVX2 "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13",
#define YUVTORGB_REGS_AVX512BW \
  "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm16", "xmm17", "xmm18",

#else  // Convert 16 pixels: 16 UV and 16 Y.

#define YUVTORGB_SETUP_AVX2(yuvconstants)
#define YUVTORGB16_AVX2(yuvconstants)                                 \
  "vpcmpeqb    %%xmm0,%%xmm0,%%xmm0                               \n" \
  "vpsllw      $7,%%xmm0,%%xmm0                                   \n" \
  "vpbroadcastb %%xmm0,%%ymm0                                     \n" \
  "vpsubb      %%ymm0,%%ymm3,%%ymm3                               \n" \
  "vpmulhuw    96(%[yuvconstants]),%%ymm4,%%ymm4                  \n" \
  "vmovdqa     (%[yuvconstants]),%%ymm0                           \n" \
  "vmovdqa     32(%[yuvconstants]),%%ymm1                         \n" \
  "vmovdqa     64(%[yuvconstants]),%%ymm2                         \n" \
  "vpmaddubsw  %%ymm3,%%ymm0,%%ymm0                               \n" \
  "vpmaddubsw  %%ymm3,%%ymm1,%%ymm1                               \n" \
  "vpmaddubsw  %%ymm3,%%ymm2,%%ymm2                               \n" \
  "vmovdqa     128(%[yuvconstants]),%%ymm3                        \n" \
  "vpaddw      %%ymm4,%%ymm3,%%ymm4                               \n" \
  "vpaddsw     %%ymm4,%%ymm0,%%ymm0                               \n" \
  "vpsubsw     %%ymm1,%%ymm4,%%ymm1                               \n" \
  "vpaddsw     %%ymm4,%%ymm2,%%ymm2                               \n"

#define YUVTORGB_REGS_AVX2
#endif

#define YUVTORGB_AVX2(yuvconstants)                                   \
  YUVTORGB16_AVX2(yuvconstants)                                       \
  "vpsraw      $0x6,%%ymm0,%%ymm0                                 \n" \
  "vpsraw      $0x6,%%ymm1,%%ymm1                                 \n" \
  "vpsraw      $0x6,%%ymm2,%%ymm2                                 \n" \
  "vpackuswb   %%ymm0,%%ymm0,%%ymm0                               \n" \
  "vpackuswb   %%ymm1,%%ymm1,%%ymm1                               \n" \
  "vpackuswb   %%ymm2,%%ymm2,%%ymm2                               \n"

#define YUVTORGB_AVX512BW(yuvconstants)                               \
  YUVTORGB16_AVX512BW(yuvconstants)                                   \
  "vpsraw     $0x6,%%zmm0,%%zmm0                                  \n" \
  "vpsraw     $0x6,%%zmm1,%%zmm1                                  \n" \
  "vpsraw     $0x6,%%zmm2,%%zmm2                                  \n" \
  "vpackuswb  %%zmm0,%%zmm0,%%zmm0                                \n" \
  "vpackuswb  %%zmm1,%%zmm1,%%zmm1                                \n" \
  "vpackuswb  %%zmm2,%%zmm2,%%zmm2                                \n"

// Store 16 ARGB values.
#define STOREARGB_AVX2                                                \
  "vpunpcklbw %%ymm1,%%ymm0,%%ymm0                                \n" \
  "vpermq     $0xd8,%%ymm0,%%ymm0                                 \n" \
  "vpunpcklbw %%ymm5,%%ymm2,%%ymm2                                \n" \
  "vpermq     $0xd8,%%ymm2,%%ymm2                                 \n" \
  "vpunpcklwd %%ymm2,%%ymm0,%%ymm1                                \n" \
  "vpunpckhwd %%ymm2,%%ymm0,%%ymm0                                \n" \
  "vmovdqu    %%ymm1,(%[dst_argb])                                \n" \
  "vmovdqu    %%ymm0,0x20(%[dst_argb])                            \n" \
  "lea        0x40(%[dst_argb]), %[dst_argb]                      \n"

// Store 32 ARGB values.
#define STOREARGB_AVX512BW                                            \
  "vpunpcklbw %%zmm1,%%zmm0,%%zmm0                                \n" \
  "vpermq     %%zmm0,%%zmm18,%%zmm0                               \n" \
  "vpunpcklbw %%zmm5,%%zmm2,%%zmm2                                \n" \
  "vpermq     %%zmm2,%%zmm18,%%zmm2                               \n" \
  "vpunpcklwd %%zmm2,%%zmm0,%%zmm1                                \n" \
  "vpunpckhwd %%zmm2,%%zmm0,%%zmm0                                \n" \
  "vmovups    %%zmm1,(%[dst_argb])                                \n" \
  "vmovups    %%zmm0,0x40(%[dst_argb])                            \n" \
  "lea        0x80(%[dst_argb]), %[dst_argb]                      \n"

// Store 16 AR30 values.
#define STOREAR30_AVX2                                                \
  "vpsraw     $0x4,%%ymm0,%%ymm0                                  \n" \
  "vpsraw     $0x4,%%ymm1,%%ymm1                                  \n" \
  "vpsraw     $0x4,%%ymm2,%%ymm2                                  \n" \
  "vpminsw    %%ymm7,%%ymm0,%%ymm0                                \n" \
  "vpminsw    %%ymm7,%%ymm1,%%ymm1                                \n" \
  "vpminsw    %%ymm7,%%ymm2,%%ymm2                                \n" \
  "vpmaxsw    %%ymm6,%%ymm0,%%ymm0                                \n" \
  "vpmaxsw    %%ymm6,%%ymm1,%%ymm1                                \n" \
  "vpmaxsw    %%ymm6,%%ymm2,%%ymm2                                \n" \
  "vpsllw     $0x4,%%ymm2,%%ymm2                                  \n" \
  "vpermq     $0xd8,%%ymm0,%%ymm0                                 \n" \
  "vpermq     $0xd8,%%ymm1,%%ymm1                                 \n" \
  "vpermq     $0xd8,%%ymm2,%%ymm2                                 \n" \
  "vpunpckhwd %%ymm2,%%ymm0,%%ymm3                                \n" \
  "vpunpcklwd %%ymm2,%%ymm0,%%ymm0                                \n" \
  "vpunpckhwd %%ymm5,%%ymm1,%%ymm2                                \n" \
  "vpunpcklwd %%ymm5,%%ymm1,%%ymm1                                \n" \
  "vpslld     $0xa,%%ymm1,%%ymm1                                  \n" \
  "vpslld     $0xa,%%ymm2,%%ymm2                                  \n" \
  "vpor       %%ymm1,%%ymm0,%%ymm0                                \n" \
  "vpor       %%ymm2,%%ymm3,%%ymm3                                \n" \
  "vmovdqu    %%ymm0,(%[dst_ar30])                                \n" \
  "vmovdqu    %%ymm3,0x20(%[dst_ar30])                            \n" \
  "lea        0x40(%[dst_ar30]), %[dst_ar30]                      \n"

#ifdef HAS_I444TOARGBROW_AVX2
// 16 pixels
// 16 UV values with 16 Y producing 16 ARGB (64 bytes).
void OMITFP I444ToARGBRow_AVX2(const uint8_t* y_buf,
                               const uint8_t* u_buf,
                               const uint8_t* v_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

    LABELALIGN
      "1:          \n"
    READYUV444_AVX2
    YUVTORGB_AVX2(yuvconstants)
    STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}
#endif  // HAS_I444TOARGBROW_AVX2

#if defined(HAS_I422TOARGBROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 ARGB (64 bytes).
void OMITFP I422ToARGBRow_AVX2(const uint8_t* y_buf,
                               const uint8_t* u_buf,
                               const uint8_t* v_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

    LABELALIGN
      "1:          \n"
    READYUV422_AVX2
    YUVTORGB_AVX2(yuvconstants)
    STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}
#endif  // HAS_I422TOARGBROW_AVX2

#if defined(HAS_I422TOARGBROW_AVX512BW)
static const uint64_t kSplitQuadWords[8] = {0, 2, 2, 2, 1, 2, 2, 2};
static const uint64_t kSplitDoubleQuadWords[8] = {0, 1, 4, 4, 2, 3, 4, 4};
static const uint64_t kUnpermuteAVX512[8] = {0, 4, 1, 5, 2, 6, 3, 7};

// 32 pixels
// 16 UV values upsampled to 32 UV, mixed with 32 Y producing 32 ARGB (128
// bytes).
void OMITFP I422ToARGBRow_AVX512BW(const uint8_t* y_buf,
                                   const uint8_t* u_buf,
                                   const uint8_t* v_buf,
                                   uint8_t* dst_argb,
                                   const struct YuvConstants* yuvconstants,
                                   int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX512BW(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%xmm5,%%xmm5,%%xmm5          \n"
      "vpbroadcastq %%xmm5,%%zmm5                \n"

    LABELALIGN
      "1:          \n"
    READYUV422_AVX512BW
    YUVTORGB_AVX512BW(yuvconstants)
    STOREARGB_AVX512BW
      "sub         $0x20,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),                         // %[y_buf]
    [u_buf]"+r"(u_buf),                         // %[u_buf]
    [v_buf]"+r"(v_buf),                         // %[v_buf]
    [dst_argb]"+r"(dst_argb),                   // %[dst_argb]
    [width]"+rm"(width)                         // %[width]
  : [yuvconstants]"r"(yuvconstants),            // %[yuvconstants]
    [quadsplitperm]"r"(kSplitQuadWords),        // %[quadsplitperm]
    [dquadsplitperm]"r"(kSplitDoubleQuadWords), // %[dquadsplitperm]
    [unperm]"r"(kUnpermuteAVX512)               // %[unperm]
  : "memory", "cc", YUVTORGB_REGS_AVX512BW
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}
#endif  // HAS_I422TOARGBROW_AVX512BW

#if defined(HAS_I422TOAR30ROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 AR30 (64 bytes).
void OMITFP I422ToAR30Row_AVX2(const uint8_t* y_buf,
                               const uint8_t* u_buf,
                               const uint8_t* v_buf,
                               uint8_t* dst_ar30,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"  // AR30 constants
      "vpsrlw      $14,%%ymm5,%%ymm5             \n"
      "vpsllw      $4,%%ymm5,%%ymm5              \n"  // 2 alpha bits
      "vpxor       %%ymm6,%%ymm6,%%ymm6          \n"  // 0 for min
      "vpcmpeqb    %%ymm7,%%ymm7,%%ymm7          \n"  // 1023 for max
      "vpsrlw      $6,%%ymm7,%%ymm7              \n"

    LABELALIGN
      "1:          \n"
    READYUV422_AVX2
    YUVTORGB16_AVX2(yuvconstants)
    STOREAR30_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_ar30]"+r"(dst_ar30),  // %[dst_ar30]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
  );
}
#endif  // HAS_I422TOAR30ROW_AVX2

#if defined(HAS_I210TOARGBROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 ARGB (64 bytes).
void OMITFP I210ToARGBRow_AVX2(const uint16_t* y_buf,
                               const uint16_t* u_buf,
                               const uint16_t* v_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

    LABELALIGN
      "1:          \n"
    READYUV210_AVX2
    YUVTORGB_AVX2(yuvconstants)
    STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}
#endif  // HAS_I210TOARGBROW_AVX2

#if defined(HAS_I212TOARGBROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 ARGB (64 bytes).
void OMITFP I212ToARGBRow_AVX2(const uint16_t* y_buf,
                               const uint16_t* u_buf,
                               const uint16_t* v_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

    LABELALIGN
      "1:          \n"
    READYUV212_AVX2
    YUVTORGB_AVX2(yuvconstants)
    STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}
#endif  // HAS_I212TOARGBROW_AVX2

#if defined(HAS_I210TOAR30ROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 AR30 (64 bytes).
void OMITFP I210ToAR30Row_AVX2(const uint16_t* y_buf,
                               const uint16_t* u_buf,
                               const uint16_t* v_buf,
                               uint8_t* dst_ar30,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"  // AR30 constants
      "vpsrlw      $14,%%ymm5,%%ymm5             \n"
      "vpsllw      $4,%%ymm5,%%ymm5              \n"  // 2 alpha bits
      "vpxor       %%ymm6,%%ymm6,%%ymm6          \n"  // 0 for min
      "vpcmpeqb    %%ymm7,%%ymm7,%%ymm7          \n"  // 1023 for max
      "vpsrlw      $6,%%ymm7,%%ymm7              \n"

    LABELALIGN
      "1:          \n"
    READYUV210_AVX2
    YUVTORGB16_AVX2(yuvconstants)
    STOREAR30_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_ar30]"+r"(dst_ar30),  // %[dst_ar30]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
  );
}
#endif  // HAS_I210TOAR30ROW_AVX2

#if defined(HAS_I212TOAR30ROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 AR30 (64 bytes).
void OMITFP I212ToAR30Row_AVX2(const uint16_t* y_buf,
                               const uint16_t* u_buf,
                               const uint16_t* v_buf,
                               uint8_t* dst_ar30,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"  // AR30 constants
      "vpsrlw      $14,%%ymm5,%%ymm5             \n"
      "vpsllw      $4,%%ymm5,%%ymm5              \n"  // 2 alpha bits
      "vpxor       %%ymm6,%%ymm6,%%ymm6          \n"  // 0 for min
      "vpcmpeqb    %%ymm7,%%ymm7,%%ymm7          \n"  // 1023 for max
      "vpsrlw      $6,%%ymm7,%%ymm7              \n"

    LABELALIGN
      "1:          \n"
    READYUV212_AVX2
    YUVTORGB16_AVX2(yuvconstants)
    STOREAR30_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_ar30]"+r"(dst_ar30),  // %[dst_ar30]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
  );
}
#endif  // HAS_I212TOAR30ROW_AVX2

#if defined(HAS_I410TOARGBROW_AVX2)
// 16 pixels
// 16 UV values with 16 Y producing 16 ARGB (64 bytes).
void OMITFP I410ToARGBRow_AVX2(const uint16_t* y_buf,
                               const uint16_t* u_buf,
                               const uint16_t* v_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

    LABELALIGN
      "1:          \n"
    READYUV410_AVX2
    YUVTORGB_AVX2(yuvconstants)
    STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"

  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
      "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}
#endif  // HAS_I410TOARGBROW_AVX2

#if defined(HAS_I210ALPHATOARGBROW_AVX2)
// 16 pixels
// 8 UV, 16 Y and 16 A producing 16 ARGB (64 bytes).
void OMITFP I210AlphaToARGBRow_AVX2(const uint16_t* y_buf,
                                    const uint16_t* u_buf,
                                    const uint16_t* v_buf,
                                    const uint16_t* a_buf,
                                    uint8_t* dst_argb,
                                    const struct YuvConstants* yuvconstants,
                                    int width) {
  asm volatile(YUVTORGB_SETUP_AVX2(
      yuvconstants) "sub         %[u_buf],%[v_buf]             \n"

      LABELALIGN "1:          \n" READYUVA210_AVX2 YUVTORGB_AVX2(
                   yuvconstants) STOREARGB_AVX2
      "subl        $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"

               : [y_buf] "+r"(y_buf),        // %[y_buf]
                 [u_buf] "+r"(u_buf),        // %[u_buf]
                 [v_buf] "+r"(v_buf),        // %[v_buf]
                 [a_buf] "+r"(a_buf),        // %[a_buf]
                 [dst_argb] "+r"(dst_argb),  // %[dst_argb]
#if defined(__i386__)
                 [width] "+m"(width)  // %[width]
#else
                 [width] "+rm"(width)  // %[width]
#endif
               : [yuvconstants] "r"(yuvconstants)  // %[yuvconstants]
               : "memory", "cc", YUVTORGB_REGS_AVX2 "xmm0", "xmm1", "xmm2",
                 "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_I210TOARGBROW_AVX2

#if defined(HAS_I410ALPHATOARGBROW_AVX2)
// 16 pixels
// 16 UV, 16 Y and 16 A producing 16 ARGB (64 bytes).
void OMITFP I410AlphaToARGBRow_AVX2(const uint16_t* y_buf,
                                    const uint16_t* u_buf,
                                    const uint16_t* v_buf,
                                    const uint16_t* a_buf,
                                    uint8_t* dst_argb,
                                    const struct YuvConstants* yuvconstants,
                                    int width) {
  asm volatile(YUVTORGB_SETUP_AVX2(
      yuvconstants) "sub         %[u_buf],%[v_buf]             \n"

      LABELALIGN "1:          \n" READYUVA410_AVX2 YUVTORGB_AVX2(
                   yuvconstants) STOREARGB_AVX2
      "subl        $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"

               : [y_buf] "+r"(y_buf),        // %[y_buf]
                 [u_buf] "+r"(u_buf),        // %[u_buf]
                 [v_buf] "+r"(v_buf),        // %[v_buf]
                 [a_buf] "+r"(a_buf),        // %[a_buf]
                 [dst_argb] "+r"(dst_argb),  // %[dst_argb]
#if defined(__i386__)
                 [width] "+m"(width)  // %[width]
#else
                 [width] "+rm"(width)  // %[width]
#endif
               : [yuvconstants] "r"(yuvconstants)  // %[yuvconstants]
               : "memory", "cc", YUVTORGB_REGS_AVX2 "xmm0", "xmm1", "xmm2",
                 "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_I410TOARGBROW_AVX2

#if defined(HAS_I410TOAR30ROW_AVX2)
// 16 pixels
// 16 UV values with 16 Y producing 16 AR30 (64 bytes).
void OMITFP I410ToAR30Row_AVX2(const uint16_t* y_buf,
                               const uint16_t* u_buf,
                               const uint16_t* v_buf,
                               uint8_t* dst_ar30,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"  // AR30 constants
      "vpsrlw      $14,%%ymm5,%%ymm5             \n"
      "vpsllw      $4,%%ymm5,%%ymm5              \n"  // 2 alpha bits
      "vpxor       %%ymm6,%%ymm6,%%ymm6          \n"  // 0 for min
      "vpcmpeqb    %%ymm7,%%ymm7,%%ymm7          \n"  // 1023 for max
      "vpsrlw      $6,%%ymm7,%%ymm7              \n"

    LABELALIGN
      "1:          \n"
    READYUV410_AVX2
    YUVTORGB16_AVX2(yuvconstants)
    STOREAR30_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_ar30]"+r"(dst_ar30),  // %[dst_ar30]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
      "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
  );
}
#endif  // HAS_I410TOAR30ROW_AVX2

#if defined(HAS_I444ALPHATOARGBROW_AVX2)
// 16 pixels
// 16 UV values with 16 Y and 16 A producing 16 ARGB.
void OMITFP I444AlphaToARGBRow_AVX2(const uint8_t* y_buf,
                                    const uint8_t* u_buf,
                                    const uint8_t* v_buf,
                                    const uint8_t* a_buf,
                                    uint8_t* dst_argb,
                                    const struct YuvConstants* yuvconstants,
                                    int width) {
  asm volatile(YUVTORGB_SETUP_AVX2(
      yuvconstants) "sub         %[u_buf],%[v_buf]             \n"

      LABELALIGN "1:          \n" READYUVA444_AVX2 YUVTORGB_AVX2(
                   yuvconstants) STOREARGB_AVX2
      "subl        $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : [y_buf] "+r"(y_buf),        // %[y_buf]
                 [u_buf] "+r"(u_buf),        // %[u_buf]
                 [v_buf] "+r"(v_buf),        // %[v_buf]
                 [a_buf] "+r"(a_buf),        // %[a_buf]
                 [dst_argb] "+r"(dst_argb),  // %[dst_argb]
#if defined(__i386__)
                 [width] "+m"(width)  // %[width]
#else
                 [width] "+rm"(width)  // %[width]
#endif
               : [yuvconstants] "r"(yuvconstants)  // %[yuvconstants]
               : "memory", "cc", YUVTORGB_REGS_AVX2 "xmm0", "xmm1", "xmm2",
                 "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_I444ALPHATOARGBROW_AVX2

#if defined(HAS_I422ALPHATOARGBROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y and 16 A producing 16 ARGB.
void OMITFP I422AlphaToARGBRow_AVX2(const uint8_t* y_buf,
                                    const uint8_t* u_buf,
                                    const uint8_t* v_buf,
                                    const uint8_t* a_buf,
                                    uint8_t* dst_argb,
                                    const struct YuvConstants* yuvconstants,
                                    int width) {
  asm volatile(YUVTORGB_SETUP_AVX2(
      yuvconstants) "sub         %[u_buf],%[v_buf]             \n"

      LABELALIGN "1:          \n" READYUVA422_AVX2 YUVTORGB_AVX2(
                   yuvconstants) STOREARGB_AVX2
      "subl        $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : [y_buf] "+r"(y_buf),        // %[y_buf]
                 [u_buf] "+r"(u_buf),        // %[u_buf]
                 [v_buf] "+r"(v_buf),        // %[v_buf]
                 [a_buf] "+r"(a_buf),        // %[a_buf]
                 [dst_argb] "+r"(dst_argb),  // %[dst_argb]
#if defined(__i386__)
                 [width] "+m"(width)  // %[width]
#else
                 [width] "+rm"(width)  // %[width]
#endif
               : [yuvconstants] "r"(yuvconstants)  // %[yuvconstants]
               : "memory", "cc", YUVTORGB_REGS_AVX2 "xmm0", "xmm1", "xmm2",
                 "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_I422ALPHATOARGBROW_AVX2

#if defined(HAS_I422TORGBAROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 RGBA (64 bytes).
void OMITFP I422ToRGBARow_AVX2(const uint8_t* y_buf,
                               const uint8_t* u_buf,
                               const uint8_t* v_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

    LABELALIGN
      "1:          \n"
    READYUV422_AVX2
    YUVTORGB_AVX2(yuvconstants)

    // Step 3: Weave into RGBA
    "vpunpcklbw %%ymm2,%%ymm1,%%ymm1           \n"
    "vpermq     $0xd8,%%ymm1,%%ymm1            \n"
    "vpunpcklbw %%ymm0,%%ymm5,%%ymm2           \n"
    "vpermq     $0xd8,%%ymm2,%%ymm2            \n"
    "vpunpcklwd %%ymm1,%%ymm2,%%ymm0           \n"
    "vpunpckhwd %%ymm1,%%ymm2,%%ymm1           \n"
    "vmovdqu    %%ymm0,(%[dst_argb])           \n"
    "vmovdqu    %%ymm1,0x20(%[dst_argb])       \n"
    "lea        0x40(%[dst_argb]),%[dst_argb]  \n"
    "sub        $0x10,%[width]                 \n"
    "jg         1b                             \n"
    "vzeroupper                                \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}
#endif  // HAS_I422TORGBAROW_AVX2

#if defined(HAS_NV12TOARGBROW_AVX2)
// 16 pixels.
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 ARGB (64 bytes).
void OMITFP NV12ToARGBRow_AVX2(const uint8_t* y_buf,
                               const uint8_t* uv_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile(YUVTORGB_SETUP_AVX2(
      yuvconstants) "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

      LABELALIGN "1:          \n" READNV12_AVX2 YUVTORGB_AVX2(
                   yuvconstants) STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : [y_buf] "+r"(y_buf),              // %[y_buf]
                 [uv_buf] "+r"(uv_buf),            // %[uv_buf]
                 [dst_argb] "+r"(dst_argb),        // %[dst_argb]
                 [width] "+rm"(width)              // %[width]
               : [yuvconstants] "r"(yuvconstants)  // %[yuvconstants]
               : "memory", "cc", YUVTORGB_REGS_AVX2 "xmm0", "xmm0", "xmm1",
                 "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_NV12TOARGBROW_AVX2

#if defined(HAS_NV21TOARGBROW_AVX2)
// 16 pixels.
// 8 VU values upsampled to 16 UV, mixed with 16 Y producing 16 ARGB (64 bytes).
void OMITFP NV21ToARGBRow_AVX2(const uint8_t* y_buf,
                               const uint8_t* vu_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile(YUVTORGB_SETUP_AVX2(
      yuvconstants) "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

      LABELALIGN "1:          \n" READNV21_AVX2 YUVTORGB_AVX2(
                   yuvconstants) STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : [y_buf] "+r"(y_buf),               // %[y_buf]
                 [vu_buf] "+r"(vu_buf),             // %[vu_buf]
                 [dst_argb] "+r"(dst_argb),         // %[dst_argb]
                 [width] "+rm"(width)               // %[width]
               : [yuvconstants] "r"(yuvconstants),  // %[yuvconstants]
                 [kShuffleNV21] "m"(kShuffleNV21)
               : "memory", "cc", YUVTORGB_REGS_AVX2 "xmm0", "xmm0", "xmm1",
                 "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_NV21TOARGBROW_AVX2

#if defined(HAS_YUY2TOARGBROW_AVX2)
// 16 pixels.
// 8 YUY2 values with 16 Y and 8 UV producing 16 ARGB (64 bytes).
void OMITFP YUY2ToARGBRow_AVX2(const uint8_t* yuy2_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile(
      "vbroadcastf128 %[kShuffleYUY2Y],%%ymm6    \n"
      "vbroadcastf128 %[kShuffleYUY2UV],%%ymm7   \n" YUVTORGB_SETUP_AVX2(
      yuvconstants) "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

      LABELALIGN "1:          \n" READYUY2_AVX2 YUVTORGB_AVX2(yuvconstants)
          STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : [yuy2_buf] "+r"(yuy2_buf),         // %[yuy2_buf]
        [dst_argb] "+r"(dst_argb),         // %[dst_argb]
        [width] "+rm"(width)               // %[width]
      : [yuvconstants] "r"(yuvconstants),  // %[yuvconstants]
        [kShuffleYUY2Y] "m"(kShuffleYUY2Y), [kShuffleYUY2UV] "m"(kShuffleYUY2UV)
      : "memory", "cc", YUVTORGB_REGS_AVX2 "xmm0", "xmm0", "xmm1", "xmm2",
        "xmm3", "xmm4", "xmm5", "xmm6", "xmm7");
}
#endif  // HAS_YUY2TOARGBROW_AVX2

#if defined(HAS_UYVYTOARGBROW_AVX2)
// 16 pixels.
// 8 UYVY values with 16 Y and 8 UV producing 16 ARGB (64 bytes).
void OMITFP UYVYToARGBRow_AVX2(const uint8_t* uyvy_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile(
      "vbroadcastf128 %[kShuffleUYVYY],%%ymm6    \n"
      "vbroadcastf128 %[kShuffleUYVYUV],%%ymm7   \n" YUVTORGB_SETUP_AVX2(
      yuvconstants) "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

      LABELALIGN "1:          \n" READUYVY_AVX2 YUVTORGB_AVX2(yuvconstants)
          STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : [uyvy_buf] "+r"(uyvy_buf),         // %[uyvy_buf]
        [dst_argb] "+r"(dst_argb),         // %[dst_argb]
        [width] "+rm"(width)               // %[width]
      : [yuvconstants] "r"(yuvconstants),  // %[yuvconstants]
        [kShuffleUYVYY] "m"(kShuffleUYVYY), [kShuffleUYVYUV] "m"(kShuffleUYVYUV)
      : "memory", "cc", YUVTORGB_REGS_AVX2 "xmm0", "xmm1", "xmm2", "xmm3",
        "xmm4", "xmm5", "xmm6", "xmm7");
}
#endif  // HAS_UYVYTOARGBROW_AVX2

#if defined(HAS_P210TOARGBROW_AVX2)
// 16 pixels.
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 ARGB (64 bytes).
void OMITFP P210ToARGBRow_AVX2(const uint16_t* y_buf,
                               const uint16_t* uv_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile(YUVTORGB_SETUP_AVX2(
      yuvconstants) "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

      LABELALIGN "1:          \n" READP210_AVX2 YUVTORGB_AVX2(
                   yuvconstants) STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : [y_buf] "+r"(y_buf),              // %[y_buf]
                 [uv_buf] "+r"(uv_buf),            // %[uv_buf]
                 [dst_argb] "+r"(dst_argb),        // %[dst_argb]
                 [width] "+rm"(width)              // %[width]
               : [yuvconstants] "r"(yuvconstants)  // %[yuvconstants]
               : "memory", "cc", YUVTORGB_REGS_AVX2 "xmm0", "xmm0", "xmm1",
                 "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_P210TOARGBROW_AVX2

#if defined(HAS_P410TOARGBROW_AVX2)
// 16 pixels.
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 ARGB (64 bytes).
void OMITFP P410ToARGBRow_AVX2(const uint16_t* y_buf,
                               const uint16_t* uv_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile(YUVTORGB_SETUP_AVX2(
      yuvconstants) "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

      LABELALIGN "1:          \n" READP410_AVX2 YUVTORGB_AVX2(
                   yuvconstants) STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : [y_buf] "+r"(y_buf),              // %[y_buf]
                 [uv_buf] "+r"(uv_buf),            // %[uv_buf]
                 [dst_argb] "+r"(dst_argb),        // %[dst_argb]
                 [width] "+rm"(width)              // %[width]
               : [yuvconstants] "r"(yuvconstants)  // %[yuvconstants]
               : "memory", "cc", YUVTORGB_REGS_AVX2 "xmm0", "xmm0", "xmm1",
                 "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_P410TOARGBROW_AVX2

#if defined(HAS_P210TOAR30ROW_AVX2)
// 16 pixels
// 16 UV values with 16 Y producing 16 AR30 (64 bytes).
void OMITFP P210ToAR30Row_AVX2(const uint16_t* y_buf,
                               const uint16_t* uv_buf,
                               uint8_t* dst_ar30,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"  // AR30 constants
      "vpsrlw      $14,%%ymm5,%%ymm5             \n"
      "vpsllw      $4,%%ymm5,%%ymm5              \n"  // 2 alpha bits
      "vpxor       %%ymm6,%%ymm6,%%ymm6          \n"  // 0 for min
      "vpcmpeqb    %%ymm7,%%ymm7,%%ymm7          \n"  // 1023 for max
      "vpsrlw      $6,%%ymm7,%%ymm7              \n"

    LABELALIGN
      "1:          \n"
    READP210_AVX2
    YUVTORGB16_AVX2(yuvconstants)
    STOREAR30_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [uv_buf]"+r"(uv_buf),    // %[uv_buf]
    [dst_ar30]"+r"(dst_ar30),  // %[dst_ar30]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
      "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
  );
}
#endif  // HAS_P210TOAR30ROW_AVX2

#if defined(HAS_P410TOAR30ROW_AVX2)
// 16 pixels
// 16 UV values with 16 Y producing 16 AR30 (64 bytes).
void OMITFP P410ToAR30Row_AVX2(const uint16_t* y_buf,
                               const uint16_t* uv_buf,
                               uint8_t* dst_ar30,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"  // AR30 constants
      "vpsrlw      $14,%%ymm5,%%ymm5             \n"
      "vpsllw      $4,%%ymm5,%%ymm5              \n"  // 2 alpha bits
      "vpxor       %%ymm6,%%ymm6,%%ymm6          \n"  // 0 for min
      "vpcmpeqb    %%ymm7,%%ymm7,%%ymm7          \n"  // 1023 for max
      "vpsrlw      $6,%%ymm7,%%ymm7              \n"

    LABELALIGN
      "1:          \n"
    READP410_AVX2
    YUVTORGB16_AVX2(yuvconstants)
    STOREAR30_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [uv_buf]"+r"(uv_buf),    // %[uv_buf]
    [dst_ar30]"+r"(dst_ar30),  // %[dst_ar30]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
      "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
  );
}
#endif  // HAS_P410TOAR30ROW_AVX2


#ifdef HAS_I400TOARGBROW_AVX2
// 16 pixels of Y converted to 16 pixels of ARGB (64 bytes).
// note: vpunpcklbw mutates and vpackuswb unmutates.
void I400ToARGBRow_AVX2(const uint8_t* y_buf,
                        uint8_t* dst_argb,
                        const struct YuvConstants* yuvconstants,
                        int width) {
  asm volatile(
      "vmovdqa     96(%3),%%ymm2                 \n"  // yg = 18997 = 1.164
      "vmovdqa     128(%3),%%ymm3                \n"  // ygb = -1160 = 1.164*16
      "vpcmpeqb    %%ymm4,%%ymm4,%%ymm4          \n"  // 0xff000000
      "vpslld      $0x18,%%ymm4,%%ymm4           \n"

      LABELALIGN
      "1:          \n"
      // Step 1: Scale Y contribution to 16 G values. G = (y - 16) * 1.164
      "vmovdqu    (%0),%%xmm0                    \n"
      "lea        0x10(%0),%0                    \n"
      "vpermq     $0xd8,%%ymm0,%%ymm0            \n"
      "vpunpcklbw %%ymm0,%%ymm0,%%ymm0           \n"
      "vpmulhuw   %%ymm2,%%ymm0,%%ymm0           \n"
      "vpaddsw    %%ymm3,%%ymm0,%%ymm0           \n"
      "vpsraw     $0x6,%%ymm0,%%ymm0             \n"
      "vpackuswb  %%ymm0,%%ymm0,%%ymm0           \n"
      "vpunpcklbw %%ymm0,%%ymm0,%%ymm1           \n"
      "vpermq     $0xd8,%%ymm1,%%ymm1            \n"
      "vpunpcklwd %%ymm1,%%ymm1,%%ymm0           \n"
      "vpunpckhwd %%ymm1,%%ymm1,%%ymm1           \n"
      "vpor       %%ymm4,%%ymm0,%%ymm0           \n"
      "vpor       %%ymm4,%%ymm1,%%ymm1           \n"
      "vmovdqu    %%ymm0,(%1)                    \n"
      "vmovdqu    %%ymm1,0x20(%1)                \n"
      "lea        0x40(%1),%1                     \n"
      "sub        $0x10,%2                       \n"
      "jg        1b                              \n"
      "vzeroupper                                \n"
      : "+r"(y_buf),       // %0
        "+r"(dst_argb),    // %1
        "+rm"(width)       // %2
      : "r"(yuvconstants)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}
#endif  // HAS_I400TOARGBROW_AVX2

#if !defined(LIBYUV_DISABLE_X86)
// Shuffle table for reversing the bytes.
static const uvec8 kShuffleMirror = {15u, 14u, 13u, 12u, 11u, 10u, 9u, 8u,
                                     7u,  6u,  5u,  4u,  3u,  2u,  1u, 0u};

#endif

#ifdef HAS_MIRRORROW_AVX2
void MirrorRow_AVX2(const uint8_t* src, uint8_t* dst, int width) {
  intptr_t temp_width = (intptr_t)(width);
      asm volatile("vbroadcastf128 %3,%%ymm5                  \n"

               LABELALIGN
      "1:          \n"
      "vmovdqu     -0x20(%0,%2,1),%%ymm0         \n"
      "vpshufb     %%ymm5,%%ymm0,%%ymm0          \n"
      "vpermq      $0x4e,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src),           // %0
                 "+r"(dst),           // %1
                 "+r"(temp_width)     // %2
               : "m"(kShuffleMirror)  // %3
               : "memory", "cc", "xmm0", "xmm5");
}
#endif  // HAS_MIRRORROW_AVX2

#if !defined(LIBYUV_DISABLE_X86)
// Shuffle table for reversing the UV.
static const uvec8 kShuffleMirrorUV = {14u, 15u, 12u, 13u, 10u, 11u, 8u, 9u,
                                       6u,  7u,  4u,  5u,  2u,  3u,  0u, 1u};

#endif

#ifdef HAS_MIRRORUVROW_AVX2
void MirrorUVRow_AVX2(const uint8_t* src_uv, uint8_t* dst_uv, int width) {
  intptr_t temp_width = (intptr_t)(width);
      asm volatile("vbroadcastf128 %3,%%ymm5                  \n"

               LABELALIGN
      "1:          \n"
      "vmovdqu     -0x20(%0,%2,2),%%ymm0         \n"
      "vpshufb     %%ymm5,%%ymm0,%%ymm0          \n"
      "vpermq      $0x4e,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_uv),          // %0
                 "+r"(dst_uv),          // %1
                 "+r"(temp_width)       // %2
               : "m"(kShuffleMirrorUV)  // %3
               : "memory", "cc", "xmm0", "xmm5");
}
#endif  // HAS_MIRRORUVROW_AVX2

#if !defined(LIBYUV_DISABLE_X86)
// Shuffle table for reversing the bytes of UV channels.
static const uvec8 kShuffleMirrorSplitUV = {14u, 12u, 10u, 8u, 6u, 4u, 2u, 0u,
                                            15u, 13u, 11u, 9u, 7u, 5u, 3u, 1u};
#endif

#if !defined(LIBYUV_DISABLE_X86)

// Shuffle first 5 pixels to last 5 mirrored.  first byte zero
static const uvec8 kShuffleMirrorRGB0 = {128u, 12u, 13u, 14u, 9u, 10u, 11u, 6u,
                                         7u,   8u,  3u,  4u,  5u, 0u,  1u,  2u};

// Shuffle last 5 pixels to first 5 mirrored.  last byte zero
static const uvec8 kShuffleMirrorRGB1 = {
    13u, 14u, 15u, 10u, 11u, 12u, 7u, 8u, 9u, 4u, 5u, 6u, 1u, 2u, 3u, 128u};

// Shuffle 5 pixels at a time (15 bytes)
#endif


#ifdef HAS_ARGBMIRRORROW_AVX2
// Shuffle table for reversing the bytes.
static const ulvec32 kARGBShuffleMirror_AVX2 = {7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u};
void ARGBMirrorRow_AVX2(const uint8_t* src, uint8_t* dst, int width) {
  intptr_t temp_width = (intptr_t)(width);
      asm volatile("vmovdqu     %3,%%ymm5                     \n"

               LABELALIGN
      "1:          \n"
      "vpermd      -0x20(%0,%2,4),%%ymm5,%%ymm0  \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src),                    // %0
                 "+r"(dst),                    // %1
                 "+r"(temp_width)              // %2
               : "m"(kARGBShuffleMirror_AVX2)  // %3
               : "memory", "cc", "xmm0", "xmm5");
}
#endif  // HAS_ARGBMIRRORROW_AVX2

#ifdef HAS_SPLITUVROW_AVX2
void SplitUVRow_AVX2(const uint8_t* src_uv,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width) {
  asm volatile(
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpsrlw      $0x8,%%ymm5,%%ymm5            \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm2            \n"
      "vpsrlw      $0x8,%%ymm1,%%ymm3            \n"
      "vpand       %%ymm5,%%ymm0,%%ymm0          \n"
      "vpand       %%ymm5,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpackuswb   %%ymm3,%%ymm2,%%ymm2          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpermq      $0xd8,%%ymm2,%%ymm2           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm2,0x00(%1,%2,1)          \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_uv),  // %0
        "+r"(dst_u),   // %1
        "+r"(dst_v),   // %2
        "+r"(width)    // %3
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm5");
}
#endif  // HAS_SPLITUVROW_AVX2




#ifdef HAS_DETILEROW_16_AVX
void DetileRow_16_AVX(const uint16_t* src,
                      ptrdiff_t src_tile_stride,
                      uint16_t* dst,
                      int width) {
  asm volatile(
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "lea         (%0,%3,2),%0                  \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),            // %0
        "+r"(dst),            // %1
        "+r"(width)           // %2
      : "r"(src_tile_stride)  // %3
      : "cc", "memory", "xmm0");
}
#endif  // HAS_DETILEROW_AVX


#if !defined(LIBYUV_DISABLE_X86)
// TODO(greenjustin): Look into generating these constants instead of loading
// them since this can cause branch mispredicts for fPIC code on 32-bit
// machines.
static const uvec8 kDeinterlaceUV = {0, 2, 4, 6, 8, 10, 12, 14,
                                     1, 3, 5, 7, 9, 11, 13, 15};

// TODO(greenjustin): Research alternatives to pshufb, since pshufb can be very
#endif

#ifdef HAS_MERGEUVROW_AVX512BW
void MergeUVRow_AVX512BW(const uint8_t* src_u,
                         const uint8_t* src_v,
                         uint8_t* dst_uv,
                         int width) {
      asm volatile("sub         %0,%1                         \n"

               LABELALIGN
      "1:          \n"
      "vpmovzxbw   (%0),%%zmm0                   \n"
      "vpmovzxbw   0x00(%0,%1,1),%%zmm1          \n"
      "lea         0x20(%0),%0                   \n"
      "vpsllw      $0x8,%%zmm1,%%zmm1            \n"
      "vporq       %%zmm0,%%zmm1,%%zmm2          \n"
      "vmovdqu64   %%zmm2,(%2)                   \n"
      "lea         0x40(%2),%2                   \n"
      "sub         $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_u),   // %0
                 "+r"(src_v),   // %1
                 "+r"(dst_uv),  // %2
                 "+r"(width)    // %3
               :
               : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_MERGEUVROW_AVX512BW

#ifdef HAS_MERGEUVROW_AVX2
void MergeUVRow_AVX2(const uint8_t* src_u,
                     const uint8_t* src_v,
                     uint8_t* dst_uv,
                     int width) {
      asm volatile("sub         %0,%1                         \n"

               LABELALIGN
      "1:          \n"
      "vpmovzxbw   (%0),%%ymm0                   \n"
      "vpmovzxbw   0x00(%0,%1,1),%%ymm1          \n"
      "lea         0x10(%0),%0                   \n"
      "vpsllw      $0x8,%%ymm1,%%ymm1            \n"
      "vpor        %%ymm0,%%ymm1,%%ymm2          \n"
      "vmovdqu     %%ymm2,(%2)                   \n"
      "lea         0x20(%2),%2                   \n"
      "sub         $0x10,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_u),   // %0
                 "+r"(src_v),   // %1
                 "+r"(dst_uv),  // %2
                 "+r"(width)    // %3
               :
               : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_MERGEUVROW_AVX2


#ifdef HAS_MERGEUVROW_16_AVX2
void MergeUVRow_16_AVX2(const uint16_t* src_u,
                        const uint16_t* src_v,
                        uint16_t* dst_uv,
                        int depth,
                        int width) {
  asm volatile(
      "vmovd       %4,%%xmm3                     \n"
      "vmovd       %5,%%xmm4                     \n"

      "sub         %0,%1                         \n"
      // 8 pixels per loop.

      LABELALIGN
      "1:          \n"
      "vpmovzxwd   (%0),%%ymm0                   \n"
      "vpmovzxwd   0x00(%0,%1,1),%%ymm1          \n"
      "lea         0x10(%0),%0                   \n"
      "vpsllw      %%xmm3,%%ymm0,%%ymm0          \n"
      "vpslld      %%xmm4,%%ymm1,%%ymm1          \n"
      "vpor        %%ymm0,%%ymm1,%%ymm2          \n"
      "vmovdqu     %%ymm2,(%2)                   \n"
      "lea         0x20(%2),%2                   \n"
      "sub         $0x8,%3                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_u),      // %0
        "+r"(src_v),      // %1
        "+r"(dst_uv),     // %2
        "+r"(width)       // %3
      : "r"(16 - depth),  // %4
        "r"(32 - depth)   // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}
#endif  // HAS_MERGEUVROW_AVX2

#ifdef HAS_SPLITUVROW_16_AVX2
const uvec8 kSplitUVShuffle16 = {0, 1, 4, 5, 8,  9,  12, 13,
                                 2, 3, 6, 7, 10, 11, 14, 15};
void SplitUVRow_16_AVX2(const uint16_t* src_uv,
                        uint16_t* dst_u,
                        uint16_t* dst_v,
                        int depth,
                        int width) {
  depth = 16 - depth;
  asm volatile(
      "vmovd       %4,%%xmm3                     \n"
      "vbroadcastf128 %5,%%ymm4                  \n"
      "sub         %1,%2                         \n"

      // 16 pixels per loop.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "add         $0x40,%0                      \n"

      "vpsrlw      %%xmm3,%%ymm0,%%ymm0          \n"
      "vpsrlw      %%xmm3,%%ymm1,%%ymm1          \n"
      "vpshufb     %%ymm4,%%ymm0,%%ymm0          \n"
      "vpshufb     %%ymm4,%%ymm1,%%ymm1          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpermq      $0xd8,%%ymm1,%%ymm1           \n"
      "vextractf128 $0x0,%%ymm0,(%1)             \n"
      "vextractf128 $0x0,%%ymm1,0x10(%1)         \n"
      "vextractf128 $0x1,%%ymm0,(%1,%2)          \n"
      "vextractf128 $0x1,%%ymm1,0x10(%1,%2)      \n"
      "add         $0x20,%1                      \n"
      "sub         $0x10,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_uv),           // %0
        "+r"(dst_u),            // %1
        "+r"(dst_v),            // %2
        "+r"(width)             // %3
      : "r"(depth),             // %4
        "m"(kSplitUVShuffle16)  // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}
#endif  // HAS_SPLITUVROW_16_AVX2

// Use scale to convert lsb formats to msb, depending how many bits there are:
// 128 = 9 bits
// 64 = 10 bits
// 16 = 12 bits
// 1 = 16 bits
#ifdef HAS_MULTIPLYROW_16_AVX2
void MultiplyRow_16_AVX2(const uint16_t* src_y,
                         uint16_t* dst_y,
                         int scale,
                         int width) {
  asm volatile(
      "vmovd       %3,%%xmm3                     \n"
      "vpbroadcastw %%xmm3,%%ymm3                \n"
      "sub         %0,%1                         \n"

      // 32 pixels per loop.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vpmullw     %%ymm3,%%ymm0,%%ymm0          \n"
      "vpmullw     %%ymm3,%%ymm1,%%ymm1          \n"
      "vmovdqu     %%ymm0,(%0,%1)                \n"
      "vmovdqu     %%ymm1,0x20(%0,%1)            \n"
      "add         $0x40,%0                      \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_y),  // %0
        "+r"(dst_y),  // %1
        "+r"(width)   // %2
      : "r"(scale)    // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm3");
}
#endif  // HAS_MULTIPLYROW_16_AVX2

// Use scale to convert msb formats to lsb, depending how many bits there are:
// 512 = 9 bits
// 1024 = 10 bits
// 4096 = 12 bits
// 65536 = 16 bits
#ifdef HAS_DIVIDEROW_16_AVX2
void DivideRow_16_AVX2(const uint16_t* src_y,
                       uint16_t* dst_y,
                       int scale,
                       int width) {
  asm volatile(
      "vmovd       %3,%%xmm3                     \n"
      "vpbroadcastw %%xmm3,%%ymm3                \n"
      "sub         %0,%1                         \n"

      // 32 pixels per loop.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vpmulhuw    %%ymm3,%%ymm0,%%ymm0          \n"
      "vpmulhuw    %%ymm3,%%ymm1,%%ymm1          \n"
      "vmovdqu     %%ymm0,(%0,%1)                \n"
      "vmovdqu     %%ymm1,0x20(%0,%1)            \n"
      "add         $0x40,%0                      \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_y),  // %0
        "+r"(dst_y),  // %1
        "+r"(width),  // %2
        "+r"(scale)   // %3
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm3");
}
#endif  // HAS_MULTIPLYROW_16_AVX2

// Use scale to convert lsb formats to msb, depending how many bits there are:
// 32768 = 9 bits
// 16384 = 10 bits
// 4096 = 12 bits
// 256 = 16 bits

#ifdef HAS_CONVERT16TO8ROW_AVX2
void Convert16To8Row_AVX2(const uint16_t* src_y,
                          uint8_t* dst_y,
                          int scale,
                          int width) {
  asm volatile(
      "vmovd       %3,%%xmm2                     \n"
      "vpbroadcastw %%xmm2,%%ymm2                \n"

      // 32 pixels per loop.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "add         $0x40,%0                      \n"
      "vpmulhuw    %%ymm2,%%ymm0,%%ymm0          \n"
      "vpmulhuw    %%ymm2,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"  // mutates
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "add         $0x20,%1                      \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_y),  // %0
        "+r"(dst_y),  // %1
        "+r"(width)   // %2
      : "r"(scale)    // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_CONVERT16TO8ROW_AVX2

#ifdef HAS_CONVERT16TO8ROW_AVX512BW
void Convert16To8Row_AVX512BW(const uint16_t* src_y,
                              uint8_t* dst_y,
                              int scale,
                              int width) {
      asm volatile("vpbroadcastw %3,%%zmm2                    \n"

               // 64 pixels per loop.
               LABELALIGN
      "1:          \n"
      "vmovups     (%0),%%zmm0                   \n"
      "vmovups     0x40(%0),%%zmm1               \n"
      "add         $0x80,%0                      \n"
      "vpmulhuw    %%zmm2,%%zmm0,%%zmm0          \n"
      "vpmulhuw    %%zmm2,%%zmm1,%%zmm1          \n"
      "vpmovuswb   %%zmm0,%%ymm0                 \n"
      "vpmovuswb   %%zmm1,%%ymm1                 \n"
      "vmovups     %%ymm0,(%1)                   \n"
      "vmovups     %%ymm1,0x20(%1)               \n"
      "add         $0x40,%1                      \n"
      "sub         $0x40,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_y),  // %0
                 "+r"(dst_y),  // %1
                 "+r"(width)   // %2
               : "r"(scale)    // %3
               : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_CONVERT16TO8ROW_AVX2

// Use scale to convert to lsb formats depending how many bits there are:
// 512 = 9 bits
// 1024 = 10 bits
// 4096 = 12 bits

#ifdef HAS_CONVERT8TO16ROW_AVX2
void Convert8To16Row_AVX2(const uint8_t* src_y,
                          uint16_t* dst_y,
                          int scale,
                          int width) {
  const int shift = __builtin_clz(scale) - 15;
      asm volatile("vmovd       %3,%%xmm2                     \n"

               // 32 pixels per loop.
               LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "add         $0x20,%0                      \n"
      "vpunpckhbw  %%ymm0,%%ymm0,%%ymm1          \n"
      "vpunpcklbw  %%ymm0,%%ymm0,%%ymm0          \n"
      "vpsrlw      %%xmm2,%%ymm0,%%ymm0          \n"
      "vpsrlw      %%xmm2,%%ymm1,%%ymm1          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "add         $0x40,%1                      \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_y),  // %0
                 "+r"(dst_y),  // %1
                 "+r"(width)   // %2
               : "r"(shift)    // %3
               : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_CONVERT8TO16ROW_AVX2

#if !defined(LIBYUV_DISABLE_X86)
// Shuffle table for converting RGB to Planar.
static const uvec8 kSplitRGBShuffle[9] = {
    {0u, 3u, 6u, 9u, 12u, 15u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u,
     128u, 128u},
    {128u, 128u, 128u, 128u, 128u, 128u, 2u, 5u, 8u, 11u, 14u, 128u, 128u, 128u,
     128u, 128u},
    {128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 1u, 4u,
     7u, 10u, 13u},
    {1u, 4u, 7u, 10u, 13u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u,
     128u, 128u},
    {128u, 128u, 128u, 128u, 128u, 0u, 3u, 6u, 9u, 12u, 15u, 128u, 128u, 128u,
     128u, 128u},
    {128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 2u, 5u,
     8u, 11u, 14u},
    {2u, 5u, 8u, 11u, 14u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u,
     128u, 128u},
    {128u, 128u, 128u, 128u, 128u, 1u, 4u, 7u, 10u, 13u, 128u, 128u, 128u, 128u,
     128u, 128u},
    {128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u, 0u, 3u, 6u, 9u,
     12u, 15u}};

#endif

#if !defined(LIBYUV_DISABLE_X86)
// Shuffle table for converting RGB to Planar, SSE4.1. Note: these are used for
// the AVX2 implementation as well.
static const uvec8 kSplitRGBShuffleAVX2[5] = {
    {0u, 3u, 6u, 9u, 12u, 15u, 2u, 5u, 8u, 11u, 14u, 1u, 4u, 7u, 10u, 13u},
    {1u, 4u, 7u, 10u, 13u, 0u, 3u, 6u, 9u, 12u, 15u, 2u, 5u, 8u, 11u, 14u},
    {2u, 5u, 8u, 11u, 14u, 1u, 4u, 7u, 10u, 13u, 0u, 3u, 6u, 9u, 12u, 15u},
    {0u, 128u, 0u, 0u, 128u, 0u, 0u, 128u, 0u, 0u, 128u, 0u, 0u, 128u, 0u, 0u},
    {0u, 0u, 128u, 0u, 0u, 128u, 0u, 0u, 128u, 0u, 0u, 128u, 0u, 0u, 128u, 0u},
};

#endif

#ifdef HAS_SPLITRGBROW_AVX2
void SplitRGBRow_AVX2(const uint8_t* src_rgb,
                      uint8_t* dst_r,
                      uint8_t* dst_g,
                      uint8_t* dst_b,
                      int width) {
  asm volatile(
      "vbroadcasti128 48(%5), %%ymm0             \n"
      "vbroadcasti128 64(%5), %%ymm7             \n"
#if defined(__x86_64__)
      "vbroadcasti128 0(%5), %%ymm8              \n"
      "vbroadcasti128 16(%5), %%ymm9             \n"
      "vbroadcasti128 32(%5), %%ymm10            \n"
#endif
      "1:          \n"
      "vmovdqu     (%0),%%ymm4                   \n"
      "vmovdqu     0x20(%0),%%ymm5               \n"
      "vmovdqu     0x40(%0),%%ymm6               \n"
      "lea         0x60(%0),%0                   \n"
      "vpblendd    $240, %%ymm5, %%ymm4, %%ymm1  \n"
      "vperm2i128  $33, %%ymm6, %%ymm4, %%ymm2   \n"
      "vpblendd    $240, %%ymm6, %%ymm5, %%ymm3  \n"
      "vpblendvb   %%ymm0, %%ymm3, %%ymm1, %%ymm4 \n"
      "vpblendvb   %%ymm0, %%ymm1, %%ymm2, %%ymm5 \n"
      "vpblendvb   %%ymm0, %%ymm2, %%ymm3, %%ymm6 \n"
      "vpblendvb   %%ymm7, %%ymm5, %%ymm4, %%ymm1 \n"
      "vpblendvb   %%ymm7, %%ymm6, %%ymm5, %%ymm2 \n"
      "vpblendvb   %%ymm7, %%ymm4, %%ymm6, %%ymm3 \n"
#if defined(__x86_64__)
      "vpshufb     %%ymm8, %%ymm1, %%ymm1        \n"
      "vpshufb     %%ymm9, %%ymm2, %%ymm2        \n"
      "vpshufb     %%ymm10, %%ymm3, %%ymm3       \n"
#else
      "vbroadcasti128 0(%5), %%ymm4              \n"
      "vbroadcasti128 16(%5), %%ymm5             \n"
      "vbroadcasti128 32(%5), %%ymm6             \n"
      "vpshufb     %%ymm4, %%ymm1, %%ymm1        \n"
      "vpshufb     %%ymm5, %%ymm2, %%ymm2        \n"
      "vpshufb     %%ymm6, %%ymm3, %%ymm3        \n"
#endif
      "vmovdqu     %%ymm1,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "vmovdqu     %%ymm2,(%2)                   \n"
      "lea         0x20(%2),%2                   \n"
      "vmovdqu     %%ymm3,(%3)                   \n"
      "lea         0x20(%3),%3                   \n"
      "sub         $0x20,%4                      \n"
      "jg          1b                            \n"
      : "+r"(src_rgb),                  // %0
        "+r"(dst_r),                    // %1
        "+r"(dst_g),                    // %2
        "+r"(dst_b),                    // %3
        "+r"(width)                     // %4
      : "r"(&kSplitRGBShuffleAVX2[0])  // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7"
#if defined(__x86_64__)
        ,
        "xmm8", "xmm9", "xmm10"
#endif
  );
}
#endif  // HAS_SPLITRGBROW_AVX2

#if !defined(LIBYUV_DISABLE_X86)
// Shuffle table for converting Planar to RGB.
static const uvec8 kMergeRGBShuffle[9] = {
    {0u, 128u, 128u, 1u, 128u, 128u, 2u, 128u, 128u, 3u, 128u, 128u, 4u, 128u,
     128u, 5u},
    {128u, 0u, 128u, 128u, 1u, 128u, 128u, 2u, 128u, 128u, 3u, 128u, 128u, 4u,
     128u, 128u},
    {128u, 128u, 0u, 128u, 128u, 1u, 128u, 128u, 2u, 128u, 128u, 3u, 128u, 128u,
     4u, 128u},
    {128u, 128u, 6u, 128u, 128u, 7u, 128u, 128u, 8u, 128u, 128u, 9u, 128u, 128u,
     10u, 128u},
    {5u, 128u, 128u, 6u, 128u, 128u, 7u, 128u, 128u, 8u, 128u, 128u, 9u, 128u,
     128u, 10u},
    {128u, 5u, 128u, 128u, 6u, 128u, 128u, 7u, 128u, 128u, 8u, 128u, 128u, 9u,
     128u, 128u},
    {128u, 11u, 128u, 128u, 12u, 128u, 128u, 13u, 128u, 128u, 14u, 128u, 128u,
     15u, 128u, 128u},
    {128u, 128u, 11u, 128u, 128u, 12u, 128u, 128u, 13u, 128u, 128u, 14u, 128u,
     128u, 15u, 128u},
    {10u, 128u, 128u, 11u, 128u, 128u, 12u, 128u, 128u, 13u, 128u, 128u, 14u,
     128u, 128u, 15u}};

#endif



#ifdef HAS_MERGEARGBROW_AVX2
void MergeARGBRow_AVX2(const uint8_t* src_r,
                       const uint8_t* src_g,
                       const uint8_t* src_b,
                       const uint8_t* src_a,
                       uint8_t* dst_argb,
                       int width) {
  asm volatile(
      "sub         %0,%1                         \n"
      "sub         %0,%2                         \n"
      "sub         %0,%3                         \n"

      LABELALIGN
      "1:          \n"

      "vmovdqu     (%0,%2),%%xmm0                \n"  // B
      "vmovdqu     (%0,%1),%%xmm1                \n"  // R
      "vinserti128 $1,(%0),%%ymm0,%%ymm0         \n"  // G
      "vinserti128 $1,(%0,%3),%%ymm1,%%ymm1      \n"  // A
      "vpunpckhbw  %%ymm1,%%ymm0,%%ymm2          \n"
      "vpunpcklbw  %%ymm1,%%ymm0,%%ymm0          \n"
      "vperm2i128  $0x31,%%ymm2,%%ymm0,%%ymm1    \n"
      "vperm2i128  $0x20,%%ymm2,%%ymm0,%%ymm0    \n"
      "vpunpckhwd  %%ymm1,%%ymm0,%%ymm2          \n"
      "vpunpcklwd  %%ymm1,%%ymm0,%%ymm0          \n"
      "vperm2i128  $0x31,%%ymm2,%%ymm0,%%ymm1    \n"
      "vperm2i128  $0x20,%%ymm2,%%ymm0,%%ymm0    \n"
      "vmovdqu     %%ymm0,(%4)                   \n"  // First 8
      "vmovdqu     %%ymm1,32(%4)                 \n"  // Next 8

      "lea         16(%0),%0                     \n"
      "lea         64(%4),%4                     \n"
      "sub         $0x10,%5                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_r),     // %0
        "+r"(src_g),     // %1
        "+r"(src_b),     // %2
        "+r"(src_a),     // %3
        "+r"(dst_argb),  // %4
        "+r"(width)      // %5
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif

#ifdef HAS_MERGEXRGBROW_AVX2
void MergeXRGBRow_AVX2(const uint8_t* src_r,
                       const uint8_t* src_g,
                       const uint8_t* src_b,
                       uint8_t* dst_argb,
                       int width) {
  asm volatile(
      "1:          \n"

      "vmovdqu     (%2),%%xmm0                   \n"  // B
      "vpcmpeqb    %%ymm1,%%ymm1,%%ymm1          \n"  // A(255)
      "vinserti128 $0,(%1),%%ymm1,%%ymm1         \n"  // R
      "vinserti128 $1,(%0),%%ymm0,%%ymm0         \n"  // G
      "vpunpckhbw  %%ymm1,%%ymm0,%%ymm2          \n"
      "vpunpcklbw  %%ymm1,%%ymm0,%%ymm0          \n"
      "vperm2i128  $0x31,%%ymm2,%%ymm0,%%ymm1    \n"
      "vperm2i128  $0x20,%%ymm2,%%ymm0,%%ymm0    \n"
      "vpunpckhwd  %%ymm1,%%ymm0,%%ymm2          \n"
      "vpunpcklwd  %%ymm1,%%ymm0,%%ymm0          \n"
      "vperm2i128  $0x31,%%ymm2,%%ymm0,%%ymm1    \n"
      "vperm2i128  $0x20,%%ymm2,%%ymm0,%%ymm0    \n"
      "vmovdqu     %%ymm0,(%3)                   \n"  // First 8
      "vmovdqu     %%ymm1,32(%3)                 \n"  // Next 8

      "lea         16(%0),%0                     \n"
      "lea         16(%1),%1                     \n"
      "lea         16(%2),%2                     \n"
      "lea         64(%3),%3                     \n"
      "sub         $0x10,%4                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_r),     // %0
        "+r"(src_g),     // %1
        "+r"(src_b),     // %2
        "+r"(dst_argb),  // %3
        "+rm"(width)     // %4
        ::"memory",
        "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_MERGEARGBROW_AVX2



static const uvec8 kShuffleMaskARGBSplit = {0, 4, 8,  12, 1, 5, 9,  13,
                                            2, 6, 10, 14, 3, 7, 11, 15};


#ifdef HAS_SPLITARGBROW_AVX2
static const ulvec32 kShuffleMaskARGBPermute = {0, 4, 1, 5, 2, 6, 3, 7};
void SplitARGBRow_AVX2(const uint8_t* src_argb,
                       uint8_t* dst_r,
                       uint8_t* dst_g,
                       uint8_t* dst_b,
                       uint8_t* dst_a,
                       int width) {
  asm volatile(
      "sub         %1,%2                         \n"
      "sub         %1,%3                         \n"
      "sub         %1,%4                         \n"
      "vmovdqa     %7,%%ymm3                     \n"
      "vbroadcastf128 %6,%%ymm4                  \n"

      LABELALIGN
      "1:          \n"

      "vmovdqu     (%0),%%xmm0                   \n"  // 00-0F
      "vmovdqu     16(%0),%%xmm1                 \n"  // 10-1F
      "vinserti128 $1,32(%0),%%ymm0,%%ymm0       \n"  // 00-0F 20-2F
      "vinserti128 $1,48(%0),%%ymm1,%%ymm1       \n"  // 10-1F 30-3F
      "vpshufb     %%ymm4,%%ymm0,%%ymm0          \n"
      "vpshufb     %%ymm4,%%ymm1,%%ymm1          \n"
      "vpermd      %%ymm0,%%ymm3,%%ymm0          \n"
      "vpermd      %%ymm1,%%ymm3,%%ymm1          \n"
      "vpunpckhdq  %%ymm1,%%ymm0,%%ymm2          \n"  // GA
      "vpunpckldq  %%ymm1,%%ymm0,%%ymm0          \n"  // BR
      "vmovdqu     %%xmm0,(%1,%3)                \n"  // B
      "vextracti128 $1,%%ymm0,(%1)               \n"  // R
      "vmovdqu     %%xmm2,(%1,%2)                \n"  // G
      "vextracti128 $1,%%ymm2,(%1,%4)            \n"  // A
      "lea         64(%0),%0                     \n"
      "lea         16(%1),%1                     \n"
      "subl        $0x10,%5                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),  // %0
        "+r"(dst_r),     // %1
        "+r"(dst_g),     // %2
        "+r"(dst_b),     // %3
        "+r"(dst_a),     // %4
#if defined(__i386__)
        "+m"(width)  // %5
#else
        "+rm"(width)  // %5
#endif
      : "m"(kShuffleMaskARGBSplit),   // %6
        "m"(kShuffleMaskARGBPermute)  // %7
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}
#endif

#ifdef HAS_SPLITXRGBROW_AVX2
void SplitXRGBRow_AVX2(const uint8_t* src_argb,
                       uint8_t* dst_r,
                       uint8_t* dst_g,
                       uint8_t* dst_b,
                       int width) {
  asm volatile(
      "vmovdqa     %6,%%ymm3                     \n"
      "vbroadcastf128 %5,%%ymm4                  \n"

      LABELALIGN
      "1:          \n"

      "vmovdqu     (%0),%%xmm0                   \n"  // 00-0F
      "vmovdqu     16(%0),%%xmm1                 \n"  // 10-1F
      "vinserti128 $1,32(%0),%%ymm0,%%ymm0       \n"  // 00-0F 20-2F
      "vinserti128 $1,48(%0),%%ymm1,%%ymm1       \n"  // 10-1F 30-3F
      "vpshufb     %%ymm4,%%ymm0,%%ymm0          \n"
      "vpshufb     %%ymm4,%%ymm1,%%ymm1          \n"
      "vpermd      %%ymm0,%%ymm3,%%ymm0          \n"
      "vpermd      %%ymm1,%%ymm3,%%ymm1          \n"
      "vpunpckhdq  %%ymm1,%%ymm0,%%ymm2          \n"  // GA
      "vpunpckldq  %%ymm1,%%ymm0,%%ymm0          \n"  // BR
      "vmovdqu     %%xmm0,(%3)                   \n"  // B
      "vextracti128 $1,%%ymm0,(%1)               \n"  // R
      "vmovdqu     %%xmm2,(%2)                   \n"  // G

      "lea         64(%0),%0                     \n"
      "lea         16(%1),%1                     \n"
      "lea         16(%2),%2                     \n"
      "lea         16(%3),%3                     \n"
      "sub         $0x10,%4                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),               // %0
        "+r"(dst_r),                  // %1
        "+r"(dst_g),                  // %2
        "+r"(dst_b),                  // %3
        "+r"(width)                   // %4
      : "m"(kShuffleMaskARGBSplit),   // %5
        "m"(kShuffleMaskARGBPermute)  // %6
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}
#endif

#ifdef HAS_MERGEXR30ROW_AVX2
void MergeXR30Row_AVX2(const uint16_t* src_r,
                       const uint16_t* src_g,
                       const uint16_t* src_b,
                       uint8_t* dst_ar30,
                       int depth,
                       int width) {
  int shift = depth - 10;
  asm volatile(
      "sub         %0,%1                         \n"
      "sub         %0,%2                         \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"  // AR30 constants
      "vpsrlw      $14,%%ymm5,%%ymm5             \n"
      "vpsllw      $4,%%ymm5,%%ymm5              \n"  // 2 alpha bits
      "vpcmpeqb    %%ymm6,%%ymm6,%%ymm6          \n"
      "vpsrlw      $6,%%ymm6,%%ymm6              \n"
      "vmovd       %5,%%xmm4                     \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     (%0,%1),%%ymm1                \n"
      "vmovdqu     (%0,%2),%%ymm2                \n"
      "vpsrlw      %%xmm4,%%ymm0,%%ymm0          \n"
      "vpsrlw      %%xmm4,%%ymm1,%%ymm1          \n"
      "vpsrlw      %%xmm4,%%ymm2,%%ymm2          \n"
      "vpminuw     %%ymm0,%%ymm6,%%ymm0          \n"
      "vpminuw     %%ymm1,%%ymm6,%%ymm1          \n"
      "vpminuw     %%ymm2,%%ymm6,%%ymm2          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpermq      $0xd8,%%ymm1,%%ymm1           \n"
      "vpermq      $0xd8,%%ymm2,%%ymm2           \n"
      "vpsllw      $0x4,%%ymm0,%%ymm0            \n"  // Shift R to target bit
      "vpunpckhwd  %%ymm0,%%ymm2,%%ymm3          \n"  // RB
      "vpunpcklwd  %%ymm0,%%ymm2,%%ymm0          \n"
      "vpunpckhwd  %%ymm5,%%ymm1,%%ymm2          \n"  // AG
      "vpunpcklwd  %%ymm5,%%ymm1,%%ymm1          \n"
      "vpslld      $0xa,%%ymm1,%%ymm1            \n"  // Shift AG to target bit
      "vpslld      $0xa,%%ymm2,%%ymm2            \n"
      "vpor        %%ymm1,%%ymm0,%%ymm0          \n"  // Combine
      "vpor        %%ymm2,%%ymm3,%%ymm3          \n"
      "vmovdqu     %%ymm0,(%3)                   \n"
      "vmovdqu     %%ymm3,0x20(%3)               \n"
      "lea         0x20(%0),%0                   \n"
      "lea         0x40(%3),%3                   \n"
      "sub         $0x10,%4                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_r),     // %0
        "+r"(src_g),     // %1
        "+r"(src_b),     // %2
        "+r"(dst_ar30),  // %3
        "+r"(width)      // %4
#if defined(__i386__)
      : "m"(shift)  // %5
#else
      : "rm"(shift)   // %5
#endif
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif

#ifdef HAS_MERGEAR64ROW_AVX2
static const lvec32 MergeAR64Permute = {0, 4, 2, 6, 1, 5, 3, 7};
void MergeAR64Row_AVX2(const uint16_t* src_r,
                       const uint16_t* src_g,
                       const uint16_t* src_b,
                       const uint16_t* src_a,
                       uint16_t* dst_ar64,
                       int depth,
                       int width) {
  int shift = 16 - depth;
  int mask = (1 << depth) - 1;
  mask = (mask << 16) + mask;
  asm volatile(
      "sub         %0,%1                         \n"
      "sub         %0,%2                         \n"
      "sub         %0,%3                         \n"
      "vmovdqa     %8,%%ymm5                     \n"
      "vmovd       %6,%%xmm6                     \n"
      "vbroadcastss %7,%%ymm7                    \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"  // R
      "vmovdqu     (%0,%1),%%ymm1                \n"  // G
      "vmovdqu     (%0,%2),%%ymm2                \n"  // B
      "vmovdqu     (%0,%3),%%ymm3                \n"  // A
      "vpminuw     %%ymm0,%%ymm7,%%ymm0          \n"
      "vpminuw     %%ymm1,%%ymm7,%%ymm1          \n"
      "vpminuw     %%ymm2,%%ymm7,%%ymm2          \n"
      "vpminuw     %%ymm3,%%ymm7,%%ymm3          \n"
      "vpsllw      %%xmm6,%%ymm0,%%ymm0          \n"
      "vpsllw      %%xmm6,%%ymm1,%%ymm1          \n"
      "vpsllw      %%xmm6,%%ymm2,%%ymm2          \n"
      "vpsllw      %%xmm6,%%ymm3,%%ymm3          \n"
      "vpermd      %%ymm0,%%ymm5,%%ymm0          \n"
      "vpermd      %%ymm1,%%ymm5,%%ymm1          \n"
      "vpermd      %%ymm2,%%ymm5,%%ymm2          \n"
      "vpermd      %%ymm3,%%ymm5,%%ymm3          \n"
      "vpunpcklwd  %%ymm1,%%ymm2,%%ymm4          \n"  // BG(low)
      "vpunpckhwd  %%ymm1,%%ymm2,%%ymm1          \n"  // BG(hi)
      "vpunpcklwd  %%ymm3,%%ymm0,%%ymm2          \n"  // RA(low)
      "vpunpckhwd  %%ymm3,%%ymm0,%%ymm0          \n"  // RA(hi)
      "vpunpckldq  %%ymm2,%%ymm4,%%ymm3          \n"  // BGRA(1)
      "vpunpckhdq  %%ymm2,%%ymm4,%%ymm4          \n"  // BGRA(3)
      "vpunpckldq  %%ymm0,%%ymm1,%%ymm2          \n"  // BGRA(2)
      "vpunpckhdq  %%ymm0,%%ymm1,%%ymm1          \n"  // BGRA(4)
      "vmovdqu     %%ymm3,(%4)                   \n"
      "vmovdqu     %%ymm2,0x20(%4)               \n"
      "vmovdqu     %%ymm4,0x40(%4)               \n"
      "vmovdqu     %%ymm1,0x60(%4)               \n"
      "lea         0x20(%0),%0                   \n"
      "lea         0x80(%4),%4                   \n"
      "subl        $0x10,%5                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_r),     // %0
        "+r"(src_g),     // %1
        "+r"(src_b),     // %2
        "+r"(src_a),     // %3
        "+r"(dst_ar64),  // %4
#if defined(__i386__)
        "+m"(width)  // %5
#else
        "+rm"(width)  // %5
#endif
      : "m"(shift),            // %6
        "m"(mask),             // %7
        "m"(MergeAR64Permute)  // %8
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif

#ifdef HAS_MERGEXR64ROW_AVX2
void MergeXR64Row_AVX2(const uint16_t* src_r,
                       const uint16_t* src_g,
                       const uint16_t* src_b,
                       uint16_t* dst_ar64,
                       int depth,
                       int width) {
  int shift = 16 - depth;
  int mask = (1 << depth) - 1;
  mask = (mask << 16) + mask;
  asm volatile(
      "sub         %0,%1                         \n"
      "sub         %0,%2                         \n"
      "vmovdqa     %7,%%ymm5                     \n"
      "vmovd       %5,%%xmm6                     \n"
      "vbroadcastss %6,%%ymm7                    \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"  // R
      "vmovdqu     (%0,%1),%%ymm1                \n"  // G
      "vmovdqu     (%0,%2),%%ymm2                \n"  // B
      "vpminuw     %%ymm0,%%ymm7,%%ymm0          \n"
      "vpminuw     %%ymm1,%%ymm7,%%ymm1          \n"
      "vpminuw     %%ymm2,%%ymm7,%%ymm2          \n"
      "vpsllw      %%xmm6,%%ymm0,%%ymm0          \n"
      "vpsllw      %%xmm6,%%ymm1,%%ymm1          \n"
      "vpsllw      %%xmm6,%%ymm2,%%ymm2          \n"
      "vpermd      %%ymm0,%%ymm5,%%ymm0          \n"
      "vpermd      %%ymm1,%%ymm5,%%ymm1          \n"
      "vpermd      %%ymm2,%%ymm5,%%ymm2          \n"
      "vpcmpeqb    %%ymm3,%%ymm3,%%ymm3          \n"  // A (0xffff)
      "vpunpcklwd  %%ymm1,%%ymm2,%%ymm4          \n"  // BG(low)
      "vpunpckhwd  %%ymm1,%%ymm2,%%ymm1          \n"  // BG(hi)
      "vpunpcklwd  %%ymm3,%%ymm0,%%ymm2          \n"  // RA(low)
      "vpunpckhwd  %%ymm3,%%ymm0,%%ymm0          \n"  // RA(hi)
      "vpunpckldq  %%ymm2,%%ymm4,%%ymm3          \n"  // BGRA(1)
      "vpunpckhdq  %%ymm2,%%ymm4,%%ymm4          \n"  // BGRA(3)
      "vpunpckldq  %%ymm0,%%ymm1,%%ymm2          \n"  // BGRA(2)
      "vpunpckhdq  %%ymm0,%%ymm1,%%ymm1          \n"  // BGRA(4)
      "vmovdqu     %%ymm3,(%3)                   \n"
      "vmovdqu     %%ymm2,0x20(%3)               \n"
      "vmovdqu     %%ymm4,0x40(%3)               \n"
      "vmovdqu     %%ymm1,0x60(%3)               \n"
      "lea         0x20(%0),%0                   \n"
      "lea         0x80(%3),%3                   \n"
      "subl        $0x10,%4                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_r),           // %0
        "+r"(src_g),           // %1
        "+r"(src_b),           // %2
        "+r"(dst_ar64),        // %3
        "+r"(width)            // %4
      : "m"(shift),            // %5
        "m"(mask),             // %6
        "m"(MergeAR64Permute)  // %7
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif

#ifdef HAS_MERGEARGB16TO8ROW_AVX2
static const uvec8 MergeARGB16To8Shuffle = {0, 8,  1, 9,  2, 10, 3, 11,
                                            4, 12, 5, 13, 6, 14, 7, 15};
void MergeARGB16To8Row_AVX2(const uint16_t* src_r,
                            const uint16_t* src_g,
                            const uint16_t* src_b,
                            const uint16_t* src_a,
                            uint8_t* dst_argb,
                            int depth,
                            int width) {
  int shift = depth - 8;
  asm volatile(
      "sub         %0,%1                         \n"
      "sub         %0,%2                         \n"
      "sub         %0,%3                         \n"
      "vbroadcastf128 %7,%%ymm5                  \n"
      "vmovd       %6,%%xmm6                     \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"  // R
      "vmovdqu     (%0,%1),%%ymm1                \n"  // G
      "vmovdqu     (%0,%2),%%ymm2                \n"  // B
      "vmovdqu     (%0,%3),%%ymm3                \n"  // A
      "vpsrlw      %%xmm6,%%ymm0,%%ymm0          \n"
      "vpsrlw      %%xmm6,%%ymm1,%%ymm1          \n"
      "vpsrlw      %%xmm6,%%ymm2,%%ymm2          \n"
      "vpsrlw      %%xmm6,%%ymm3,%%ymm3          \n"
      "vpackuswb   %%ymm1,%%ymm2,%%ymm1          \n"  // BG (planar)
      "vpackuswb   %%ymm3,%%ymm0,%%ymm0          \n"  // RA (planar)
      "vpshufb     %%ymm5,%%ymm1,%%ymm1          \n"  // BG (interleave)
      "vpshufb     %%ymm5,%%ymm0,%%ymm0          \n"  // RA (interleave)
      "vpermq      $0xd8,%%ymm1,%%ymm1           \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpunpcklwd  %%ymm0,%%ymm1,%%ymm2          \n"  // BGRA (low)
      "vpunpckhwd  %%ymm0,%%ymm1,%%ymm0          \n"  // BGRA (hi)
      "vmovdqu     %%ymm2,(%4)                   \n"
      "vmovdqu     %%ymm0,0x20(%4)               \n"
      "lea         0x20(%0),%0                   \n"
      "lea         0x40(%4),%4                   \n"
      "subl        $0x10,%5                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_r),     // %0
        "+r"(src_g),     // %1
        "+r"(src_b),     // %2
        "+r"(src_a),     // %3
        "+r"(dst_argb),  // %4
#if defined(__i386__)
        "+m"(width)  // %5
#else
        "+rm"(width)  // %5
#endif
      : "m"(shift),                 // %6
        "m"(MergeARGB16To8Shuffle)  // %7
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}
#endif

#ifdef HAS_MERGEXRGB16TO8ROW_AVX2
void MergeXRGB16To8Row_AVX2(const uint16_t* src_r,
                            const uint16_t* src_g,
                            const uint16_t* src_b,
                            uint8_t* dst_argb,
                            int depth,
                            int width) {
  int shift = depth - 8;
  asm volatile(
      "sub         %0,%1                         \n"
      "sub         %0,%2                         \n"
      "vbroadcastf128 %6,%%ymm5                  \n"
      "vmovd       %5,%%xmm6                     \n"
      "vpcmpeqb    %%ymm3,%%ymm3,%%ymm3          \n"
      "vpsrlw      $8,%%ymm3,%%ymm3              \n"  // A (0xff)

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"  // R
      "vmovdqu     (%0,%1),%%ymm1                \n"  // G
      "vmovdqu     (%0,%2),%%ymm2                \n"  // B
      "vpsrlw      %%xmm6,%%ymm0,%%ymm0          \n"
      "vpsrlw      %%xmm6,%%ymm1,%%ymm1          \n"
      "vpsrlw      %%xmm6,%%ymm2,%%ymm2          \n"
      "vpackuswb   %%ymm1,%%ymm2,%%ymm1          \n"  // BG (planar)
      "vpackuswb   %%ymm3,%%ymm0,%%ymm0          \n"  // RA (planar)
      "vpshufb     %%ymm5,%%ymm1,%%ymm1          \n"  // BG (interleave)
      "vpshufb     %%ymm5,%%ymm0,%%ymm0          \n"  // RA (interleave)
      "vpermq      $0xd8,%%ymm1,%%ymm1           \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpunpcklwd  %%ymm0,%%ymm1,%%ymm2          \n"  // BGRA (low)
      "vpunpckhwd  %%ymm0,%%ymm1,%%ymm0          \n"  // BGRA (hi)
      "vmovdqu     %%ymm2,(%3)                   \n"
      "vmovdqu     %%ymm0,0x20(%3)               \n"
      "lea         0x20(%0),%0                   \n"
      "lea         0x40(%3),%3                   \n"
      "subl        $0x10,%4                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_r),                // %0
        "+r"(src_g),                // %1
        "+r"(src_b),                // %2
        "+r"(dst_argb),             // %3
        "+r"(width)                 // %4
      : "m"(shift),                 // %5
        "m"(MergeARGB16To8Shuffle)  // %6
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}
#endif


#ifdef HAS_COPYROW_AVX
void CopyRow_AVX(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x40,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),   // %0
        "+r"(dst),   // %1
        "+r"(width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1");
}
#endif  // HAS_COPYROW_AVX

#ifdef HAS_COPYROW_AVX512BW
void CopyRow_AVX512BW(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "1:          \n"
      "vmovups     (%0),%%zmm0                   \n"
      "vmovups     0x40(%0),%%zmm1               \n"
      "lea         0x80(%0),%0                   \n"
      "vmovups     %%zmm0,(%1)                   \n"
      "vmovups     %%zmm1,0x40(%1)               \n"
      "lea         0x80(%1),%1                   \n"
      "sub         $0x80,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),   // %0
        "+r"(dst),   // %1
        "+r"(width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1");
}
#endif  // HAS_COPYROW_AVX512

#ifdef HAS_COPYROW_ERMS
// Multiple of 1.
void CopyRow_ERMS(const uint8_t* src, uint8_t* dst, int width) {
  size_t width_tmp = (size_t)(width);
      asm volatile("rep         movsb                         \n"
               : "+S"(src),       // %0
                 "+D"(dst),       // %1
                 "+c"(width_tmp)  // %2
               :
               : "memory", "cc");
}
#endif  // HAS_COPYROW_ERMS


#ifdef HAS_ARGBCOPYALPHAROW_AVX2
// width in pixels
void ARGBCopyAlphaRow_AVX2(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "vpcmpeqb    %%ymm0,%%ymm0,%%ymm0          \n"
      "vpsrld      $0x8,%%ymm0,%%ymm0            \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm1                   \n"
      "vmovdqu     0x20(%0),%%ymm2               \n"
      "lea         0x40(%0),%0                   \n"
      "vpblendvb   %%ymm0,(%1),%%ymm1,%%ymm1     \n"
      "vpblendvb   %%ymm0,0x20(%1),%%ymm2,%%ymm2 \n"
      "vmovdqu     %%ymm1,(%1)                   \n"
      "vmovdqu     %%ymm2,0x20(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),   // %0
        "+r"(dst),   // %1
        "+r"(width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_ARGBCOPYALPHAROW_AVX2


#ifdef HAS_ARGBEXTRACTALPHAROW_AVX2
static const uvec8 kShuffleAlphaShort_AVX2 = {
    3u,  128u, 128u, 128u, 7u,  128u, 128u, 128u,
    11u, 128u, 128u, 128u, 15u, 128u, 128u, 128u};

void ARGBExtractAlphaRow_AVX2(const uint8_t* src_argb,
                              uint8_t* dst_a,
                              int width) {
  asm volatile(
      "vmovdqa     %3,%%ymm4                     \n"
      "vbroadcastf128 %4,%%ymm5                  \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0), %%ymm0                  \n"
      "vmovdqu     0x20(%0), %%ymm1              \n"
      "vpshufb     %%ymm5,%%ymm0,%%ymm0          \n"  // vpsrld $0x18, %%ymm0
      "vpshufb     %%ymm5,%%ymm1,%%ymm1          \n"
      "vmovdqu     0x40(%0), %%ymm2              \n"
      "vmovdqu     0x60(%0), %%ymm3              \n"
      "lea         0x80(%0), %0                  \n"
      "vpackssdw   %%ymm1, %%ymm0, %%ymm0        \n"  // mutates
      "vpshufb     %%ymm5,%%ymm2,%%ymm2          \n"
      "vpshufb     %%ymm5,%%ymm3,%%ymm3          \n"
      "vpackssdw   %%ymm3, %%ymm2, %%ymm2        \n"  // mutates
      "vpackuswb   %%ymm2,%%ymm0,%%ymm0          \n"  // mutates.
      "vpermd      %%ymm0,%%ymm4,%%ymm0          \n"  // unmutate.
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20, %2                     \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),               // %0
        "+r"(dst_a),                  // %1
        "+rm"(width)                  // %2
      : "m"(kPermdARGBToY_AVX),       // %3
        "m"(kShuffleAlphaShort_AVX2)  // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_ARGBEXTRACTALPHAROW_AVX2


#ifdef HAS_ARGBCOPYYTOALPHAROW_AVX2
// width in pixels
void ARGBCopyYToAlphaRow_AVX2(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "vpcmpeqb    %%ymm0,%%ymm0,%%ymm0          \n"
      "vpsrld      $0x8,%%ymm0,%%ymm0            \n"

      LABELALIGN
      "1:          \n"
      "vpmovzxbd   (%0),%%ymm1                   \n"
      "vpmovzxbd   0x8(%0),%%ymm2                \n"
      "lea         0x10(%0),%0                   \n"
      "vpslld      $0x18,%%ymm1,%%ymm1           \n"
      "vpslld      $0x18,%%ymm2,%%ymm2           \n"
      "vpblendvb   %%ymm0,(%1),%%ymm1,%%ymm1     \n"
      "vpblendvb   %%ymm0,0x20(%1),%%ymm2,%%ymm2 \n"
      "vmovdqu     %%ymm1,(%1)                   \n"
      "vmovdqu     %%ymm2,0x20(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),   // %0
        "+r"(dst),   // %1
        "+r"(width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_ARGBCOPYYTOALPHAROW_AVX2

#ifdef HAS_SETROW_X86
void SetRow_X86(uint8_t* dst, uint8_t v8, int width) {
  size_t width_tmp = (size_t)(width >> 2);
  const uint32_t v32 = v8 * 0x01010101u;  // Duplicate byte to all bytes.
      asm volatile("rep         stosl                         \n"
               : "+D"(dst),       // %0
                 "+c"(width_tmp)  // %1
               : "a"(v32)         // %2
               : "memory", "cc");
}

void SetRow_ERMS(uint8_t* dst, uint8_t v8, int width) {
  size_t width_tmp = (size_t)(width);
      asm volatile("rep         stosb                         \n"
               : "+D"(dst),       // %0
                 "+c"(width_tmp)  // %1
               : "a"(v8)          // %2
               : "memory", "cc");
}

void ARGBSetRow_X86(uint8_t* dst_argb, uint32_t v32, int width) {
  size_t width_tmp = (size_t)(width);
      asm volatile("rep         stosl                         \n"
               : "+D"(dst_argb),  // %0
                 "+c"(width_tmp)  // %1
               : "a"(v32)         // %2
               : "memory", "cc");
}
#endif  // HAS_SETROW_X86


#ifdef HAS_YUY2TOYROW_AVX2
void YUY2ToYRow_AVX2(const uint8_t* src_yuy2, uint8_t* dst_y, int width) {
  asm volatile(
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpsrlw      $0x8,%%ymm5,%%ymm5            \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpand       %%ymm5,%%ymm0,%%ymm0          \n"
      "vpand       %%ymm5,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_yuy2),  // %0
        "+r"(dst_y),     // %1
        "+r"(width)      // %2
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}

void YUY2ToNVUVRow_AVX2(const uint8_t* src_yuy2,
                        int stride_yuy2,
                        uint8_t* dst_uv,
                        int width) {
  asm volatile(
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vpavgb      0x00(%0,%3,1),%%ymm0,%%ymm0   \n"
      "vpavgb      0x20(%0,%3,1),%%ymm1,%%ymm1   \n"
      "lea         0x40(%0),%0                   \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x8,%%ymm1,%%ymm1            \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_yuy2),               // %0
        "+r"(dst_uv),                 // %1
        "+r"(width)                   // %2
      : "r"((intptr_t)(stride_yuy2))  // %3
      : "memory", "cc", "xmm0", "xmm1");
}

void YUY2ToUVRow_AVX2(const uint8_t* src_yuy2,
                      int stride_yuy2,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width) {
  asm volatile(
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpsrlw      $0x8,%%ymm5,%%ymm5            \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vpavgb      0x00(%0,%4,1),%%ymm0,%%ymm0   \n"
      "vpavgb      0x20(%0,%4,1),%%ymm1,%%ymm1   \n"
      "lea         0x40(%0),%0                   \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x8,%%ymm1,%%ymm1            \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpand       %%ymm5,%%ymm0,%%ymm1          \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm1,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm0,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm1,%%ymm1           \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vextractf128 $0x0,%%ymm1,(%1)             \n"
      "vextractf128 $0x0,%%ymm0,0x00(%1,%2,1)    \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_yuy2),               // %0
        "+r"(dst_u),                  // %1
        "+r"(dst_v),                  // %2
        "+r"(width)                   // %3
      : "r"((intptr_t)(stride_yuy2))  // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}

void YUY2ToUV422Row_AVX2(const uint8_t* src_yuy2,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width) {
  asm volatile(
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpsrlw      $0x8,%%ymm5,%%ymm5            \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x8,%%ymm1,%%ymm1            \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpand       %%ymm5,%%ymm0,%%ymm1          \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm1,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm0,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm1,%%ymm1           \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vextractf128 $0x0,%%ymm1,(%1)             \n"
      "vextractf128 $0x0,%%ymm0,0x00(%1,%2,1)    \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_yuy2),  // %0
        "+r"(dst_u),     // %1
        "+r"(dst_v),     // %2
        "+r"(width)      // %3
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}

void UYVYToYRow_AVX2(const uint8_t* src_uyvy, uint8_t* dst_y, int width) {
  asm volatile(
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x8,%%ymm1,%%ymm1            \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_uyvy),  // %0
        "+r"(dst_y),     // %1
        "+r"(width)      // %2
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}
void UYVYToUVRow_AVX2(const uint8_t* src_uyvy,
                      int stride_uyvy,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width) {
  asm volatile(
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpsrlw      $0x8,%%ymm5,%%ymm5            \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vpavgb      0x00(%0,%4,1),%%ymm0,%%ymm0   \n"
      "vpavgb      0x20(%0,%4,1),%%ymm1,%%ymm1   \n"
      "lea         0x40(%0),%0                   \n"
      "vpand       %%ymm5,%%ymm0,%%ymm0          \n"
      "vpand       %%ymm5,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpand       %%ymm5,%%ymm0,%%ymm1          \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm1,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm0,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm1,%%ymm1           \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vextractf128 $0x0,%%ymm1,(%1)             \n"
      "vextractf128 $0x0,%%ymm0,0x00(%1,%2,1)    \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_uyvy),               // %0
        "+r"(dst_u),                  // %1
        "+r"(dst_v),                  // %2
        "+r"(width)                   // %3
      : "r"((intptr_t)(stride_uyvy))  // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}

void UYVYToUV422Row_AVX2(const uint8_t* src_uyvy,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width) {
  asm volatile(
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpsrlw      $0x8,%%ymm5,%%ymm5            \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpand       %%ymm5,%%ymm0,%%ymm0          \n"
      "vpand       %%ymm5,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpand       %%ymm5,%%ymm0,%%ymm1          \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm1,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm0,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm1,%%ymm1           \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vextractf128 $0x0,%%ymm1,(%1)             \n"
      "vextractf128 $0x0,%%ymm0,0x00(%1,%2,1)    \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_uyvy),  // %0
        "+r"(dst_u),     // %1
        "+r"(dst_v),     // %2
        "+r"(width)      // %3
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}
#endif  // HAS_YUY2TOYROW_AVX2

#if !defined(LIBYUV_DISABLE_X86)
// Shuffle table for isolating alpha.
static const uvec8 kShuffleAlpha = {3u,  0x80, 3u,  0x80, 7u,  0x80, 7u,  0x80,
                                    11u, 0x80, 11u, 0x80, 15u, 0x80, 15u, 0x80};

// Blend 8 pixels at a time
#endif


#ifdef HAS_BLENDPLANEROW_AVX2
// Blend 32 pixels at a time.
// unsigned version of math
// =((A2*C2)+(B2*(255-C2))+255)/256
// signed version of math
// =(((A2-128)*C2)+((B2-128)*(255-C2))+32768+127)/256
void BlendPlaneRow_AVX2(const uint8_t* src0,
                        const uint8_t* src1,
                        const uint8_t* alpha,
                        uint8_t* dst,
                        int width) {
  asm volatile(
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpsllw      $0x8,%%ymm5,%%ymm5            \n"
      "mov         $0x80808080,%%eax             \n"
      "vmovd       %%eax,%%xmm6                  \n"
      "vbroadcastss %%xmm6,%%ymm6                \n"
      "mov         $0x807f807f,%%eax             \n"
      "vmovd       %%eax,%%xmm7                  \n"
      "vbroadcastss %%xmm7,%%ymm7                \n"
      "sub         %2,%0                         \n"
      "sub         %2,%1                         \n"
      "sub         %2,%3                         \n"

      // 32 pixel loop.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%2),%%ymm0                   \n"
      "vpunpckhbw  %%ymm0,%%ymm0,%%ymm3          \n"
      "vpunpcklbw  %%ymm0,%%ymm0,%%ymm0          \n"
      "vpxor       %%ymm5,%%ymm3,%%ymm3          \n"
      "vpxor       %%ymm5,%%ymm0,%%ymm0          \n"
      "vmovdqu     (%0,%2,1),%%ymm1              \n"
      "vmovdqu     (%1,%2,1),%%ymm2              \n"
      "vpunpckhbw  %%ymm2,%%ymm1,%%ymm4          \n"
      "vpunpcklbw  %%ymm2,%%ymm1,%%ymm1          \n"
      "vpsubb      %%ymm6,%%ymm4,%%ymm4          \n"
      "vpsubb      %%ymm6,%%ymm1,%%ymm1          \n"
      "vpmaddubsw  %%ymm4,%%ymm3,%%ymm3          \n"
      "vpmaddubsw  %%ymm1,%%ymm0,%%ymm0          \n"
      "vpaddw      %%ymm7,%%ymm3,%%ymm3          \n"
      "vpaddw      %%ymm7,%%ymm0,%%ymm0          \n"
      "vpsrlw      $0x8,%%ymm3,%%ymm3            \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm3,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%3,%2,1)              \n"
      "lea         0x20(%2),%2                   \n"
      "sub         $0x20,%4                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src0),   // %0
        "+r"(src1),   // %1
        "+r"(alpha),  // %2
        "+r"(dst),    // %3
        "+rm"(width)  // %4
        ::"memory",
        "cc", "eax", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_BLENDPLANEROW_AVX2

#if !defined(LIBYUV_DISABLE_X86)
// Shuffle table duplicating alpha.
static const vec8 kAttenuateShuffle = {6,    -128, 6,    -128, 6,  -128,
                                       -128, -128, 14,   -128, 14, -128,
                                       14,   -128, -128, -128};

// Attenuate 4 pixels at a time.
#endif

#ifdef HAS_ARGBATTENUATEROW_AVX2

// Shuffle table duplicating alpha.
static const lvec8 kAttenuateShuffle_AVX2 = {
    6,    -128, 6,    -128, 6,    -128, -128, -128, 14,   -128, 14,
    -128, 14,   -128, -128, -128, 22,   -128, 22,   -128, 22,   -128,
    -128, -128, 30,   -128, 30,   -128, 30,   -128, -128, -128};

// Attenuate 8 pixels at a time.
void ARGBAttenuateRow_AVX2(const uint8_t* src_argb,
                           uint8_t* dst_argb,
                           int width) {
  asm volatile(
      "vmovdqa     %3,%%ymm4                     \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpslld      $0x18,%%ymm5,%%ymm5           \n"
      "vpxor       %%ymm6,%%ymm6,%%ymm6          \n"
      "vpcmpeqb    %%ymm7,%%ymm7,%%ymm7          \n"
      "vpunpcklbw  %%ymm6,%%ymm7,%%ymm7          \n"
      "sub         %0,%1                         \n"

      // 8 pixel loop.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm6                   \n"
      "vpunpcklbw  %%ymm5,%%ymm6,%%ymm0          \n"
      "vpunpckhbw  %%ymm5,%%ymm6,%%ymm1          \n"
      "vpshufb     %%ymm4,%%ymm0,%%ymm2          \n"
      "vpshufb     %%ymm4,%%ymm1,%%ymm3          \n"
      "vpmullw     %%ymm2,%%ymm0,%%ymm0          \n"
      "vpmullw     %%ymm3,%%ymm1,%%ymm1          \n"
      "vpaddw      %%ymm7,%%ymm0,%%ymm0          \n"
      "vpaddw      %%ymm7,%%ymm1,%%ymm1          \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x8,%%ymm1,%%ymm1            \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpand       %%ymm5,%%ymm6,%%ymm1          \n"
      "vpor        %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,0x00(%0,%1,1)          \n"
      "lea         0x20(%0),%0                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),              // %0
        "+r"(dst_argb),              // %1
        "+r"(width)                  // %2
      : "m"(kAttenuateShuffle_AVX2)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ARGBATTENUATEROW_AVX2


#ifdef HAS_ARGBUNATTENUATEROW_AVX2
// Shuffle table duplicating alpha.
static const uvec8 kUnattenShuffleAlpha_AVX2 = {
    0u, 1u, 0u, 1u, 0u, 1u, 6u, 7u, 8u, 9u, 8u, 9u, 8u, 9u, 14u, 15u};
// Unattenuate 8 pixels at a time.
void ARGBUnattenuateRow_AVX2(const uint8_t* src_argb,
                             uint8_t* dst_argb,
                             int width) {
  uintptr_t alpha;
  asm volatile(
      "sub         %0,%1                         \n"
      "vbroadcastf128 %5,%%ymm5                  \n"

      // 8 pixel loop.
      LABELALIGN
      "1:          \n"
      // replace VPGATHER
      "movzb       0x03(%0),%3                   \n"
      "vmovd       0x00(%4,%3,4),%%xmm0          \n"
      "movzb       0x07(%0),%3                   \n"
      "vmovd       0x00(%4,%3,4),%%xmm1          \n"
      "movzb       0x0b(%0),%3                   \n"
      "vpunpckldq  %%xmm1,%%xmm0,%%xmm6          \n"
      "vmovd       0x00(%4,%3,4),%%xmm2          \n"
      "movzb       0x0f(%0),%3                   \n"
      "vmovd       0x00(%4,%3,4),%%xmm3          \n"
      "movzb       0x13(%0),%3                   \n"
      "vpunpckldq  %%xmm3,%%xmm2,%%xmm7          \n"
      "vmovd       0x00(%4,%3,4),%%xmm0          \n"
      "movzb       0x17(%0),%3                   \n"
      "vmovd       0x00(%4,%3,4),%%xmm1          \n"
      "movzb       0x1b(%0),%3                   \n"
      "vpunpckldq  %%xmm1,%%xmm0,%%xmm0          \n"
      "vmovd       0x00(%4,%3,4),%%xmm2          \n"
      "movzb       0x1f(%0),%3                   \n"
      "vmovd       0x00(%4,%3,4),%%xmm3          \n"
      "vpunpckldq  %%xmm3,%%xmm2,%%xmm2          \n"
      "vpunpcklqdq %%xmm7,%%xmm6,%%xmm3          \n"
      "vpunpcklqdq %%xmm2,%%xmm0,%%xmm0          \n"
      "vinserti128 $0x1,%%xmm0,%%ymm3,%%ymm3     \n"
      // end of VPGATHER

      "vmovdqu     (%0),%%ymm6                   \n"
      "vpunpcklbw  %%ymm6,%%ymm6,%%ymm0          \n"
      "vpunpckhbw  %%ymm6,%%ymm6,%%ymm1          \n"
      "vpunpcklwd  %%ymm3,%%ymm3,%%ymm2          \n"
      "vpunpckhwd  %%ymm3,%%ymm3,%%ymm3          \n"
      "vpshufb     %%ymm5,%%ymm2,%%ymm2          \n"
      "vpshufb     %%ymm5,%%ymm3,%%ymm3          \n"
      "vpmulhuw    %%ymm2,%%ymm0,%%ymm0          \n"
      "vpmulhuw    %%ymm3,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,0x00(%0,%1,1)          \n"
      "lea         0x20(%0),%0                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),                 // %0
        "+r"(dst_argb),                 // %1
        "+r"(width),                    // %2
        "=&r"(alpha)                    // %3
      : "r"(fixed_invtbl8),             // %4
        "m"(kUnattenShuffleAlpha_AVX2)  // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ARGBUNATTENUATEROW_AVX2


#if !defined(LIBYUV_DISABLE_X86)
//    b = (r * 35 + g * 68 + b * 17) >> 7
//    g = (r * 45 + g * 88 + b * 22) >> 7
//    r = (r * 50 + g * 98 + b * 24) >> 7
// Constant for ARGB color to sepia tone
static const vec8 kARGBToSepiaB = {17, 68, 35, 0, 17, 68, 35, 0,
                                   17, 68, 35, 0, 17, 68, 35, 0};

static const vec8 kARGBToSepiaG = {22, 88, 45, 0, 22, 88, 45, 0,
                                   22, 88, 45, 0, 22, 88, 45, 0};

static const vec8 kARGBToSepiaR = {24, 98, 50, 0, 24, 98, 50, 0,
                                   24, 98, 50, 0, 24, 98, 50, 0};

// Convert 8 ARGB pixels (32 bytes) to 8 Sepia ARGB pixels.
#endif





#ifdef HAS_ARGBMULTIPLYROW_AVX2
// Multiply 2 rows of ARGB pixels together, 8 pixels at a time.
void ARGBMultiplyRow_AVX2(const uint8_t* src_argb,
                          const uint8_t* src_argb1,
                          uint8_t* dst_argb,
                          int width) {
      asm volatile("vpxor       %%ymm5,%%ymm5,%%ymm5          \n"

               // 4 pixel loop.
               LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm1                   \n"
      "lea         0x20(%0),%0                   \n"
      "vmovdqu     (%1),%%ymm3                   \n"
      "lea         0x20(%1),%1                   \n"
      "vpunpcklbw  %%ymm1,%%ymm1,%%ymm0          \n"
      "vpunpckhbw  %%ymm1,%%ymm1,%%ymm1          \n"
      "vpunpcklbw  %%ymm5,%%ymm3,%%ymm2          \n"
      "vpunpckhbw  %%ymm5,%%ymm3,%%ymm3          \n"
      "vpmulhuw    %%ymm2,%%ymm0,%%ymm0          \n"
      "vpmulhuw    %%ymm3,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%2)                   \n"
      "lea         0x20(%2),%2                   \n"
      "sub         $0x8,%3                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_argb),   // %0
                 "+r"(src_argb1),  // %1
                 "+r"(dst_argb),   // %2
                 "+r"(width)       // %3
               :
               : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm5");
}
#endif  // HAS_ARGBMULTIPLYROW_AVX2


#ifdef HAS_ARGBADDROW_AVX2
// Add 2 rows of ARGB pixels together, 4 pixels at a time.
void ARGBAddRow_AVX2(const uint8_t* src_argb,
                     const uint8_t* src_argb1,
                     uint8_t* dst_argb,
                     int width) {
  asm volatile(
      // 4 pixel loop.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "lea         0x20(%0),%0                   \n"
      "vpaddusb    (%1),%%ymm0,%%ymm0            \n"
      "lea         0x20(%1),%1                   \n"
      "vmovdqu     %%ymm0,(%2)                   \n"
      "lea         0x20(%2),%2                   \n"
      "sub         $0x8,%3                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),   // %0
        "+r"(src_argb1),  // %1
        "+r"(dst_argb),   // %2
        "+r"(width)       // %3
      :
      : "memory", "cc", "xmm0");
}
#endif  // HAS_ARGBADDROW_AVX2


#ifdef HAS_ARGBSUBTRACTROW_AVX2
// Subtract 2 rows of ARGB pixels, 8 pixels at a time.
void ARGBSubtractRow_AVX2(const uint8_t* src_argb,
                          const uint8_t* src_argb1,
                          uint8_t* dst_argb,
                          int width) {
  asm volatile(
      // 4 pixel loop.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "lea         0x20(%0),%0                   \n"
      "vpsubusb    (%1),%%ymm0,%%ymm0            \n"
      "lea         0x20(%1),%1                   \n"
      "vmovdqu     %%ymm0,(%2)                   \n"
      "lea         0x20(%2),%2                   \n"
      "sub         $0x8,%3                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),   // %0
        "+r"(src_argb1),  // %1
        "+r"(dst_argb),   // %2
        "+r"(width)       // %3
      :
      : "memory", "cc", "xmm0");
}
#endif  // HAS_ARGBSUBTRACTROW_AVX2










#ifdef HAS_INTERPOLATEROW_AVX2
// Bilinear filter 32x2 -> 32x1
void InterpolateRow_AVX2(uint8_t* dst_ptr,
                         const uint8_t* src_ptr,
                         ptrdiff_t src_stride,
                         int width,
                         int source_y_fraction) {
  asm volatile(
      "sub         %1,%0                         \n"
      "cmp         $0x0,%3                       \n"
      "je          100f                          \n"
      "cmp         $0x80,%3                      \n"
      "je          50f                           \n"

      "vmovd       %3,%%xmm0                     \n"
      "neg         %3                            \n"
      "add         $0x100,%3                     \n"
      "vmovd       %3,%%xmm5                     \n"
      "vpunpcklbw  %%xmm0,%%xmm5,%%xmm5          \n"
      "vpunpcklwd  %%xmm5,%%xmm5,%%xmm5          \n"
      "vbroadcastss %%xmm5,%%ymm5                \n"
      "mov         $0x80808080,%%eax             \n"
      "vmovd       %%eax,%%xmm4                  \n"
      "vbroadcastss %%xmm4,%%ymm4                \n"

      // General purpose row blend.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%1),%%ymm0                   \n"
      "vmovdqu     0x00(%1,%4,1),%%ymm2          \n"
      "vpunpckhbw  %%ymm2,%%ymm0,%%ymm1          \n"
      "vpunpcklbw  %%ymm2,%%ymm0,%%ymm0          \n"
      "vpsubb      %%ymm4,%%ymm1,%%ymm1          \n"
      "vpsubb      %%ymm4,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm1,%%ymm5,%%ymm1          \n"
      "vpmaddubsw  %%ymm0,%%ymm5,%%ymm0          \n"
      "vpaddw      %%ymm4,%%ymm1,%%ymm1          \n"
      "vpaddw      %%ymm4,%%ymm0,%%ymm0          \n"
      "vpsrlw      $0x8,%%ymm1,%%ymm1            \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,0x00(%1,%0,1)          \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "jmp         99f                           \n"

      // Blend 50 / 50.
      LABELALIGN
      "50:         \n"
      "vmovdqu     (%1),%%ymm0                   \n"
      "vpavgb      0x00(%1,%4,1),%%ymm0,%%ymm0   \n"
      "vmovdqu     %%ymm0,0x00(%1,%0,1)          \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          50b                           \n"
      "jmp         99f                           \n"

      // Blend 100 / 0 - Copy row unchanged.
      LABELALIGN
      "100:        \n"
      "vmovdqu     (%1),%%ymm0                   \n"
      "vmovdqu     %%ymm0,0x00(%1,%0,1)          \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          100b                          \n"

      "99:         \n"
      "vzeroupper  \n"
      : "+r"(dst_ptr),               // %0
        "+r"(src_ptr),               // %1
        "+r"(width),                 // %2
        "+r"(source_y_fraction)      // %3
      : "r"((intptr_t)(src_stride))  // %4
      : "memory", "cc", "eax", "xmm0", "xmm1", "xmm2", "xmm4", "xmm5");
}
#endif  // HAS_INTERPOLATEROW_AVX2


#ifdef HAS_ARGBSHUFFLEROW_AVX2
// For BGRAToARGB, ABGRToARGB, RGBAToARGB, and ARGBToRGBA.
void ARGBShuffleRow_AVX2(const uint8_t* src_argb,
                         uint8_t* dst_argb,
                         const uint8_t* shuffler,
                         int width) {
      asm volatile("vbroadcastf128 (%3),%%ymm5                \n"

               LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpshufb     %%ymm5,%%ymm0,%%ymm0          \n"
      "vpshufb     %%ymm5,%%ymm1,%%ymm1          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_argb),  // %0
                 "+r"(dst_argb),  // %1
                 "+r"(width)      // %2
               : "r"(shuffler)    // %3
               : "memory", "cc", "xmm0", "xmm1", "xmm5");
}
#endif  // HAS_ARGBSHUFFLEROW_AVX2



#ifdef HAS_I422TOYUY2ROW_AVX2
void I422ToYUY2Row_AVX2(const uint8_t* src_y,
                        const uint8_t* src_u,
                        const uint8_t* src_v,
                        uint8_t* dst_yuy2,
                        int width) {
      asm volatile("sub         %1,%2                         \n"

               LABELALIGN
      "1:          \n"
      "vpmovzxbw   (%1),%%ymm1                   \n"
      "vpmovzxbw   0x00(%1,%2,1),%%ymm2          \n"
      "add         $0x10,%1                      \n"
      "vpsllw      $0x8,%%ymm2,%%ymm2            \n"
      "vpor        %%ymm1,%%ymm2,%%ymm2          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "add         $0x20,%0                      \n"
      "vpunpcklbw  %%ymm2,%%ymm0,%%ymm1          \n"
      "vpunpckhbw  %%ymm2,%%ymm0,%%ymm2          \n"
      "vextractf128 $0x0,%%ymm1,(%3)             \n"
      "vextractf128 $0x0,%%ymm2,0x10(%3)         \n"
      "vextractf128 $0x1,%%ymm1,0x20(%3)         \n"
      "vextractf128 $0x1,%%ymm2,0x30(%3)         \n"
      "lea         0x40(%3),%3                   \n"
      "sub         $0x20,%4                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_y),     // %0
                 "+r"(src_u),     // %1
                 "+r"(src_v),     // %2
                 "+r"(dst_yuy2),  // %3
                 "+rm"(width)     // %4
               :
               : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_I422TOYUY2ROW_AVX2

#ifdef HAS_I422TOUYVYROW_AVX2
void I422ToUYVYRow_AVX2(const uint8_t* src_y,
                        const uint8_t* src_u,
                        const uint8_t* src_v,
                        uint8_t* dst_uyvy,
                        int width) {
      asm volatile("sub         %1,%2                         \n"

               LABELALIGN
      "1:          \n"
      "vpmovzxbw   (%1),%%ymm1                   \n"
      "vpmovzxbw   0x00(%1,%2,1),%%ymm2          \n"
      "add         $0x10,%1                      \n"
      "vpsllw      $0x8,%%ymm2,%%ymm2            \n"
      "vpor        %%ymm1,%%ymm2,%%ymm2          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "add         $0x20,%0                      \n"
      "vpunpcklbw  %%ymm0,%%ymm2,%%ymm1          \n"
      "vpunpckhbw  %%ymm0,%%ymm2,%%ymm2          \n"
      "vextractf128 $0x0,%%ymm1,(%3)             \n"
      "vextractf128 $0x0,%%ymm2,0x10(%3)         \n"
      "vextractf128 $0x1,%%ymm1,0x20(%3)         \n"
      "vextractf128 $0x1,%%ymm2,0x30(%3)         \n"
      "lea         0x40(%3),%3                   \n"
      "sub         $0x20,%4                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_y),     // %0
                 "+r"(src_u),     // %1
                 "+r"(src_v),     // %2
                 "+r"(dst_uyvy),  // %3
                 "+rm"(width)     // %4
               :
               : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_I422TOUYVYROW_AVX2


#ifdef HAS_ARGBPOLYNOMIALROW_AVX2
void ARGBPolynomialRow_AVX2(const uint8_t* src_argb,
                            uint8_t* dst_argb,
                            const float* poly,
                            int width) {
  asm volatile(
      "vbroadcastf128 (%3),%%ymm4                \n"
      "vbroadcastf128 0x10(%3),%%ymm5            \n"
      "vbroadcastf128 0x20(%3),%%ymm6            \n"
      "vbroadcastf128 0x30(%3),%%ymm7            \n"

      // 2 pixel loop.
      LABELALIGN
      "1:          \n"
      "vpmovzxbd   (%0),%%ymm0                   \n"  // 2 ARGB pixels
      "lea         0x8(%0),%0                    \n"
      "vcvtdq2ps   %%ymm0,%%ymm0                 \n"  // X 8 floats
      "vmulps      %%ymm0,%%ymm0,%%ymm2          \n"  // X * X
      "vmulps      %%ymm7,%%ymm0,%%ymm3          \n"  // C3 * X
      "vfmadd132ps %%ymm5,%%ymm4,%%ymm0          \n"  // result = C0 + C1 * X
      "vfmadd231ps %%ymm6,%%ymm2,%%ymm0          \n"  // result += C2 * X * X
      "vfmadd231ps %%ymm3,%%ymm2,%%ymm0          \n"  // result += C3 * X * X *
                                                      // X
      "vcvttps2dq  %%ymm0,%%ymm0                 \n"
      "vpackusdw   %%ymm0,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpackuswb   %%xmm0,%%xmm0,%%xmm0          \n"
      "vmovq       %%xmm0,(%1)                   \n"
      "lea         0x8(%1),%1                    \n"
      "sub         $0x2,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_argb),  // %0
        "+r"(dst_argb),  // %1
        "+r"(width)      // %2
      : "r"(poly)        // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ARGBPOLYNOMIALROW_AVX2

#if !defined(LIBYUV_DISABLE_X86)
static float kScaleBias = 1.9259299444e-34f;
#endif

#ifdef HAS_HALFFLOATROW_AVX2
void HalfFloatRow_AVX2(const uint16_t* src,
                       uint16_t* dst,
                       float scale,
                       int width) {
  scale *= kScaleBias;
  asm volatile(
      "vbroadcastss %3, %%ymm4                   \n"
      "vpxor       %%ymm5,%%ymm5,%%ymm5          \n"
      "sub         %0,%1                         \n"

      // 16 pixel loop.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm2                   \n"  // 16 shorts
      "add         $0x20,%0                      \n"
      "vpunpckhwd  %%ymm5,%%ymm2,%%ymm3          \n"  // mutates
      "vpunpcklwd  %%ymm5,%%ymm2,%%ymm2          \n"
      "vcvtdq2ps   %%ymm3,%%ymm3                 \n"
      "vcvtdq2ps   %%ymm2,%%ymm2                 \n"
      "vmulps      %%ymm3,%%ymm4,%%ymm3          \n"
      "vmulps      %%ymm2,%%ymm4,%%ymm2          \n"
      "vpsrld      $0xd,%%ymm3,%%ymm3            \n"
      "vpsrld      $0xd,%%ymm2,%%ymm2            \n"
      "vpackssdw   %%ymm3, %%ymm2, %%ymm2        \n"  // unmutates
      "vmovdqu     %%ymm2,-0x20(%0,%1,1)         \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"

      "vzeroupper  \n"
      : "+r"(src),   // %0
        "+r"(dst),   // %1
        "+r"(width)  // %2
#if defined(__x86_64__)
      : "x"(scale)  // %3
#else
      : "m"(scale)    // %3
#endif
      : "memory", "cc", "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_HALFFLOATROW_AVX2

#ifdef HAS_HALFFLOATROW_F16C
void HalfFloatRow_F16C(const uint16_t* src,
                       uint16_t* dst,
                       float scale,
                       int width) {
  asm volatile(
      "vbroadcastss %3, %%ymm4                   \n"
      "sub         %0,%1                         \n"

      // 16 pixel loop.
      LABELALIGN
      "1:          \n"
      "vpmovzxwd   (%0),%%ymm2                   \n"  // 16 shorts -> 16 ints
      "vpmovzxwd   0x10(%0),%%ymm3               \n"
      "vcvtdq2ps   %%ymm2,%%ymm2                 \n"
      "vcvtdq2ps   %%ymm3,%%ymm3                 \n"
      "vmulps      %%ymm2,%%ymm4,%%ymm2          \n"
      "vmulps      %%ymm3,%%ymm4,%%ymm3          \n"
      "vcvtps2ph   $3, %%ymm2, %%xmm2            \n"
      "vcvtps2ph   $3, %%ymm3, %%xmm3            \n"
      "vmovdqu     %%xmm2,0x00(%0,%1,1)          \n"
      "vmovdqu     %%xmm3,0x10(%0,%1,1)          \n"
      "add         $0x20,%0                      \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),   // %0
        "+r"(dst),   // %1
        "+r"(width)  // %2
#if defined(__x86_64__)
      : "x"(scale)  // %3
#else
      : "m"(scale)    // %3
#endif
      : "memory", "cc", "xmm2", "xmm3", "xmm4");
}
#endif  // HAS_HALFFLOATROW_F16C

#ifdef HAS_HALFFLOATROW_F16C
void HalfFloat1Row_F16C(const uint16_t* src, uint16_t* dst, float, int width) {
  asm volatile(
      "sub         %0,%1                         \n"
      // 16 pixel loop.
      LABELALIGN
      "1:          \n"
      "vpmovzxwd   (%0),%%ymm2                   \n"  // 16 shorts -> 16 ints
      "vpmovzxwd   0x10(%0),%%ymm3               \n"
      "vcvtdq2ps   %%ymm2,%%ymm2                 \n"
      "vcvtdq2ps   %%ymm3,%%ymm3                 \n"
      "vcvtps2ph   $3, %%ymm2, %%xmm2            \n"
      "vcvtps2ph   $3, %%ymm3, %%xmm3            \n"
      "vmovdqu     %%xmm2,0x00(%0,%1,1)          \n"
      "vmovdqu     %%xmm3,0x10(%0,%1,1)          \n"
      "add         $0x20,%0                      \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),   // %0
        "+r"(dst),   // %1
        "+r"(width)  // %2
      :
      : "memory", "cc", "xmm2", "xmm3");
}
#endif  // HAS_HALFFLOATROW_F16C

#ifdef HAS_ARGBCOLORTABLEROW_X86
// Tranform ARGB pixels with color table.
void ARGBColorTableRow_X86(uint8_t* dst_argb,
                           const uint8_t* table_argb,
                           int width) {
  uintptr_t pixel_temp;
  asm volatile(
      // 1 pixel loop.
      LABELALIGN
      "1:          \n"
      "movzb       (%0),%1                       \n"
      "lea         0x4(%0),%0                    \n"
      "movzb       0x00(%3,%1,4),%1              \n"
      "mov         %b1,-0x4(%0)                  \n"
      "movzb       -0x3(%0),%1                   \n"
      "movzb       0x01(%3,%1,4),%1              \n"
      "mov         %b1,-0x3(%0)                  \n"
      "movzb       -0x2(%0),%1                   \n"
      "movzb       0x02(%3,%1,4),%1              \n"
      "mov         %b1,-0x2(%0)                  \n"
      "movzb       -0x1(%0),%1                   \n"
      "movzb       0x03(%3,%1,4),%1              \n"
      "mov         %b1,-0x1(%0)                  \n"
      "dec         %2                            \n"
      "jg          1b                            \n"
      : "+r"(dst_argb),     // %0
        "=&d"(pixel_temp),  // %1
        "+r"(width)         // %2
      : "r"(table_argb)     // %3
      : "memory", "cc");
}
#endif  // HAS_ARGBCOLORTABLEROW_X86

#ifdef HAS_RGBCOLORTABLEROW_X86
// Tranform RGB pixels with color table.
void RGBColorTableRow_X86(uint8_t* dst_argb,
                          const uint8_t* table_argb,
                          int width) {
  uintptr_t pixel_temp;
  asm volatile(
      // 1 pixel loop.
      LABELALIGN
      "1:          \n"
      "movzb       (%0),%1                       \n"
      "lea         0x4(%0),%0                    \n"
      "movzb       0x00(%3,%1,4),%1              \n"
      "mov         %b1,-0x4(%0)                  \n"
      "movzb       -0x3(%0),%1                   \n"
      "movzb       0x01(%3,%1,4),%1              \n"
      "mov         %b1,-0x3(%0)                  \n"
      "movzb       -0x2(%0),%1                   \n"
      "movzb       0x02(%3,%1,4),%1              \n"
      "mov         %b1,-0x2(%0)                  \n"
      "dec         %2                            \n"
      "jg          1b                            \n"
      : "+r"(dst_argb),     // %0
        "=&d"(pixel_temp),  // %1
        "+r"(width)         // %2
      : "r"(table_argb)     // %3
      : "memory", "cc");
}
#endif  // HAS_RGBCOLORTABLEROW_X86


static const uvec8 kYUV24Shuffle[3] = {
    {8, 9, 0, 8, 9, 1, 10, 11, 2, 10, 11, 3, 12, 13, 4, 12},
    {9, 1, 10, 11, 2, 10, 11, 3, 12, 13, 4, 12, 13, 5, 14, 15},
    {2, 10, 11, 3, 12, 13, 4, 12, 13, 5, 14, 15, 6, 14, 15, 7}};

// Convert biplanar NV21 to packed YUV24
// NV21 has VU in memory for chroma.
// YUV24 is VUY in memory

// Convert biplanar NV21 to packed YUV24
// NV21 has VU in memory for chroma.
// YUV24 is VUY in memory
void NV21ToYUV24Row_AVX2(const uint8_t* src_y,
                         const uint8_t* src_vu,
                         uint8_t* dst_yuv24,
                         int width) {
  asm volatile(
      "sub         %0,%1                         \n"
      "vbroadcastf128 (%4),%%ymm4                \n"  // 3 shuffler constants
      "vbroadcastf128 16(%4),%%ymm5              \n"
      "vbroadcastf128 32(%4),%%ymm6              \n"

      "1:          \n"
      "vmovdqu     (%0),%%ymm2                   \n"  // load 32 Y values
      "vmovdqu     (%0,%1),%%ymm3                \n"  // load 16 VU values
      "lea         32(%0),%0                     \n"
      "vshufps     $0x44,%%ymm3,%%ymm2,%%ymm0    \n"  // Y 0..7,  UV 0..3
      "vshufps     $0x99,%%ymm3,%%ymm2,%%ymm1    \n"  // Y 4..11, UV 2..5
      "vshufps     $0xee,%%ymm3,%%ymm2,%%ymm2    \n"  // Y 8..15, UV 4..7
      "vpshufb     %%ymm4,%%ymm0,%%ymm0          \n"  // weave into YUV24
      "vpshufb     %%ymm5,%%ymm1,%%ymm1          \n"
      "vpshufb     %%ymm6,%%ymm2,%%ymm2          \n"
      "vperm2i128  $0x20,%%ymm1,%%ymm0,%%ymm3    \n"
      "vperm2i128  $0x30,%%ymm0,%%ymm2,%%ymm0    \n"
      "vperm2i128  $0x31,%%ymm2,%%ymm1,%%ymm1    \n"
      "vmovdqu     %%ymm3,(%2)                   \n"
      "vmovdqu     %%ymm0,32(%2)                 \n"
      "vmovdqu     %%ymm1,64(%2)                 \n"
      "lea         96(%2),%2                     \n"
      "sub         $32,%3                        \n"  // 32 pixels per loop
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_y),            // %0
        "+r"(src_vu),           // %1
        "+r"(dst_yuv24),        // %2
        "+r"(width)             // %3
      : "r"(&kYUV24Shuffle[0])  // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}

#ifdef HAS_NV21ToYUV24ROW_AVX512
// The following VMBI VEX256 code tests okay with the intelsde emulator.
static const lvec8 kYUV24Perm[3] = {
    {32, 33, 0,  32, 33, 1,  34, 35, 2,  34, 35, 3,  36, 37, 4,  36,
     37, 5,  38, 39, 6,  38, 39, 7,  40, 41, 8,  40, 41, 9,  42, 43},
    {10, 42, 43, 11, 44, 45, 12, 44, 45, 13, 46, 47, 14, 46, 47, 15,
     48, 49, 16, 48, 49, 17, 50, 51, 18, 50, 51, 19, 52, 53, 20, 52},
    {53, 21, 54, 55, 22, 54, 55, 23, 56, 57, 24, 56, 57, 25, 58, 59,
     26, 58, 59, 27, 60, 61, 28, 60, 61, 29, 62, 63, 30, 62, 63, 31}};

void NV21ToYUV24Row_AVX512(const uint8_t* src_y,
                           const uint8_t* src_vu,
                           uint8_t* dst_yuv24,
                           int width) {
  asm volatile(
      "sub         %0,%1                         \n"
      "vmovdqa     (%4),%%ymm4                   \n"  // 3 shuffler constants
      "vmovdqa     32(%4),%%ymm5                 \n"
      "vmovdqa     64(%4),%%ymm6                 \n" LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm2                   \n"  // load 32 Y values
      "vmovdqu     (%0,%1),%%ymm3                \n"  // load 16 VU values
      "lea         32(%0),%0                     \n"
      "vmovdqa     %%ymm2, %%ymm0                \n"
      "vmovdqa     %%ymm2, %%ymm1                \n"
      "vpermt2b    %%ymm3,%%ymm4,%%ymm0          \n"
      "vpermt2b    %%ymm3,%%ymm5,%%ymm1          \n"
      "vpermt2b    %%ymm3,%%ymm6,%%ymm2          \n"
      "vmovdqu     %%ymm0,(%2)                   \n"
      "vmovdqu     %%ymm1,32(%2)                 \n"
      "vmovdqu     %%ymm2,64(%2)                 \n"
      "lea         96(%2),%2                     \n"
      "sub         $32,%3                        \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_y),         // %0
        "+r"(src_vu),        // %1
        "+r"(dst_yuv24),     // %2
        "+r"(width)          // %3
      : "r"(&kYUV24Perm[0])  // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}

#endif  // HAS_NV21ToYUV24ROW_AVX512

#if !defined(LIBYUV_DISABLE_X86)

// Shuffle table for reversing the bytes.
static const uvec8 kShuffleUVToVU = {1u, 0u, 3u,  2u,  5u,  4u,  7u,  6u,
                                     9u, 8u, 11u, 10u, 13u, 12u, 15u, 14u};

// Convert UV plane of NV12 to VU of NV21.
#endif

#ifdef HAS_SWAPUVROW_AVX2
void SwapUVRow_AVX2(const uint8_t* src_uv, uint8_t* dst_vu, int width) {
      asm volatile("vbroadcastf128 %3,%%ymm5                  \n"

               LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpshufb     %%ymm5,%%ymm0,%%ymm0          \n"
      "vpshufb     %%ymm5,%%ymm1,%%ymm1          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_uv),        // %0
                 "+r"(dst_vu),        // %1
                 "+r"(width)          // %2
               : "m"(kShuffleUVToVU)  // %3
               : "memory", "cc", "xmm0", "xmm1", "xmm5");
}
#endif  // HAS_SWAPUVROW_AVX2


void HalfMergeUVRow_AVX2(const uint8_t* src_u,
                         int src_stride_u,
                         const uint8_t* src_v,
                         int src_stride_v,
                         uint8_t* dst_uv,
                         int width) {
  asm volatile(
      "vpcmpeqb    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpabsb      %%ymm4,%%ymm4                 \n"
      "vpxor       %%ymm5,%%ymm5,%%ymm5          \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"  // load 32 U values
      "vmovdqu     (%1),%%ymm1                   \n"  // load 32 V values
      "vmovdqu     0(%0,%4,1),%%ymm2             \n"  // 32 from next row
      "vmovdqu     0(%1,%5,1),%%ymm3             \n"
      "lea         0x20(%0),%0                   \n"
      "vpmaddubsw  %%ymm4,%%ymm0,%%ymm0          \n"  // half size
      "vpmaddubsw  %%ymm4,%%ymm1,%%ymm1          \n"
      "vpmaddubsw  %%ymm4,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm4,%%ymm3,%%ymm3          \n"
      "lea         0x20(%1),%1                   \n"
      "vpaddw      %%ymm2,%%ymm0,%%ymm0          \n"
      "vpaddw      %%ymm3,%%ymm1,%%ymm1          \n"
      "vpsrlw      $0x1,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x1,%%ymm1,%%ymm1            \n"
      "vpavgw      %%ymm5,%%ymm0,%%ymm0          \n"
      "vpavgw      %%ymm5,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm0,%%ymm0,%%ymm0          \n"
      "vpackuswb   %%ymm1,%%ymm1,%%ymm1          \n"
      "vpunpcklbw  %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%2)                   \n"  // store 16 UV pixels
      "lea         0x20(%2),%2                   \n"
      "sub         $0x20,%3                      \n"  // 32 src pixels per loop
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_u),                    // %0
        "+r"(src_v),                    // %1
        "+r"(dst_uv),                   // %2
        "+r"(width)                     // %3
      : "r"((intptr_t)(src_stride_u)),  // %4
        "r"((intptr_t)(src_stride_v))   // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}


#ifdef HAS_CONVERT16TO8ROW_AVX2
void Convert8To8Row_AVX2(const uint8_t* src_y,
                         uint8_t* dst_y,
                         int scale,
                         int bias,
                         int width) {
  asm volatile(
      "sub         %0,%1                         \n"
      "vmovd       %3,%%xmm2                     \n"
      "vmovd       %4,%%xmm3                     \n"
      "vpbroadcastw %%xmm2,%%ymm2                \n"
      "vpbroadcastb %%xmm3,%%ymm3                \n"
      "vpxor       %%ymm4,%%ymm4,%%ymm4          \n"
      "vpsllw      $8,%%ymm2,%%ymm2              \n"

      // 32 pixels per loop.
      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vpunpckhbw  %%ymm4,%%ymm0,%%ymm1          \n"  // mutates
      "vpunpcklbw  %%ymm4,%%ymm0,%%ymm0          \n"
      "vpmulhuw    %%ymm2,%%ymm0,%%ymm0          \n"
      "vpmulhuw    %%ymm2,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"  // unmutates
      "vpaddb      %%ymm3,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%0,%1)                \n"
      "add         $0x20,%0                      \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_y),  // %0
        "+r"(dst_y),  // %1
        "+r"(width)   // %2
      : "r"(scale),   // %3
        "r"(bias)     // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}
#endif  // HAS_CONVERT16TO8ROW_AVX2

#endif  // defined(__x86_64__) || defined(__i386__)

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
