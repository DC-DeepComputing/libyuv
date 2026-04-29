/*
 *  Copyright 2013 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/row.h"
#include "libyuv/scale_row.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// This module is for GCC x86 and x64.
#if !defined(LIBYUV_DISABLE_X86) &&               \
    (defined(__x86_64__) || defined(__i386__)) && \
    !defined(LIBYUV_ENABLE_ROWWIN)

// Offsets for source bytes 0 to 9
static const uvec8 kShuf0 = {0,   1,   3,   4,   5,   7,   8,   9,
                             128, 128, 128, 128, 128, 128, 128, 128};

// Offsets for source bytes 11 to 20 with 8 subtracted = 3 to 12.
static const uvec8 kShuf1 = {3,   4,   5,   7,   8,   9,   11,  12,
                             128, 128, 128, 128, 128, 128, 128, 128};

// Offsets for source bytes 21 to 31 with 16 subtracted = 5 to 31.
static const uvec8 kShuf2 = {5,   7,   8,   9,   11,  12,  13,  15,
                             128, 128, 128, 128, 128, 128, 128, 128};

// Offsets for source bytes 0 to 10
static const uvec8 kShuf01 = {0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9, 9, 10};

// Offsets for source bytes 10 to 21 with 8 subtracted = 3 to 13.
static const uvec8 kShuf11 = {2, 3, 4, 5,  5,  6,  6,  7,
                              8, 9, 9, 10, 10, 11, 12, 13};

// Offsets for source bytes 21 to 31 with 16 subtracted = 5 to 31.
static const uvec8 kShuf21 = {5,  6,  6,  7,  8,  9,  9,  10,
                              10, 11, 12, 13, 13, 14, 14, 15};

// Coefficients for source bytes 0 to 10
static const uvec8 kMadd01 = {3, 1, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 3, 1, 2, 2};

// Coefficients for source bytes 10 to 21
static const uvec8 kMadd11 = {1, 3, 3, 1, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 3, 1};

// Coefficients for source bytes 21 to 31
static const uvec8 kMadd21 = {2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3};

// Coefficients for source bytes 21 to 31
static const vec16 kRound34 = {2, 2, 2, 2, 2, 2, 2, 2};

static const uvec8 kShuf38a = {0,   3,   6,   8,   11,  14,  128, 128,
                               128, 128, 128, 128, 128, 128, 128, 128};

static const uvec8 kShuf38b = {128, 128, 128, 128, 128, 128, 0,   3,
                               6,   8,   11,  14,  128, 128, 128, 128};

// Arrange words 0,3,6 into 0,1,2
static const uvec8 kShufAc = {0,   1,   6,   7,   12,  13,  128, 128,
                              128, 128, 128, 128, 128, 128, 128, 128};

// Arrange words 0,3,6 into 3,4,5
static const uvec8 kShufAc3 = {128, 128, 128, 128, 128, 128, 0,   1,
                               6,   7,   12,  13,  128, 128, 128, 128};

// Scaling values for boxes of 3x3 and 2x3
static const uvec16 kScaleAc33 = {65536 / 9, 65536 / 9, 65536 / 6, 65536 / 9,
                                  65536 / 9, 65536 / 6, 0,         0};

// Arrange first value for pixels 0,1,2,3,4,5
static const uvec8 kShufAb0 = {0,  128, 3,  128, 6,   128, 8,   128,
                               11, 128, 14, 128, 128, 128, 128, 128};

// Arrange second value for pixels 0,1,2,3,4,5
static const uvec8 kShufAb1 = {1,  128, 4,  128, 7,   128, 9,   128,
                               12, 128, 15, 128, 128, 128, 128, 128};

// Arrange third value for pixels 0,1,2,3,4,5
static const uvec8 kShufAb2 = {2,  128, 5,   128, 128, 128, 10,  128,
                               13, 128, 128, 128, 128, 128, 128, 128};

// Scaling values for boxes of 3x2 and 2x2
static const uvec16 kScaleAb2 = {65536 / 3, 65536 / 3, 65536 / 2, 65536 / 3,
                                 65536 / 3, 65536 / 2, 0,         0};

// GCC versions of row functions are verbatim conversions from Visual C.
// Generated using gcc disassembly on Visual C object file:
// objdump -D yuvscaler.obj >yuvscaler.txt







#ifdef HAS_SCALEROWDOWN2_AVX2
void ScaleRowDown2_AVX2(const uint8_t* src_ptr,
                        ptrdiff_t src_stride,
                        uint8_t* dst_ptr,
                        int dst_width) {
  (void)src_stride;
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
      : "+r"(src_ptr),   // %0
        "+r"(dst_ptr),   // %1
        "+r"(dst_width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1");
}

void ScaleRowDown2Linear_AVX2(const uint8_t* src_ptr,
                              ptrdiff_t src_stride,
                              uint8_t* dst_ptr,
                              int dst_width) {
  (void)src_stride;
  asm volatile(
      "vpcmpeqb    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpabsb      %%ymm4,%%ymm4                 \n"
      "vpxor       %%ymm5,%%ymm5,%%ymm5          \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpmaddubsw  %%ymm4,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm4,%%ymm1,%%ymm1          \n"
      "vpavgw      %%ymm5,%%ymm0,%%ymm0          \n"
      "vpavgw      %%ymm5,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),   // %0
        "+r"(dst_ptr),   // %1
        "+r"(dst_width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm4", "xmm5");
}

void ScaleRowDown2Box_AVX2(const uint8_t* src_ptr,
                           ptrdiff_t src_stride,
                           uint8_t* dst_ptr,
                           int dst_width) {
  asm volatile(
      "vpcmpeqb    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpabsb      %%ymm4,%%ymm4                 \n"
      "vpxor       %%ymm5,%%ymm5,%%ymm5          \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x00(%0,%3,1),%%ymm2          \n"
      "vmovdqu     0x20(%0,%3,1),%%ymm3          \n"
      "lea         0x40(%0),%0                   \n"
      "vpmaddubsw  %%ymm4,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm4,%%ymm1,%%ymm1          \n"
      "vpmaddubsw  %%ymm4,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm4,%%ymm3,%%ymm3          \n"
      "vpaddw      %%ymm2,%%ymm0,%%ymm0          \n"
      "vpaddw      %%ymm3,%%ymm1,%%ymm1          \n"
      "vpsrlw      $0x1,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x1,%%ymm1,%%ymm1            \n"
      "vpavgw      %%ymm5,%%ymm0,%%ymm0          \n"
      "vpavgw      %%ymm5,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),   // %0
        "+r"(dst_ptr),   // %1
        "+r"(dst_width)  // %2
      : "r"(src_stride)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm5");
}
#endif  // HAS_SCALEROWDOWN2_AVX2





#ifdef HAS_SCALEROWDOWN4_AVX2
void ScaleRowDown4_AVX2(const uint8_t* src_ptr,
                        ptrdiff_t src_stride,
                        uint8_t* dst_ptr,
                        int dst_width) {
  (void)src_stride;
  asm volatile(
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpsrld      $0x18,%%ymm5,%%ymm5           \n"
      "vpslld      $0x10,%%ymm5,%%ymm5           \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpand       %%ymm5,%%ymm0,%%ymm0          \n"
      "vpand       %%ymm5,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm0,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%xmm0,(%1)                   \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),   // %0
        "+r"(dst_ptr),   // %1
        "+r"(dst_width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}

void ScaleRowDown4Box_AVX2(const uint8_t* src_ptr,
                           ptrdiff_t src_stride,
                           uint8_t* dst_ptr,
                           int dst_width) {
  asm volatile(
      "vpcmpeqb    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpabsw      %%ymm4,%%ymm5                 \n"
      "vpabsb      %%ymm4,%%ymm4                 \n"  // 0x0101
      "vpsllw      $0x3,%%ymm5,%%ymm5            \n"  // 0x0008

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x00(%0,%3,1),%%ymm2          \n"
      "vmovdqu     0x20(%0,%3,1),%%ymm3          \n"
      "vpmaddubsw  %%ymm4,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm4,%%ymm1,%%ymm1          \n"
      "vpmaddubsw  %%ymm4,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm4,%%ymm3,%%ymm3          \n"
      "vpaddw      %%ymm2,%%ymm0,%%ymm0          \n"
      "vpaddw      %%ymm3,%%ymm1,%%ymm1          \n"
      "vmovdqu     0x00(%0,%3,2),%%ymm2          \n"
      "vmovdqu     0x20(%0,%3,2),%%ymm3          \n"
      "vpmaddubsw  %%ymm4,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm4,%%ymm3,%%ymm3          \n"
      "vpaddw      %%ymm2,%%ymm0,%%ymm0          \n"
      "vpaddw      %%ymm3,%%ymm1,%%ymm1          \n"
      "vmovdqu     0x00(%0,%4,1),%%ymm2          \n"
      "vmovdqu     0x20(%0,%4,1),%%ymm3          \n"
      "lea         0x40(%0),%0                   \n"
      "vpmaddubsw  %%ymm4,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm4,%%ymm3,%%ymm3          \n"
      "vpaddw      %%ymm2,%%ymm0,%%ymm0          \n"
      "vpaddw      %%ymm3,%%ymm1,%%ymm1          \n"
      "vphaddw     %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpaddw      %%ymm5,%%ymm0,%%ymm0          \n"
      "vpsrlw      $0x4,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm0,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%xmm0,(%1)                   \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),       // %0
        "+r"(dst_ptr),       // %1
        "+r"(dst_width)      // %2
      : "r"(src_stride),     // %3
        "r"(src_stride * 3)  // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_SCALEROWDOWN4_AVX2













static const uvec8 kLinearShuffleFar = {2,  3,  0, 1, 6,  7,  4,  5,
                                        10, 11, 8, 9, 14, 15, 12, 13};

static const uvec8 kLinearMadd31 = {3, 1, 1, 3, 3, 1, 1, 3,
                                    3, 1, 1, 3, 3, 1, 1, 3};









#ifdef HAS_SCALEROWUP2_LINEAR_AVX2
void ScaleRowUp2_Linear_AVX2(const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             int dst_width) {
  asm volatile(
      "vpcmpeqw    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpsrlw      $15,%%ymm4,%%ymm4             \n"
      "vpsllw      $1,%%ymm4,%%ymm4              \n"  // all 2
      "vbroadcastf128 %3,%%ymm3                  \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%xmm0                   \n"  // 0123456789ABCDEF
      "vmovdqu     1(%0),%%xmm1                  \n"  // 123456789ABCDEF0
      "vpermq      $0b11011000,%%ymm0,%%ymm0     \n"
      "vpermq      $0b11011000,%%ymm1,%%ymm1     \n"
      "vpunpcklwd  %%ymm0,%%ymm0,%%ymm0          \n"
      "vpunpcklwd  %%ymm1,%%ymm1,%%ymm1          \n"
      "vpunpckhdq  %%ymm1,%%ymm0,%%ymm2          \n"
      "vpunpckldq  %%ymm1,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm3,%%ymm2,%%ymm1          \n"  // 3*near+far (hi)
      "vpmaddubsw  %%ymm3,%%ymm0,%%ymm0          \n"  // 3*near+far (lo)
      "vpaddw      %%ymm4,%%ymm0,%%ymm0          \n"  // 3*near+far+2 (lo)
      "vpaddw      %%ymm4,%%ymm1,%%ymm1          \n"  // 3*near+far+2 (hi)
      "vpsrlw      $2,%%ymm0,%%ymm0              \n"  // 3/4*near+1/4*far (lo)
      "vpsrlw      $2,%%ymm1,%%ymm1              \n"  // 3/4*near+1/4*far (hi)
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"

      "lea         0x10(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"  // 16 sample to 32 sample
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),      // %0
        "+r"(dst_ptr),      // %1
        "+r"(dst_width)     // %2
      : "m"(kLinearMadd31)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}
#endif

#ifdef HAS_SCALEROWUP2_BILINEAR_AVX2
void ScaleRowUp2_Bilinear_AVX2(const uint8_t* src_ptr,
                               ptrdiff_t src_stride,
                               uint8_t* dst_ptr,
                               ptrdiff_t dst_stride,
                               int dst_width) {
  asm volatile(
      "vpcmpeqw    %%ymm6,%%ymm6,%%ymm6          \n"
      "vpsrlw      $15,%%ymm6,%%ymm6             \n"
      "vpsllw      $3,%%ymm6,%%ymm6              \n"  // all 8
      "vbroadcastf128 %5,%%ymm7                  \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%xmm0                   \n"  // 0123456789ABCDEF
      "vmovdqu     1(%0),%%xmm1                  \n"  // 123456789ABCDEF0
      "vpermq      $0b11011000,%%ymm0,%%ymm0     \n"
      "vpermq      $0b11011000,%%ymm1,%%ymm1     \n"
      "vpunpcklwd  %%ymm0,%%ymm0,%%ymm0          \n"
      "vpunpcklwd  %%ymm1,%%ymm1,%%ymm1          \n"
      "vpunpckhdq  %%ymm1,%%ymm0,%%ymm2          \n"
      "vpunpckldq  %%ymm1,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm7,%%ymm2,%%ymm1          \n"  // 3*near+far (1, hi)
      "vpmaddubsw  %%ymm7,%%ymm0,%%ymm0          \n"  // 3*near+far (1, lo)

      "vmovdqu     (%0,%3),%%xmm2                \n"  // 0123456789ABCDEF
      "vmovdqu     1(%0,%3),%%xmm3               \n"  // 123456789ABCDEF0
      "vpermq      $0b11011000,%%ymm2,%%ymm2     \n"
      "vpermq      $0b11011000,%%ymm3,%%ymm3     \n"
      "vpunpcklwd  %%ymm2,%%ymm2,%%ymm2          \n"
      "vpunpcklwd  %%ymm3,%%ymm3,%%ymm3          \n"
      "vpunpckhdq  %%ymm3,%%ymm2,%%ymm4          \n"
      "vpunpckldq  %%ymm3,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm7,%%ymm4,%%ymm3          \n"  // 3*near+far (2, hi)
      "vpmaddubsw  %%ymm7,%%ymm2,%%ymm2          \n"  // 3*near+far (2, lo)

      // ymm0 ymm1
      // ymm2 ymm3

      "vpaddw      %%ymm0,%%ymm0,%%ymm4          \n"  // 6*near+2*far (1, lo)
      "vpaddw      %%ymm6,%%ymm2,%%ymm5          \n"  // 3*near+far+8 (2, lo)
      "vpaddw      %%ymm4,%%ymm0,%%ymm4          \n"  // 9*near+3*far (1, lo)
      "vpaddw      %%ymm4,%%ymm5,%%ymm4          \n"  // 9 3 3 1 + 8 (1, lo)
      "vpsrlw      $4,%%ymm4,%%ymm4              \n"  // ^ div by 16 (1, lo)

      "vpaddw      %%ymm2,%%ymm2,%%ymm5          \n"  // 6*near+2*far (2, lo)
      "vpaddw      %%ymm6,%%ymm0,%%ymm0          \n"  // 3*near+far+8 (1, lo)
      "vpaddw      %%ymm5,%%ymm2,%%ymm5          \n"  // 9*near+3*far (2, lo)
      "vpaddw      %%ymm5,%%ymm0,%%ymm5          \n"  // 9 3 3 1 + 8 (2, lo)
      "vpsrlw      $4,%%ymm5,%%ymm5              \n"  // ^ div by 16 (2, lo)

      "vpaddw      %%ymm1,%%ymm1,%%ymm0          \n"  // 6*near+2*far (1, hi)
      "vpaddw      %%ymm6,%%ymm3,%%ymm2          \n"  // 3*near+far+8 (2, hi)
      "vpaddw      %%ymm0,%%ymm1,%%ymm0          \n"  // 9*near+3*far (1, hi)
      "vpaddw      %%ymm0,%%ymm2,%%ymm0          \n"  // 9 3 3 1 + 8 (1, hi)
      "vpsrlw      $4,%%ymm0,%%ymm0              \n"  // ^ div by 16 (1, hi)

      "vpaddw      %%ymm3,%%ymm3,%%ymm2          \n"  // 6*near+2*far (2, hi)
      "vpaddw      %%ymm6,%%ymm1,%%ymm1          \n"  // 3*near+far+8 (1, hi)
      "vpaddw      %%ymm2,%%ymm3,%%ymm2          \n"  // 9*near+3*far (2, hi)
      "vpaddw      %%ymm2,%%ymm1,%%ymm2          \n"  // 9 3 3 1 + 8 (2, hi)
      "vpsrlw      $4,%%ymm2,%%ymm2              \n"  // ^ div by 16 (2, hi)

      "vpackuswb   %%ymm0,%%ymm4,%%ymm4          \n"
      "vmovdqu     %%ymm4,(%1)                   \n"  // store above
      "vpackuswb   %%ymm2,%%ymm5,%%ymm5          \n"
      "vmovdqu     %%ymm5,(%1,%4)                \n"  // store below

      "lea         0x10(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"  // 16 sample to 32 sample
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),      // %0
        "+r"(dst_ptr),      // %1
        "+r"(dst_width)     // %2
      : "r"(src_stride),    // %3
        "r"(dst_stride),    // %4
        "m"(kLinearMadd31)  // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif

#ifdef HAS_SCALEROWUP2_LINEAR_12_AVX2
void ScaleRowUp2_Linear_12_AVX2(const uint16_t* src_ptr,
                                uint16_t* dst_ptr,
                                int dst_width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm5                  \n"
      "vpcmpeqw    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpsrlw      $15,%%ymm4,%%ymm4             \n"
      "vpsllw      $1,%%ymm4,%%ymm4              \n"  // all 2

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"  // 0123456789ABCDEF (16b)
      "vmovdqu     2(%0),%%ymm1                  \n"  // 123456789ABCDEF0 (16b)

      "vpermq      $0b11011000,%%ymm0,%%ymm0     \n"  // 012389AB4567CDEF
      "vpermq      $0b11011000,%%ymm1,%%ymm1     \n"  // 12349ABC5678DEF0

      "vpunpckhwd  %%ymm1,%%ymm0,%%ymm2          \n"  // 899AABBCCDDEEFF0 (near)
      "vpunpcklwd  %%ymm1,%%ymm0,%%ymm0          \n"  // 0112233445566778 (near)
      "vpshufb     %%ymm5,%%ymm2,%%ymm3          \n"  // 98A9BACBDCEDFE0F (far)
      "vpshufb     %%ymm5,%%ymm0,%%ymm1          \n"  // 1021324354657687 (far)

      "vpaddw      %%ymm4,%%ymm1,%%ymm1          \n"  // far+2
      "vpaddw      %%ymm4,%%ymm3,%%ymm3          \n"  // far+2
      "vpaddw      %%ymm0,%%ymm1,%%ymm1          \n"  // near+far+2
      "vpaddw      %%ymm2,%%ymm3,%%ymm3          \n"  // near+far+2
      "vpaddw      %%ymm0,%%ymm0,%%ymm0          \n"  // 2*near
      "vpaddw      %%ymm2,%%ymm2,%%ymm2          \n"  // 2*near
      "vpaddw      %%ymm0,%%ymm1,%%ymm0          \n"  // 3*near+far+2
      "vpaddw      %%ymm2,%%ymm3,%%ymm2          \n"  // 3*near+far+2

      "vpsrlw      $2,%%ymm0,%%ymm0              \n"  // 3/4*near+1/4*far
      "vpsrlw      $2,%%ymm2,%%ymm2              \n"  // 3/4*near+1/4*far
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm2,32(%1)                 \n"

      "lea         0x20(%0),%0                   \n"
      "lea         0x40(%1),%1                   \n"  // 16 sample to 32 sample
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),          // %0
        "+r"(dst_ptr),          // %1
        "+r"(dst_width)         // %2
      : "m"(kLinearShuffleFar)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif

#ifdef HAS_SCALEROWUP2_BILINEAR_12_AVX2
void ScaleRowUp2_Bilinear_12_AVX2(const uint16_t* src_ptr,
                                  ptrdiff_t src_stride,
                                  uint16_t* dst_ptr,
                                  ptrdiff_t dst_stride,
                                  int dst_width) {
  asm volatile(
      "vbroadcastf128 %5,%%ymm5                  \n"
      "vpcmpeqw    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpsrlw      $15,%%ymm4,%%ymm4             \n"
      "vpsllw      $3,%%ymm4,%%ymm4              \n"  // all 8

      LABELALIGN
      "1:          \n"

      "vmovdqu     (%0),%%xmm0                   \n"  // 01234567 (16b)
      "vmovdqu     2(%0),%%xmm1                  \n"  // 12345678 (16b)
      "vpermq      $0b11011000,%%ymm0,%%ymm0     \n"  // 0123000045670000
      "vpermq      $0b11011000,%%ymm1,%%ymm1     \n"  // 1234000056780000
      "vpunpcklwd  %%ymm1,%%ymm0,%%ymm0          \n"  // 0112233445566778 (near)
      "vpshufb     %%ymm5,%%ymm0,%%ymm1          \n"  // 1021324354657687 (far)
      "vpaddw      %%ymm0,%%ymm1,%%ymm1          \n"  // near+far
      "vpaddw      %%ymm0,%%ymm0,%%ymm0          \n"  // 2*near
      "vpaddw      %%ymm0,%%ymm1,%%ymm2          \n"  // 3*near+far (1)

      "vmovdqu     (%0,%3,2),%%xmm0              \n"  // 01234567 (16b)
      "vmovdqu     2(%0,%3,2),%%xmm1             \n"  // 12345678 (16b)
      "vpermq      $0b11011000,%%ymm0,%%ymm0     \n"  // 0123000045670000
      "vpermq      $0b11011000,%%ymm1,%%ymm1     \n"  // 1234000056780000
      "vpunpcklwd  %%ymm1,%%ymm0,%%ymm0          \n"  // 0112233445566778 (near)
      "vpshufb     %%ymm5,%%ymm0,%%ymm1          \n"  // 1021324354657687 (far)
      "vpaddw      %%ymm0,%%ymm1,%%ymm1          \n"  // near+far
      "vpaddw      %%ymm0,%%ymm0,%%ymm0          \n"  // 2*near
      "vpaddw      %%ymm0,%%ymm1,%%ymm3          \n"  // 3*near+far (2)

      "vpaddw      %%ymm2,%%ymm2,%%ymm0          \n"  // 6*near+2*far (1)
      "vpaddw      %%ymm4,%%ymm3,%%ymm1          \n"  // 3*near+far+8 (2)
      "vpaddw      %%ymm0,%%ymm2,%%ymm0          \n"  // 9*near+3*far (1)
      "vpaddw      %%ymm0,%%ymm1,%%ymm0          \n"  // 9 3 3 1 + 8 (1)
      "vpsrlw      $4,%%ymm0,%%ymm0              \n"  // ^ div by 16
      "vmovdqu     %%ymm0,(%1)                   \n"  // store above

      "vpaddw      %%ymm3,%%ymm3,%%ymm0          \n"  // 6*near+2*far (2)
      "vpaddw      %%ymm4,%%ymm2,%%ymm1          \n"  // 3*near+far+8 (1)
      "vpaddw      %%ymm0,%%ymm3,%%ymm0          \n"  // 9*near+3*far (2)
      "vpaddw      %%ymm0,%%ymm1,%%ymm0          \n"  // 9 3 3 1 + 8 (2)
      "vpsrlw      $4,%%ymm0,%%ymm0              \n"  // ^ div by 16
      "vmovdqu     %%ymm0,(%1,%4,2)              \n"  // store below

      "lea         0x10(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"  // 8 sample to 16 sample
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),          // %0
        "+r"(dst_ptr),          // %1
        "+r"(dst_width)         // %2
      : "r"(src_stride),        // %3
        "r"(dst_stride),        // %4
        "m"(kLinearShuffleFar)  // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif

#ifdef HAS_SCALEROWUP2_LINEAR_16_AVX2
void ScaleRowUp2_Linear_16_AVX2(const uint16_t* src_ptr,
                                uint16_t* dst_ptr,
                                int dst_width) {
  asm volatile(
      "vpcmpeqd    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpsrld      $31,%%ymm4,%%ymm4             \n"
      "vpslld      $1,%%ymm4,%%ymm4              \n"  // all 2

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%xmm0                   \n"  // 01234567 (16b, 1u1v)
      "vmovdqu     2(%0),%%xmm1                  \n"  // 12345678 (16b, 1u1v)

      "vpmovzxwd   %%xmm0,%%ymm0                 \n"  // 01234567 (32b, 1u1v)
      "vpmovzxwd   %%xmm1,%%ymm1                 \n"  // 12345678 (32b, 1u1v)

      "vpshufd     $0b10110001,%%ymm0,%%ymm2     \n"  // 10325476 (lo, far)
      "vpshufd     $0b10110001,%%ymm1,%%ymm3     \n"  // 21436587 (hi, far)

      "vpaddd      %%ymm4,%%ymm2,%%ymm2          \n"  // far+2 (lo)
      "vpaddd      %%ymm4,%%ymm3,%%ymm3          \n"  // far+2 (hi)
      "vpaddd      %%ymm0,%%ymm2,%%ymm2          \n"  // near+far+2 (lo)
      "vpaddd      %%ymm1,%%ymm3,%%ymm3          \n"  // near+far+2 (hi)
      "vpaddd      %%ymm0,%%ymm0,%%ymm0          \n"  // 2*near (lo)
      "vpaddd      %%ymm1,%%ymm1,%%ymm1          \n"  // 2*near (hi)
      "vpaddd      %%ymm0,%%ymm2,%%ymm0          \n"  // 3*near+far+2 (lo)
      "vpaddd      %%ymm1,%%ymm3,%%ymm1          \n"  // 3*near+far+2 (hi)

      "vpsrld      $2,%%ymm0,%%ymm0              \n"  // 3/4*near+1/4*far (lo)
      "vpsrld      $2,%%ymm1,%%ymm1              \n"  // 3/4*near+1/4*far (hi)
      "vpackusdw   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpshufd     $0b11011000,%%ymm0,%%ymm0     \n"
      "vmovdqu     %%ymm0,(%1)                   \n"

      "lea         0x10(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"  // 8 pixel to 16 pixel
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),   // %0
        "+r"(dst_ptr),   // %1
        "+r"(dst_width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}
#endif

#ifdef HAS_SCALEROWUP2_BILINEAR_16_AVX2
void ScaleRowUp2_Bilinear_16_AVX2(const uint16_t* src_ptr,
                                  ptrdiff_t src_stride,
                                  uint16_t* dst_ptr,
                                  ptrdiff_t dst_stride,
                                  int dst_width) {
  asm volatile(
      "vpcmpeqd    %%ymm6,%%ymm6,%%ymm6          \n"
      "vpsrld      $31,%%ymm6,%%ymm6             \n"
      "vpslld      $3,%%ymm6,%%ymm6              \n"  // all 8

      LABELALIGN
      "1:          \n"

      "vmovdqu     (%0),%%xmm0                   \n"  // 01234567 (16b, 1u1v)
      "vmovdqu     2(%0),%%xmm1                  \n"  // 12345678 (16b, 1u1v)
      "vpmovzxwd   %%xmm0,%%ymm0                 \n"  // 01234567 (32b, 1u1v)
      "vpmovzxwd   %%xmm1,%%ymm1                 \n"  // 12345678 (32b, 1u1v)
      "vpshufd     $0b10110001,%%ymm0,%%ymm2     \n"  // 10325476 (lo, far)
      "vpshufd     $0b10110001,%%ymm1,%%ymm3     \n"  // 21436587 (hi, far)
      "vpaddd      %%ymm0,%%ymm2,%%ymm2          \n"  // near+far (lo)
      "vpaddd      %%ymm1,%%ymm3,%%ymm3          \n"  // near+far (hi)
      "vpaddd      %%ymm0,%%ymm0,%%ymm0          \n"  // 2*near (lo)
      "vpaddd      %%ymm1,%%ymm1,%%ymm1          \n"  // 2*near (hi)
      "vpaddd      %%ymm0,%%ymm2,%%ymm0          \n"  // 3*near+far (1, lo)
      "vpaddd      %%ymm1,%%ymm3,%%ymm1          \n"  // 3*near+far (1, hi)

      "vmovdqu     (%0,%3,2),%%xmm2              \n"  // 01234567 (16b, 1u1v)
      "vmovdqu     2(%0,%3,2),%%xmm3             \n"  // 12345678 (16b, 1u1v)
      "vpmovzxwd   %%xmm2,%%ymm2                 \n"  // 01234567 (32b, 1u1v)
      "vpmovzxwd   %%xmm3,%%ymm3                 \n"  // 12345678 (32b, 1u1v)
      "vpshufd     $0b10110001,%%ymm2,%%ymm4     \n"  // 10325476 (lo, far)
      "vpshufd     $0b10110001,%%ymm3,%%ymm5     \n"  // 21436587 (hi, far)
      "vpaddd      %%ymm2,%%ymm4,%%ymm4          \n"  // near+far (lo)
      "vpaddd      %%ymm3,%%ymm5,%%ymm5          \n"  // near+far (hi)
      "vpaddd      %%ymm2,%%ymm2,%%ymm2          \n"  // 2*near (lo)
      "vpaddd      %%ymm3,%%ymm3,%%ymm3          \n"  // 2*near (hi)
      "vpaddd      %%ymm2,%%ymm4,%%ymm2          \n"  // 3*near+far (2, lo)
      "vpaddd      %%ymm3,%%ymm5,%%ymm3          \n"  // 3*near+far (2, hi)

      "vpaddd      %%ymm0,%%ymm0,%%ymm4          \n"  // 6*near+2*far (1, lo)
      "vpaddd      %%ymm6,%%ymm2,%%ymm5          \n"  // 3*near+far+8 (2, lo)
      "vpaddd      %%ymm4,%%ymm0,%%ymm4          \n"  // 9*near+3*far (1, lo)
      "vpaddd      %%ymm4,%%ymm5,%%ymm4          \n"  // 9 3 3 1 + 8 (1, lo)
      "vpsrld      $4,%%ymm4,%%ymm4              \n"  // ^ div by 16 (1, lo)

      "vpaddd      %%ymm2,%%ymm2,%%ymm5          \n"  // 6*near+2*far (2, lo)
      "vpaddd      %%ymm6,%%ymm0,%%ymm0          \n"  // 3*near+far+8 (1, lo)
      "vpaddd      %%ymm5,%%ymm2,%%ymm5          \n"  // 9*near+3*far (2, lo)
      "vpaddd      %%ymm5,%%ymm0,%%ymm5          \n"  // 9 3 3 1 + 8 (2, lo)
      "vpsrld      $4,%%ymm5,%%ymm5              \n"  // ^ div by 16 (2, lo)

      "vpaddd      %%ymm1,%%ymm1,%%ymm0          \n"  // 6*near+2*far (1, hi)
      "vpaddd      %%ymm6,%%ymm3,%%ymm2          \n"  // 3*near+far+8 (2, hi)
      "vpaddd      %%ymm0,%%ymm1,%%ymm0          \n"  // 9*near+3*far (1, hi)
      "vpaddd      %%ymm0,%%ymm2,%%ymm0          \n"  // 9 3 3 1 + 8 (1, hi)
      "vpsrld      $4,%%ymm0,%%ymm0              \n"  // ^ div by 16 (1, hi)

      "vpaddd      %%ymm3,%%ymm3,%%ymm2          \n"  // 6*near+2*far (2, hi)
      "vpaddd      %%ymm6,%%ymm1,%%ymm1          \n"  // 3*near+far+8 (1, hi)
      "vpaddd      %%ymm2,%%ymm3,%%ymm2          \n"  // 9*near+3*far (2, hi)
      "vpaddd      %%ymm2,%%ymm1,%%ymm2          \n"  // 9 3 3 1 + 8 (2, hi)
      "vpsrld      $4,%%ymm2,%%ymm2              \n"  // ^ div by 16 (2, hi)

      "vpackusdw   %%ymm0,%%ymm4,%%ymm4          \n"
      "vpshufd     $0b11011000,%%ymm4,%%ymm4     \n"
      "vmovdqu     %%ymm4,(%1)                   \n"  // store above
      "vpackusdw   %%ymm2,%%ymm5,%%ymm5          \n"
      "vpshufd     $0b11011000,%%ymm5,%%ymm5     \n"
      "vmovdqu     %%ymm5,(%1,%4,2)              \n"  // store below

      "lea         0x10(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"  // 8 pixel to 16 pixel
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),    // %0
        "+r"(dst_ptr),    // %1
        "+r"(dst_width)   // %2
      : "r"(src_stride),  // %3
        "r"(dst_stride)   // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}
#endif

// Reads 16xN bytes and produces 16 shorts at a time.


#ifdef HAS_SCALEADDROW_AVX2
// Reads 32 bytes and accumulates to 32 shorts at a time.
void ScaleAddRow_AVX2(const uint8_t* src_ptr,
                      uint16_t* dst_ptr,
                      int src_width) {
      asm volatile("vpxor       %%ymm5,%%ymm5,%%ymm5          \n"

               LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm3                   \n"
      "lea         0x20(%0),%0                   \n"  // src_ptr += 32
      "vpermq      $0xd8,%%ymm3,%%ymm3           \n"
      "vpunpcklbw  %%ymm5,%%ymm3,%%ymm2          \n"
      "vpunpckhbw  %%ymm5,%%ymm3,%%ymm3          \n"
      "vpaddusw    (%1),%%ymm2,%%ymm0            \n"
      "vpaddusw    0x20(%1),%%ymm3,%%ymm1        \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
               : "+r"(src_ptr),   // %0
                 "+r"(dst_ptr),   // %1
                 "+r"(src_width)  // %2
               :
               : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm5");
}
#endif  // HAS_SCALEADDROW_AVX2

// Constant for making pixels signed to avoid pmaddubsw
// saturation.
static const uvec8 kFsub80 = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                              0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};

// Constant for making pixels unsigned and adding .5 for rounding.
static const uvec16 kFadd40 = {0x4040, 0x4040, 0x4040, 0x4040,
                               0x4040, 0x4040, 0x4040, 0x4040};

// Bilinear column filtering. SSSE3 version.


// Reads 4 pixels, duplicates them and writes 8 pixels.
// Alignment requirement: src_argb 16 byte aligned, dst_argb 16 byte aligned.








// Reads 4 pixels at a time.
// Alignment requirement: dst_argb 16 byte aligned.


// Blends four 2x2 to 4x1.
// Alignment requirement: dst_argb 16 byte aligned.




// Reads 4 pixels, duplicates them and writes 8 pixels.
// Alignment requirement: src_argb 16 byte aligned, dst_argb 16 byte aligned.


// Shuffle table for arranging 2 pixels into pairs for pmaddubsw
static const uvec8 kShuffleColARGB = {
    0u, 4u,  1u, 5u,  2u,  6u,  3u,  7u,  // bbggrraa 1st pixel
    8u, 12u, 9u, 13u, 10u, 14u, 11u, 15u  // bbggrraa 2nd pixel
};

// Shuffle table for duplicating 2 fractions into 8 bytes each
static const uvec8 kShuffleFractions = {
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u,
};

// Bilinear row filtering combines 4x2 -> 4x1. SSSE3 version


// Divide num by div and return as 16.16 fixed point result.
int FixedDiv_X86(int num, int div) {
  asm volatile(
      "cdq         \n"
      "shld        $0x10,%%eax,%%edx             \n"
      "shl         $0x10,%%eax                   \n"
      "idiv        %1                            \n"
      "mov         %0, %%eax                     \n"
      : "+a"(num)  // %0
      : "c"(div)   // %1
      : "memory", "cc", "edx");
  return num;
}

// Divide num - 1 by div - 1 and return as 16.16 fixed point result.
int FixedDiv1_X86(int num, int div) {
  asm volatile(
      "cdq         \n"
      "shld        $0x10,%%eax,%%edx             \n"
      "shl         $0x10,%%eax                   \n"
      "sub         $0x10001,%%eax                \n"
      "sbb         $0x0,%%edx                    \n"
      "sub         $0x1,%1                       \n"
      "idiv        %1                            \n"
      "mov         %0, %%eax                     \n"
      : "+a"(num)  // %0
      : "c"(div)   // %1
      : "memory", "cc", "edx");
  return num;
}

#if 1 || \
    defined(HAS_SCALEUVROWDOWN2BOX_AVX2)

// Shuffle table for splitting UV into upper and lower part of register.
static const uvec8 kShuffleSplitUV = {0u, 2u, 4u, 6u, 8u, 10u, 12u, 14u,
                                      1u, 3u, 5u, 7u, 9u, 11u, 13u, 15u};
static const uvec8 kShuffleMergeUV = {0u,   8u,   2u,   10u,  4u,   12u,
                                      6u,   14u,  0x80, 0x80, 0x80, 0x80,
                                      0x80, 0x80, 0x80, 0x80};
#endif


#ifdef HAS_SCALEUVROWDOWN2BOX_AVX2
void ScaleUVRowDown2Box_AVX2(const uint8_t* src_ptr,
                             ptrdiff_t src_stride,
                             uint8_t* dst_ptr,
                             int dst_width) {
  asm volatile(
      "vpcmpeqb    %%ymm4,%%ymm4,%%ymm4          \n"  // 01010101
      "vpabsb      %%ymm4,%%ymm4                 \n"
      "vpxor       %%ymm5,%%ymm5,%%ymm5          \n"  // zero
      "vbroadcastf128 %4,%%ymm1                  \n"  // split shuffler
      "vbroadcastf128 %5,%%ymm3                  \n"  // merge shuffler

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%ymm0                   \n"  // 16 UV row 0
      "vmovdqu     0x00(%0,%3,1),%%ymm2          \n"  // 16 UV row 1
      "lea         0x20(%0),%0                   \n"
      "vpshufb     %%ymm1,%%ymm0,%%ymm0          \n"  // uuuuvvvv
      "vpshufb     %%ymm1,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm4,%%ymm0,%%ymm0          \n"  // horizontal add
      "vpmaddubsw  %%ymm4,%%ymm2,%%ymm2          \n"
      "vpaddw      %%ymm2,%%ymm0,%%ymm0          \n"  // vertical add
      "vpsrlw      $0x1,%%ymm0,%%ymm0            \n"  // round
      "vpavgw      %%ymm5,%%ymm0,%%ymm0          \n"
      "vpshufb     %%ymm3,%%ymm0,%%ymm0          \n"  // merge uv
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"  // combine qwords
      "vmovdqu     %%xmm0,(%1)                   \n"
      "lea         0x10(%1),%1                   \n"  // 8 UV
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),         // %0
        "+r"(dst_ptr),         // %1
        "+r"(dst_width)        // %2
      : "r"(src_stride),       // %3
        "m"(kShuffleSplitUV),  // %4
        "m"(kShuffleMergeUV)   // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_SCALEUVROWDOWN2BOX_AVX2

static const uvec8 kUVLinearMadd31 = {3, 1, 3, 1, 1, 3, 1, 3,
                                      3, 1, 3, 1, 1, 3, 1, 3};



#ifdef HAS_SCALEUVROWUP2_LINEAR_AVX2

void ScaleUVRowUp2_Linear_AVX2(const uint8_t* src_ptr,
                               uint8_t* dst_ptr,
                               int dst_width) {
  asm volatile(
      "vpcmpeqw    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpsrlw      $15,%%ymm4,%%ymm4             \n"
      "vpsllw      $1,%%ymm4,%%ymm4              \n"  // all 2
      "vbroadcastf128 %3,%%ymm3                  \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%xmm0                   \n"
      "vmovdqu     2(%0),%%xmm1                  \n"
      "vpermq      $0b11011000,%%ymm0,%%ymm0     \n"
      "vpermq      $0b11011000,%%ymm1,%%ymm1     \n"
      "vpunpcklbw  %%ymm1,%%ymm0,%%ymm0          \n"
      "vpunpckhdq  %%ymm0,%%ymm0,%%ymm2          \n"
      "vpunpckldq  %%ymm0,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm3,%%ymm2,%%ymm1          \n"  // 3*near+far (hi)
      "vpmaddubsw  %%ymm3,%%ymm0,%%ymm0          \n"  // 3*near+far (lo)
      "vpaddw      %%ymm4,%%ymm0,%%ymm0          \n"  // 3*near+far+2 (lo)
      "vpaddw      %%ymm4,%%ymm1,%%ymm1          \n"  // 3*near+far+2 (hi)
      "vpsrlw      $2,%%ymm0,%%ymm0              \n"  // 3/4*near+1/4*far (lo)
      "vpsrlw      $2,%%ymm1,%%ymm1              \n"  // 3/4*near+1/4*far (hi)
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"

      "lea         0x10(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"  // 8 uv to 16 uv
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),        // %0
        "+r"(dst_ptr),        // %1
        "+r"(dst_width)       // %2
      : "m"(kUVLinearMadd31)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}
#endif

#ifdef HAS_SCALEUVROWUP2_BILINEAR_AVX2
void ScaleUVRowUp2_Bilinear_AVX2(const uint8_t* src_ptr,
                                 ptrdiff_t src_stride,
                                 uint8_t* dst_ptr,
                                 ptrdiff_t dst_stride,
                                 int dst_width) {
  asm volatile(
      "vpcmpeqw    %%ymm6,%%ymm6,%%ymm6          \n"
      "vpsrlw      $15,%%ymm6,%%ymm6             \n"
      "vpsllw      $3,%%ymm6,%%ymm6              \n"  // all 8
      "vbroadcastf128 %5,%%ymm7                  \n"

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%xmm0                   \n"
      "vmovdqu     2(%0),%%xmm1                  \n"
      "vpermq      $0b11011000,%%ymm0,%%ymm0     \n"
      "vpermq      $0b11011000,%%ymm1,%%ymm1     \n"
      "vpunpcklbw  %%ymm1,%%ymm0,%%ymm0          \n"
      "vpunpckhdq  %%ymm0,%%ymm0,%%ymm2          \n"
      "vpunpckldq  %%ymm0,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm7,%%ymm2,%%ymm1          \n"  // 3*near+far (1, hi)
      "vpmaddubsw  %%ymm7,%%ymm0,%%ymm0          \n"  // 3*near+far (1, lo)

      "vmovdqu     (%0,%3),%%xmm2                \n"  // 0123456789ABCDEF
      "vmovdqu     2(%0,%3),%%xmm3               \n"  // 123456789ABCDEF0
      "vpermq      $0b11011000,%%ymm2,%%ymm2     \n"
      "vpermq      $0b11011000,%%ymm3,%%ymm3     \n"
      "vpunpcklbw  %%ymm3,%%ymm2,%%ymm2          \n"
      "vpunpckhdq  %%ymm2,%%ymm2,%%ymm4          \n"
      "vpunpckldq  %%ymm2,%%ymm2,%%ymm2          \n"
      "vpmaddubsw  %%ymm7,%%ymm4,%%ymm3          \n"  // 3*near+far (2, hi)
      "vpmaddubsw  %%ymm7,%%ymm2,%%ymm2          \n"  // 3*near+far (2, lo)

      // ymm0 ymm1
      // ymm2 ymm3

      "vpaddw      %%ymm0,%%ymm0,%%ymm4          \n"  // 6*near+2*far (1, lo)
      "vpaddw      %%ymm6,%%ymm2,%%ymm5          \n"  // 3*near+far+8 (2, lo)
      "vpaddw      %%ymm4,%%ymm0,%%ymm4          \n"  // 9*near+3*far (1, lo)
      "vpaddw      %%ymm4,%%ymm5,%%ymm4          \n"  // 9 3 3 1 + 8 (1, lo)
      "vpsrlw      $4,%%ymm4,%%ymm4              \n"  // ^ div by 16 (1, lo)

      "vpaddw      %%ymm2,%%ymm2,%%ymm5          \n"  // 6*near+2*far (2, lo)
      "vpaddw      %%ymm6,%%ymm0,%%ymm0          \n"  // 3*near+far+8 (1, lo)
      "vpaddw      %%ymm5,%%ymm2,%%ymm5          \n"  // 9*near+3*far (2, lo)
      "vpaddw      %%ymm5,%%ymm0,%%ymm5          \n"  // 9 3 3 1 + 8 (2, lo)
      "vpsrlw      $4,%%ymm5,%%ymm5              \n"  // ^ div by 16 (2, lo)

      "vpaddw      %%ymm1,%%ymm1,%%ymm0          \n"  // 6*near+2*far (1, hi)
      "vpaddw      %%ymm6,%%ymm3,%%ymm2          \n"  // 3*near+far+8 (2, hi)
      "vpaddw      %%ymm0,%%ymm1,%%ymm0          \n"  // 9*near+3*far (1, hi)
      "vpaddw      %%ymm0,%%ymm2,%%ymm0          \n"  // 9 3 3 1 + 8 (1, hi)
      "vpsrlw      $4,%%ymm0,%%ymm0              \n"  // ^ div by 16 (1, hi)

      "vpaddw      %%ymm3,%%ymm3,%%ymm2          \n"  // 6*near+2*far (2, hi)
      "vpaddw      %%ymm6,%%ymm1,%%ymm1          \n"  // 3*near+far+8 (1, hi)
      "vpaddw      %%ymm2,%%ymm3,%%ymm2          \n"  // 9*near+3*far (2, hi)
      "vpaddw      %%ymm2,%%ymm1,%%ymm2          \n"  // 9 3 3 1 + 8 (2, hi)
      "vpsrlw      $4,%%ymm2,%%ymm2              \n"  // ^ div by 16 (2, hi)

      "vpackuswb   %%ymm0,%%ymm4,%%ymm4          \n"
      "vmovdqu     %%ymm4,(%1)                   \n"  // store above
      "vpackuswb   %%ymm2,%%ymm5,%%ymm5          \n"
      "vmovdqu     %%ymm5,(%1,%4)                \n"  // store below

      "lea         0x10(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"  // 8 uv to 16 uv
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),        // %0
        "+r"(dst_ptr),        // %1
        "+r"(dst_width)       // %2
      : "r"(src_stride),      // %3
        "r"(dst_stride),      // %4
        "m"(kUVLinearMadd31)  // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif



#ifdef HAS_SCALEUVROWUP2_LINEAR_16_AVX2
void ScaleUVRowUp2_Linear_16_AVX2(const uint16_t* src_ptr,
                                  uint16_t* dst_ptr,
                                  int dst_width) {
  asm volatile(
      "vpcmpeqd    %%ymm4,%%ymm4,%%ymm4          \n"
      "vpsrld      $31,%%ymm4,%%ymm4             \n"
      "vpslld      $1,%%ymm4,%%ymm4              \n"  // all 2

      LABELALIGN
      "1:          \n"
      "vmovdqu     (%0),%%xmm0                   \n"  // 00112233 (16b, 1u1v)
      "vmovdqu     4(%0),%%xmm1                  \n"  // 11223344 (16b, 1u1v)

      "vpmovzxwd   %%xmm0,%%ymm0                 \n"  // 01234567 (32b, 1u1v)
      "vpmovzxwd   %%xmm1,%%ymm1                 \n"  // 12345678 (32b, 1u1v)

      "vpshufd     $0b01001110,%%ymm0,%%ymm2     \n"  // 11003322 (lo, far)
      "vpshufd     $0b01001110,%%ymm1,%%ymm3     \n"  // 22114433 (hi, far)

      "vpaddd      %%ymm4,%%ymm2,%%ymm2          \n"  // far+2 (lo)
      "vpaddd      %%ymm4,%%ymm3,%%ymm3          \n"  // far+2 (hi)
      "vpaddd      %%ymm0,%%ymm2,%%ymm2          \n"  // near+far+2 (lo)
      "vpaddd      %%ymm1,%%ymm3,%%ymm3          \n"  // near+far+2 (hi)
      "vpaddd      %%ymm0,%%ymm0,%%ymm0          \n"  // 2*near (lo)
      "vpaddd      %%ymm1,%%ymm1,%%ymm1          \n"  // 2*near (hi)
      "vpaddd      %%ymm0,%%ymm2,%%ymm0          \n"  // 3*near+far+2 (lo)
      "vpaddd      %%ymm1,%%ymm3,%%ymm1          \n"  // 3*near+far+2 (hi)

      "vpsrld      $2,%%ymm0,%%ymm0              \n"  // 3/4*near+1/4*far (lo)
      "vpsrld      $2,%%ymm1,%%ymm1              \n"  // 3/4*near+1/4*far (hi)
      "vpackusdw   %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"

      "lea         0x10(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"  // 4 uv to 8 uv
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),   // %0
        "+r"(dst_ptr),   // %1
        "+r"(dst_width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}
#endif

#ifdef HAS_SCALEUVROWUP2_BILINEAR_16_AVX2
void ScaleUVRowUp2_Bilinear_16_AVX2(const uint16_t* src_ptr,
                                    ptrdiff_t src_stride,
                                    uint16_t* dst_ptr,
                                    ptrdiff_t dst_stride,
                                    int dst_width) {
  asm volatile(
      "vpcmpeqd    %%ymm6,%%ymm6,%%ymm6          \n"
      "vpsrld      $31,%%ymm6,%%ymm6             \n"
      "vpslld      $3,%%ymm6,%%ymm6              \n"  // all 8

      LABELALIGN
      "1:          \n"

      "vmovdqu     (%0),%%xmm0                   \n"  // 00112233 (16b, 1u1v)
      "vmovdqu     4(%0),%%xmm1                  \n"  // 11223344 (16b, 1u1v)
      "vpmovzxwd   %%xmm0,%%ymm0                 \n"  // 01234567 (32b, 1u1v)
      "vpmovzxwd   %%xmm1,%%ymm1                 \n"  // 12345678 (32b, 1u1v)
      "vpshufd     $0b01001110,%%ymm0,%%ymm2     \n"  // 11003322 (lo, far)
      "vpshufd     $0b01001110,%%ymm1,%%ymm3     \n"  // 22114433 (hi, far)
      "vpaddd      %%ymm0,%%ymm2,%%ymm2          \n"  // near+far (lo)
      "vpaddd      %%ymm1,%%ymm3,%%ymm3          \n"  // near+far (hi)
      "vpaddd      %%ymm0,%%ymm0,%%ymm0          \n"  // 2*near (lo)
      "vpaddd      %%ymm1,%%ymm1,%%ymm1          \n"  // 2*near (hi)
      "vpaddd      %%ymm0,%%ymm2,%%ymm0          \n"  // 3*near+far (lo)
      "vpaddd      %%ymm1,%%ymm3,%%ymm1          \n"  // 3*near+far (hi)

      "vmovdqu     (%0,%3,2),%%xmm2              \n"  // 00112233 (16b, 1u1v)
      "vmovdqu     4(%0,%3,2),%%xmm3             \n"  // 11223344 (16b, 1u1v)
      "vpmovzxwd   %%xmm2,%%ymm2                 \n"  // 01234567 (32b, 1u1v)
      "vpmovzxwd   %%xmm3,%%ymm3                 \n"  // 12345678 (32b, 1u1v)
      "vpshufd     $0b01001110,%%ymm2,%%ymm4     \n"  // 11003322 (lo, far)
      "vpshufd     $0b01001110,%%ymm3,%%ymm5     \n"  // 22114433 (hi, far)
      "vpaddd      %%ymm2,%%ymm4,%%ymm4          \n"  // near+far (lo)
      "vpaddd      %%ymm3,%%ymm5,%%ymm5          \n"  // near+far (hi)
      "vpaddd      %%ymm2,%%ymm2,%%ymm2          \n"  // 2*near (lo)
      "vpaddd      %%ymm3,%%ymm3,%%ymm3          \n"  // 2*near (hi)
      "vpaddd      %%ymm2,%%ymm4,%%ymm2          \n"  // 3*near+far (lo)
      "vpaddd      %%ymm3,%%ymm5,%%ymm3          \n"  // 3*near+far (hi)

      "vpaddd      %%ymm0,%%ymm0,%%ymm4          \n"  // 6*near+2*far (1, lo)
      "vpaddd      %%ymm6,%%ymm2,%%ymm5          \n"  // 3*near+far+8 (2, lo)
      "vpaddd      %%ymm4,%%ymm0,%%ymm4          \n"  // 9*near+3*far (1, lo)
      "vpaddd      %%ymm4,%%ymm5,%%ymm4          \n"  // 9 3 3 1 + 8 (1, lo)
      "vpsrld      $4,%%ymm4,%%ymm4              \n"  // ^ div by 16 (1, lo)

      "vpaddd      %%ymm2,%%ymm2,%%ymm5          \n"  // 6*near+2*far (2, lo)
      "vpaddd      %%ymm6,%%ymm0,%%ymm0          \n"  // 3*near+far+8 (1, lo)
      "vpaddd      %%ymm5,%%ymm2,%%ymm5          \n"  // 9*near+3*far (2, lo)
      "vpaddd      %%ymm5,%%ymm0,%%ymm5          \n"  // 9 3 3 1 + 8 (2, lo)
      "vpsrld      $4,%%ymm5,%%ymm5              \n"  // ^ div by 16 (2, lo)

      "vpaddd      %%ymm1,%%ymm1,%%ymm0          \n"  // 6*near+2*far (1, hi)
      "vpaddd      %%ymm6,%%ymm3,%%ymm2          \n"  // 3*near+far+8 (2, hi)
      "vpaddd      %%ymm0,%%ymm1,%%ymm0          \n"  // 9*near+3*far (1, hi)
      "vpaddd      %%ymm0,%%ymm2,%%ymm0          \n"  // 9 3 3 1 + 8 (1, hi)
      "vpsrld      $4,%%ymm0,%%ymm0              \n"  // ^ div by 16 (1, hi)

      "vpaddd      %%ymm3,%%ymm3,%%ymm2          \n"  // 6*near+2*far (2, hi)
      "vpaddd      %%ymm6,%%ymm1,%%ymm1          \n"  // 3*near+far+8 (1, hi)
      "vpaddd      %%ymm2,%%ymm3,%%ymm2          \n"  // 9*near+3*far (2, hi)
      "vpaddd      %%ymm2,%%ymm1,%%ymm2          \n"  // 9 3 3 1 + 8 (2, hi)
      "vpsrld      $4,%%ymm2,%%ymm2              \n"  // ^ div by 16 (2, hi)

      "vpackusdw   %%ymm0,%%ymm4,%%ymm4          \n"
      "vmovdqu     %%ymm4,(%1)                   \n"  // store above
      "vpackusdw   %%ymm2,%%ymm5,%%ymm5          \n"
      "vmovdqu     %%ymm5,(%1,%4,2)              \n"  // store below

      "lea         0x10(%0),%0                   \n"
      "lea         0x20(%1),%1                   \n"  // 4 uv to 8 uv
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src_ptr),    // %0
        "+r"(dst_ptr),    // %1
        "+r"(dst_width)   // %2
      : "r"(src_stride),  // %3
        "r"(dst_stride)   // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}
#endif

#endif  // defined(__x86_64__) || defined(__i386__)

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
