/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/basic_types.h"

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
#else
#endif

static const vec8 kNibbleMask = {15, 15, 15, 15, 15, 15, 15, 15,
                                 15, 15, 15, 15, 15, 15, 15, 15};
static const vec8 kBitCount = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};


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
      "vmovdqa     (%0),%%ymm4                   \n"
      "vmovdqa     0x20(%0), %%ymm5              \n"
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

#endif  // defined(__x86_64__) || (defined(__i386__) && !defined(__pic__)))

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
