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

// This module is for 32 bit Visual C x86
#if !defined(LIBYUV_DISABLE_X86) && defined(_MSC_VER) && defined(_M_IX86) && \
    (!defined(__clang__) || defined(LIBYUV_ENABLE_ROWWIN))

// Offsets for source bytes 0 to 9
SIMD_ALIGNED(const uint8_t) kShuf0[32] = {0,   1,   3,   4,   5,   7,   8,   9,
                             128, 128, 128, 128, 128, 128, 128, 128, 0,   1,   3,   4,   5,   7,   8,   9,
                             128, 128, 128, 128, 128, 128, 128, 128};

// Offsets for source bytes 11 to 20 with 8 subtracted = 3 to 12.
SIMD_ALIGNED(const uint8_t) kShuf1[32] = {3,   4,   5,   7,   8,   9,   11,  12,
                             128, 128, 128, 128, 128, 128, 128, 128, 3,   4,   5,   7,   8,   9,   11,  12,
                             128, 128, 128, 128, 128, 128, 128, 128};

// Offsets for source bytes 21 to 31 with 16 subtracted = 5 to 31.
SIMD_ALIGNED(const uint8_t) kShuf2[32] = {5,   7,   8,   9,   11,  12,  13,  15,
                             128, 128, 128, 128, 128, 128, 128, 128, 5,   7,   8,   9,   11,  12,  13,  15,
                             128, 128, 128, 128, 128, 128, 128, 128};

// Offsets for source bytes 0 to 10
SIMD_ALIGNED(const uint8_t) kShuf01[32] = {0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9, 9, 10, 0, 1, 1, 2, 2, 3, 4, 5, 5, 6, 6, 7, 8, 9, 9, 10};

// Offsets for source bytes 10 to 21 with 8 subtracted = 3 to 13.
SIMD_ALIGNED(const uint8_t) kShuf11[32] = {2, 3, 4, 5,  5,  6,  6,  7,
                              8, 9, 9, 10, 10, 11, 12, 13, 2, 3, 4, 5,  5,  6,  6,  7,
                              8, 9, 9, 10, 10, 11, 12, 13};

// Offsets for source bytes 21 to 31 with 16 subtracted = 5 to 31.
SIMD_ALIGNED(const uint8_t) kShuf21[32] = {5,  6,  6,  7,  8,  9,  9,  10,
                              10, 11, 12, 13, 13, 14, 14, 15, 5,  6,  6,  7,  8,  9,  9,  10,
                              10, 11, 12, 13, 13, 14, 14, 15};

// Coefficients for source bytes 0 to 10
SIMD_ALIGNED(const uint8_t) kMadd01[32] = {3, 1, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 3, 1, 2, 2, 3, 1, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 3, 1, 2, 2};

// Coefficients for source bytes 10 to 21
SIMD_ALIGNED(const uint8_t) kMadd11[32] = {1, 3, 3, 1, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 3, 1, 1, 3, 3, 1, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 3, 1};

// Coefficients for source bytes 21 to 31
SIMD_ALIGNED(const uint8_t) kMadd21[32] = {2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3, 3, 1, 2, 2, 1, 3};

// Coefficients for source bytes 21 to 31
SIMD_ALIGNED(const int16_t) kRound34[16] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

SIMD_ALIGNED(const uint8_t) kShuf38a[32] = {0,   3,   6,   8,   11,  14,  128, 128,
                               128, 128, 128, 128, 128, 128, 128, 128, 0,   3,   6,   8,   11,  14,  128, 128,
                               128, 128, 128, 128, 128, 128, 128, 128};

SIMD_ALIGNED(const uint8_t) kShuf38b[32] = {128, 128, 128, 128, 128, 128, 0,   3,
                               6,   8,   11,  14,  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 0,   3,
                               6,   8,   11,  14,  128, 128, 128, 128};

// Arrange words 0,3,6 into 0,1,2
SIMD_ALIGNED(const uint8_t) kShufAc[32] = {0,   1,   6,   7,   12,  13,  128, 128,
                              128, 128, 128, 128, 128, 128, 128, 128, 0,   1,   6,   7,   12,  13,  128, 128,
                              128, 128, 128, 128, 128, 128, 128, 128};

// Arrange words 0,3,6 into 3,4,5
SIMD_ALIGNED(const uint8_t) kShufAc3[32] = {128, 128, 128, 128, 128, 128, 0,   1,
                               6,   7,   12,  13,  128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 0,   1,
                               6,   7,   12,  13,  128, 128, 128, 128};

// Scaling values for boxes of 3x3 and 2x3
SIMD_ALIGNED(const uint16_t) kScaleAc33[16] = {65536 / 9, 65536 / 9, 65536 / 6, 65536 / 9,
                                  65536 / 9, 65536 / 6, 0,         0, 65536 / 9, 65536 / 9, 65536 / 6, 65536 / 9,
                                  65536 / 9, 65536 / 6, 0,         0};

// Arrange first value for pixels 0,1,2,3,4,5
SIMD_ALIGNED(const uint8_t) kShufAb0[32] = {0,  128, 3,  128, 6,   128, 8,   128,
                               11, 128, 14, 128, 128, 128, 128, 128, 0,  128, 3,  128, 6,   128, 8,   128,
                               11, 128, 14, 128, 128, 128, 128, 128};

// Arrange second value for pixels 0,1,2,3,4,5
SIMD_ALIGNED(const uint8_t) kShufAb1[32] = {1,  128, 4,  128, 7,   128, 9,   128,
                               12, 128, 15, 128, 128, 128, 128, 128, 1,  128, 4,  128, 7,   128, 9,   128,
                               12, 128, 15, 128, 128, 128, 128, 128};

// Arrange third value for pixels 0,1,2,3,4,5
SIMD_ALIGNED(const uint8_t) kShufAb2[32] = {2,  128, 5,   128, 128, 128, 10,  128,
                               13, 128, 128, 128, 128, 128, 128, 128, 2,  128, 5,   128, 128, 128, 10,  128,
                               13, 128, 128, 128, 128, 128, 128, 128};

// Scaling values for boxes of 3x2 and 2x2
SIMD_ALIGNED(const uint16_t) kScaleAb2[16] = {65536 / 3, 65536 / 3, 65536 / 2, 65536 / 3,
                                 65536 / 3, 65536 / 2, 0,         0, 65536 / 3, 65536 / 3, 65536 / 2, 65536 / 3,
                                 65536 / 3, 65536 / 2, 0,         0};

// Reads 32 pixels, throws half away and writes 16 pixels.


// Blends 32x1 rectangle to 16x1.


// Blends 32x2 rectangle to 16x1.


#ifdef HAS_SCALEROWDOWN2_AVX2
// Reads 64 pixels, throws half away and writes 32 pixels.
__declspec(naked) void ScaleRowDown2_AVX2(const uint8_t* src_ptr,
                                          ptrdiff_t src_stride,
                                          uint8_t* dst_ptr,
                                          int dst_width) {
  __asm {
    mov        eax, [esp + 4]  // src_ptr
    // src_stride ignored
    mov        edx, [esp + 12]  // dst_ptr
    mov        ecx, [esp + 16]  // dst_width

  wloop:
    vmovdqu     ymm0, [eax]
    vmovdqu     ymm1, [eax + 32]
    lea         eax,  [eax + 64]
    vpsrlw      ymm0, ymm0, 8  // isolate odd pixels.
    vpsrlw      ymm1, ymm1, 8
    vpackuswb   ymm0, ymm0, ymm1
    vpermq      ymm0, ymm0, 0xd8       // unmutate vpackuswb
    vmovdqu     [edx], ymm0
    lea         edx, [edx + 32]
    sub         ecx, 32
    jg          wloop

    vzeroupper
    ret
  }
}

// Blends 64x1 rectangle to 32x1.
__declspec(naked) void ScaleRowDown2Linear_AVX2(const uint8_t* src_ptr,
                                                ptrdiff_t src_stride,
                                                uint8_t* dst_ptr,
                                                int dst_width) {
  __asm {
    mov         eax, [esp + 4]  // src_ptr
    // src_stride
    mov         edx, [esp + 12]  // dst_ptr
    mov         ecx, [esp + 16]  // dst_width

    vpcmpeqb    ymm4, ymm4, ymm4  // '1' constant, 8b
    vpsrlw      ymm4, ymm4, 15
    vpackuswb   ymm4, ymm4, ymm4
    vpxor       ymm5, ymm5, ymm5  // constant 0

  wloop:
    vmovdqu     ymm0, [eax]
    vmovdqu     ymm1, [eax + 32]
    lea         eax,  [eax + 64]
    vpmaddubsw  ymm0, ymm0, ymm4  // horizontal add
    vpmaddubsw  ymm1, ymm1, ymm4
    vpavgw      ymm0, ymm0, ymm5  // (x + 1) / 2
    vpavgw      ymm1, ymm1, ymm5
    vpackuswb   ymm0, ymm0, ymm1
    vpermq      ymm0, ymm0, 0xd8       // unmutate vpackuswb
    vmovdqu     [edx], ymm0
    lea         edx, [edx + 32]
    sub         ecx, 32
    jg          wloop

    vzeroupper
    ret
  }
}

// For rounding, average = (sum + 2) / 4
// becomes average((sum >> 1), 0)
// Blends 64x2 rectangle to 32x1.
__declspec(naked) void ScaleRowDown2Box_AVX2(const uint8_t* src_ptr,
                                             ptrdiff_t src_stride,
                                             uint8_t* dst_ptr,
                                             int dst_width) {
  __asm {
    push        esi
    mov         eax, [esp + 4 + 4]  // src_ptr
    mov         esi, [esp + 4 + 8]  // src_stride
    mov         edx, [esp + 4 + 12]  // dst_ptr
    mov         ecx, [esp + 4 + 16]  // dst_width

    vpcmpeqb    ymm4, ymm4, ymm4  // '1' constant, 8b
    vpsrlw      ymm4, ymm4, 15
    vpackuswb   ymm4, ymm4, ymm4
    vpxor       ymm5, ymm5, ymm5  // constant 0

  wloop:
    vmovdqu     ymm0, [eax]
    vmovdqu     ymm1, [eax + 32]
    vmovdqu     ymm2, [eax + esi]
    vmovdqu     ymm3, [eax + esi + 32]
    lea         eax,  [eax + 64]
    vpmaddubsw  ymm0, ymm0, ymm4  // horizontal add
    vpmaddubsw  ymm1, ymm1, ymm4
    vpmaddubsw  ymm2, ymm2, ymm4
    vpmaddubsw  ymm3, ymm3, ymm4
    vpaddw      ymm0, ymm0, ymm2  // vertical add
    vpaddw      ymm1, ymm1, ymm3
    vpsrlw      ymm0, ymm0, 1  // (x + 2) / 4 = (x / 2 + 1) / 2
    vpsrlw      ymm1, ymm1, 1
    vpavgw      ymm0, ymm0, ymm5  // (x + 1) / 2
    vpavgw      ymm1, ymm1, ymm5
    vpackuswb   ymm0, ymm0, ymm1
    vpermq      ymm0, ymm0, 0xd8  // unmutate vpackuswb
    vmovdqu     [edx], ymm0
    lea         edx, [edx + 32]
    sub         ecx, 32
    jg          wloop

    pop         esi
    vzeroupper
    ret
  }
}
#endif  // HAS_SCALEROWDOWN2_AVX2

// Point samples 32 pixels to 8 pixels.


// Blends 32x4 rectangle to 8x1.


#ifdef HAS_SCALEROWDOWN4_AVX2
// Point samples 64 pixels to 16 pixels.
__declspec(naked) void ScaleRowDown4_AVX2(const uint8_t* src_ptr,
                                          ptrdiff_t src_stride,
                                          uint8_t* dst_ptr,
                                          int dst_width) {
  __asm {
    mov         eax, [esp + 4]  // src_ptr
    // src_stride ignored
    mov         edx, [esp + 12]  // dst_ptr
    mov         ecx, [esp + 16]  // dst_width
    vpcmpeqb    ymm5, ymm5, ymm5  // generate mask 0x00ff0000
    vpsrld      ymm5, ymm5, 24
    vpslld      ymm5, ymm5, 16

  wloop:
    vmovdqu     ymm0, [eax]
    vmovdqu     ymm1, [eax + 32]
    lea         eax,  [eax + 64]
    vpand       ymm0, ymm0, ymm5
    vpand       ymm1, ymm1, ymm5
    vpackuswb   ymm0, ymm0, ymm1
    vpermq      ymm0, ymm0, 0xd8  // unmutate vpackuswb
    vpsrlw      ymm0, ymm0, 8
    vpackuswb   ymm0, ymm0, ymm0
    vpermq      ymm0, ymm0, 0xd8       // unmutate vpackuswb
    vmovdqu     [edx], xmm0
    lea         edx, [edx + 16]
    sub         ecx, 16
    jg          wloop

    vzeroupper
    ret
  }
}

// Blends 64x4 rectangle to 16x1.
__declspec(naked) void ScaleRowDown4Box_AVX2(const uint8_t* src_ptr,
                                             ptrdiff_t src_stride,
                                             uint8_t* dst_ptr,
                                             int dst_width) {
  __asm {
    push        esi
    push        edi
    mov         eax, [esp + 8 + 4]  // src_ptr
    mov         esi, [esp + 8 + 8]  // src_stride
    mov         edx, [esp + 8 + 12]  // dst_ptr
    mov         ecx, [esp + 8 + 16]  // dst_width
    lea         edi, [esi + esi * 2]  // src_stride * 3
    vpcmpeqb    ymm4, ymm4, ymm4  // constant 0x0101
    vpsrlw      ymm4, ymm4, 15
    vpsllw      ymm5, ymm4, 3  // constant 0x0008
    vpackuswb   ymm4, ymm4, ymm4

  wloop:
    vmovdqu     ymm0, [eax]  // average rows
    vmovdqu     ymm1, [eax + 32]
    vmovdqu     ymm2, [eax + esi]
    vmovdqu     ymm3, [eax + esi + 32]
    vpmaddubsw  ymm0, ymm0, ymm4  // horizontal add
    vpmaddubsw  ymm1, ymm1, ymm4
    vpmaddubsw  ymm2, ymm2, ymm4
    vpmaddubsw  ymm3, ymm3, ymm4
    vpaddw      ymm0, ymm0, ymm2  // vertical add rows 0, 1
    vpaddw      ymm1, ymm1, ymm3
    vmovdqu     ymm2, [eax + esi * 2]
    vmovdqu     ymm3, [eax + esi * 2 + 32]
    vpmaddubsw  ymm2, ymm2, ymm4
    vpmaddubsw  ymm3, ymm3, ymm4
    vpaddw      ymm0, ymm0, ymm2  // add row 2
    vpaddw      ymm1, ymm1, ymm3
    vmovdqu     ymm2, [eax + edi]
    vmovdqu     ymm3, [eax + edi + 32]
    lea         eax,  [eax + 64]
    vpmaddubsw  ymm2, ymm2, ymm4
    vpmaddubsw  ymm3, ymm3, ymm4
    vpaddw      ymm0, ymm0, ymm2  // add row 3
    vpaddw      ymm1, ymm1, ymm3
    vphaddw     ymm0, ymm0, ymm1  // mutates
    vpermq      ymm0, ymm0, 0xd8  // unmutate vphaddw
    vpaddw      ymm0, ymm0, ymm5  // + 8 for round
    vpsrlw      ymm0, ymm0, 4  // /32 for average of 4 * 4
    vpackuswb   ymm0, ymm0, ymm0
    vpermq      ymm0, ymm0, 0xd8  // unmutate vpackuswb
    vmovdqu     [edx], xmm0
    lea         edx, [edx + 16]
    sub         ecx, 16
    jg          wloop

    pop        edi
    pop        esi
    vzeroupper
    ret
  }
}
#endif  // HAS_SCALEROWDOWN4_AVX2

// Point samples 32 pixels to 24 pixels.
// Produces three 8 byte values. For each 8 bytes, 16 bytes are read.
// Then shuffled to do the scaling.



// Blends 32x2 rectangle to 24x1
// Produces three 8 byte values. For each 8 bytes, 16 bytes are read.
// Then shuffled to do the scaling.

// Register usage:
// xmm0 src_row 0
// xmm1 src_row 1
// xmm2 shuf 0
// xmm3 shuf 1
// xmm4 shuf 2
// xmm5 madd 0
// xmm6 madd 1
// xmm7 kRound34

// Note that movdqa+palign may be better than movdqu.


// Note that movdqa+palign may be better than movdqu.


// 3/8 point sampler

// Scale 32 pixels to 12


// Scale 16x3 pixels to 6x1 with interpolation


// Scale 16x2 pixels to 6x1 with interpolation


// Reads 16 bytes and accumulates to 16 shorts at a time.


#ifdef HAS_SCALEADDROW_AVX2
// Reads 32 bytes and accumulates to 32 shorts at a time.
__declspec(naked) void ScaleAddRow_AVX2(const uint8_t* src_ptr,
                                        uint16_t* dst_ptr,
                                        int src_width) {
  __asm {
    mov         eax, [esp + 4]  // src_ptr
    mov         edx, [esp + 8]  // dst_ptr
    mov         ecx, [esp + 12]  // src_width
    vpxor       ymm5, ymm5, ymm5

        // sum rows
  xloop:
    vmovdqu     ymm3, [eax]  // read 32 bytes
    lea         eax, [eax + 32]
    vpermq      ymm3, ymm3, 0xd8  // unmutate for vpunpck
    vpunpcklbw  ymm2, ymm3, ymm5
    vpunpckhbw  ymm3, ymm3, ymm5
    vpaddusw    ymm0, ymm2, [edx]  // sum 16 words
    vpaddusw    ymm1, ymm3, [edx + 32]
    vmovdqu     [edx], ymm0  // write 32 words to destination
    vmovdqu     [edx + 32], ymm1
    lea         edx, [edx + 64]
    sub         ecx, 32
    jg          xloop

    vzeroupper
    ret
  }
}
#endif  // HAS_SCALEADDROW_AVX2

// Constant for making pixels signed to avoid pmaddubsw
// saturation.
SIMD_ALIGNED(const uint8_t) kFsub80[32] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                              0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
                              0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};

// Constant for making pixels unsigned and adding .5 for rounding.
SIMD_ALIGNED(const uint16_t) kFadd40[16] = {0x4040, 0x4040, 0x4040, 0x4040,
                               0x4040, 0x4040, 0x4040, 0x4040, 0x4040, 0x4040, 0x4040, 0x4040,
                               0x4040, 0x4040, 0x4040, 0x4040};

// Bilinear column filtering. SSSE3 version.


// Reads 16 pixels, duplicates them and writes 32 pixels.


// Reads 8 pixels, throws half away and writes 4 even pixels (0, 2, 4, 6)


// Blends 8x1 rectangle to 4x1.


// Blends 8x2 rectangle to 4x1.


// Reads 4 pixels at a time.


// Blends four 2x2 to 4x1.


// Column scaling unfiltered. SSE2 version.


// Bilinear row filtering combines 2x1 -> 1x1. SSSE3 version.
// TODO(fbarchard): Port to Neon

// Shuffle table for arranging 2 pixels into pairs for pmaddubsw
SIMD_ALIGNED(const uint8_t) kShuffleColARGB[32] = {
    0u, 4u,  1u, 5u,  2u,  6u,  3u,  7u,  // bbggrraa 1st pixel
    8u, 12u, 9u, 13u, 10u, 14u, 11u, 15u  // bbggrraa 2nd pixel
, 
    0u, 4u,  1u, 5u,  2u,  6u,  3u,  7u,  // bbggrraa 1st pixel
    8u, 12u, 9u, 13u, 10u, 14u, 11u, 15u  // bbggrraa 2nd pixel
};

// Shuffle table for duplicating 2 fractions into 8 bytes each
SIMD_ALIGNED(const uint8_t) kShuffleFractions[32] = {
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u,
, 
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u,
};



// Reads 4 pixels, duplicates them and writes 8 pixels.


// Divide num by div and return as 16.16 fixed point result.
__declspec(naked) int FixedDiv_X86(int num, int div) {
  __asm {
    mov        eax, [esp + 4]  // num
    cdq  // extend num to 64 bits
    shld       edx, eax, 16  // 32.16
    shl        eax, 16
    idiv       dword ptr [esp + 8]
    ret
  }
}

// Divide num by div and return as 16.16 fixed point result.
__declspec(naked) int FixedDiv1_X86(int num, int div) {
  __asm {
    mov        eax, [esp + 4]  // num
    mov        ecx, [esp + 8]  // denom
    cdq  // extend num to 64 bits
    shld       edx, eax, 16  // 32.16
    shl        eax, 16
    sub        eax, 0x00010001
    sbb        edx, 0
    sub        ecx, 1
    idiv       ecx
    ret
  }
}
#endif  // !defined(LIBYUV_DISABLE_X86) && defined(_M_IX86)

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
