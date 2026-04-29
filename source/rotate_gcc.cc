/*
 *  Copyright 2015 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/rotate_row.h"
#include "libyuv/row.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// This module is for GCC x86 and x64.
#if !defined(LIBYUV_DISABLE_X86) &&               \
    (defined(__x86_64__) || defined(__i386__)) && \
    !defined(LIBYUV_ENABLE_ROWWIN)

// Transpose 8x8. 32 or 64 bit, but not NaCL for 64 bit.

// Transpose 16x8. 64 bit

// Transpose UV 8x8.  64 bit.


#if defined(HAS_TRANSPOSE4X4_32_AVX2)

// Transpose 32 bit values (ARGB)
void Transpose4x4_32_AVX2(const uint8_t* src,
                          int src_stride,
                          uint8_t* dst,
                          int dst_stride,
                          int width) {
  asm volatile(
      // Main loop transpose 2 blocks of 4x4.  Read a column, write a row.
      "1:          \n"
      "vmovdqu     (%0),%%xmm0                   \n"  // a b c d
      "vmovdqu     (%0,%3),%%xmm1                \n"  // e f g h
      "lea         (%0,%3,2),%0                  \n"  // src += stride * 2
      "vmovdqu     (%0),%%xmm2                   \n"  // i j k l
      "vmovdqu     (%0,%3),%%xmm3                \n"  // m n o p
      "lea         (%0,%3,2),%0                  \n"  // src += stride * 2

      "vinserti128 $1,(%0),%%ymm0,%%ymm0         \n"  // a b c d
      "vinserti128 $1,(%0,%3),%%ymm1,%%ymm1      \n"  // e f g h
      "lea         (%0,%3,2),%0                  \n"  // src += stride * 2
      "vinserti128 $1,(%0),%%ymm2,%%ymm2         \n"  // i j k l
      "vinserti128 $1,(%0,%3),%%ymm3,%%ymm3      \n"  // m n o p
      "lea         (%0,%3,2),%0                  \n"  // src += stride * 2

      // Transpose 2x2
      "vpunpckldq  %%ymm1,%%ymm0,%%ymm4          \n"  // a e b f   from row 0, 1
      "vpunpckldq  %%ymm3,%%ymm2,%%ymm5          \n"  // i m j n   from row 2, 3
      "vpunpckhdq  %%ymm1,%%ymm0,%%ymm6          \n"  // c g d h   from row 0, 1
      "vpunpckhdq  %%ymm3,%%ymm2,%%ymm7          \n"  // k o l p   from row 2, 3

      // Transpose 4x4
      "vpunpcklqdq %%ymm5,%%ymm4,%%ymm0          \n"  // a e i m   from row 0, 1
      "vpunpckhqdq %%ymm5,%%ymm4,%%ymm1          \n"  // b f j n   from row 0, 1
      "vpunpcklqdq %%ymm7,%%ymm6,%%ymm2          \n"  // c g k o   from row 2, 3
      "vpunpckhqdq %%ymm7,%%ymm6,%%ymm3          \n"  // d h l p   from row 2, 3

      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         32(%1,%4),%1                  \n"  // dst += stride + 32
      "vmovdqu     %%ymm1,-32(%1)                \n"
      "vmovdqu     %%ymm2,-32(%1,%4)             \n"
      "vmovdqu     %%ymm3,-32(%1,%4,2)           \n"
      "sub         %4,%1                         \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper  \n"
      : "+r"(src),                     // %0
        "+r"(dst),                     // %1
        "+rm"(width)                   // %2
      : "r"((ptrdiff_t)(src_stride)),  // %3
        "r"((ptrdiff_t)(dst_stride))   // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // defined(HAS_TRANSPOSE4X4_32_AVX2)

#endif  // defined(__x86_64__) || defined(__i386__)

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
