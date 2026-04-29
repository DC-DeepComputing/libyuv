/*
 *  Copyright 2013 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "./psnr.h"  // NOLINT

#ifdef _OPENMP
#include <omp.h>
#endif
#ifdef _MSC_VER
#include <intrin.h>  // For __cpuid()
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int uint32_t;  // NOLINT
#ifdef _MSC_VER
typedef unsigned __int64 uint64_t;
#else  // COMPILER_MSVC
#if defined(__LP64__) && !defined(__OpenBSD__) && !defined(__APPLE__)
typedef unsigned long uint64_t;  // NOLINT
#else   // defined(__LP64__) && !defined(__OpenBSD__) && !defined(__APPLE__)
typedef unsigned long long uint64_t;  // NOLINT
#endif  // __LP64__
#endif  // _MSC_VER

// libyuv provides this function when linking library for jpeg support.
#if !defined(HAVE_JPEG)

#if !defined(LIBYUV_DISABLE_NEON) && defined(__ARM_NEON__) && \
    !defined(__aarch64__)
#define HAS_SUMSQUAREERROR_NEON
static uint32_t SumSquareError_NEON(const uint8_t* src_a,
                                    const uint8_t* src_b,
                                    int count) {
  volatile uint32_t sse;
  asm volatile(
      "vmov.u8    q7, #0                         \n"
      "vmov.u8    q9, #0                         \n"
      "vmov.u8    q8, #0                         \n"
      "vmov.u8    q10, #0                        \n"

      "1:                                        \n"
      "vld1.u8    {q0}, [%0]!                    \n"
      "vld1.u8    {q1}, [%1]!                    \n"
      "vsubl.u8   q2, d0, d2                     \n"
      "vsubl.u8   q3, d1, d3                     \n"
      "vmlal.s16  q7, d4, d4                     \n"
      "vmlal.s16  q8, d6, d6                     \n"
      "vmlal.s16  q8, d5, d5                     \n"
      "vmlal.s16  q10, d7, d7                    \n"
      "subs       %2, %2, #16                    \n"
      "bhi        1b                             \n"

      "vadd.u32   q7, q7, q8                     \n"
      "vadd.u32   q9, q9, q10                    \n"
      "vadd.u32   q10, q7, q9                    \n"
      "vpaddl.u32 q1, q10                        \n"
      "vadd.u64   d0, d2, d3                     \n"
      "vmov.32    %3, d0[0]                      \n"
      : "+r"(src_a), "+r"(src_b), "+r"(count), "=r"(sse)
      :
      : "memory", "cc", "q0", "q1", "q2", "q3", "q7", "q8", "q9", "q10");
  return sse;
}
#elif !defined(LIBYUV_DISABLE_NEON) && defined(__aarch64__)
#define HAS_SUMSQUAREERROR_NEON
static uint32_t SumSquareError_NEON(const uint8_t* src_a,
                                    const uint8_t* src_b,
                                    int count) {
  volatile uint32_t sse;
  asm volatile(
      "eor        v16.16b, v16.16b, v16.16b      \n"
      "eor        v18.16b, v18.16b, v18.16b      \n"
      "eor        v17.16b, v17.16b, v17.16b      \n"
      "eor        v19.16b, v19.16b, v19.16b      \n"

      "1:                                        \n"
      "ld1        {v0.16b}, [%0], #16            \n"
      "ld1        {v1.16b}, [%1], #16            \n"
      "subs       %w2, %w2, #16                  \n"
      "usubl      v2.8h, v0.8b, v1.8b            \n"
      "usubl2     v3.8h, v0.16b, v1.16b          \n"
      "smlal      v16.4s, v2.4h, v2.4h           \n"
      "smlal      v17.4s, v3.4h, v3.4h           \n"
      "smlal2     v18.4s, v2.8h, v2.8h           \n"
      "smlal2     v19.4s, v3.8h, v3.8h           \n"
      "b.gt       1b                             \n"

      "add        v16.4s, v16.4s, v17.4s         \n"
      "add        v18.4s, v18.4s, v19.4s         \n"
      "add        v19.4s, v16.4s, v18.4s         \n"
      "addv       s0, v19.4s                     \n"
      "fmov       %w3, s0                        \n"
      : "+r"(src_a), "+r"(src_b), "+r"(count), "=r"(sse)
      :
      : "cc", "v0", "v1", "v2", "v3", "v16", "v17", "v18", "v19");
  return sse;
}
#elif !defined(LIBYUV_DISABLE_X86) && defined(_M_IX86) && defined(_MSC_VER)

#elif !defined(LIBYUV_DISABLE_X86) && (defined(__x86_64__) || defined(__i386__))

#endif  // LIBYUV_DISABLE_X86 etc

#if 1
#if (defined(__pic__) || defined(__APPLE__)) && defined(__i386__)
static __inline void __cpuid(int cpu_info[4], int info_type) {
  asm volatile(  // NOLINT
      "mov %%ebx, %%edi                          \n"
      "cpuid                                     \n"
      "xchg %%edi, %%ebx                         \n"
      : "=a"(cpu_info[0]), "=D"(cpu_info[1]), "=c"(cpu_info[2]),
        "=d"(cpu_info[3])
      : "a"(info_type));
}
// For gcc/clang but not clangcl.
#elif !defined(_MSC_VER) && (defined(__i386__) || defined(__x86_64__))
static __inline void __cpuid(int cpu_info[4], int info_type) {
  asm volatile(  // NOLINT
      "cpuid                                     \n"
      : "=a"(cpu_info[0]), "=b"(cpu_info[1]), "=c"(cpu_info[2]),
        "=d"(cpu_info[3])
      : "a"(info_type));
}
#endif


static uint32_t SumSquareError_C(const uint8_t* src_a,
                                 const uint8_t* src_b,
                                 int count) {
  uint32_t sse = 0u;
  for (int x = 0; x < count; ++x) {
    int diff = src_a[x] - src_b[x];
    sse += static_cast<uint32_t>(diff * diff);
  }
  return sse;
}

double ComputeSumSquareError(const uint8_t* src_a,
                             const uint8_t* src_b,
                             int count) {
  uint32_t (*SumSquareError)(const uint8_t* src_a, const uint8_t* src_b,
                             int count) = SumSquareError_C;
#if defined(HAS_SUMSQUAREERROR_NEON)
  SumSquareError = SumSquareError_NEON;
#endif
  const int kBlockSize = 1 << 15;
  uint64_t sse = 0;
#ifdef _OPENMP
#pragma omp parallel for reduction(+ : sse)
#endif
  for (int i = 0; i < (count - (kBlockSize - 1)); i += kBlockSize) {
    sse += SumSquareError(src_a + i, src_b + i, kBlockSize);
  }
  src_a += count & ~(kBlockSize - 1);
  src_b += count & ~(kBlockSize - 1);
  int remainder = count & (kBlockSize - 1) & ~15;
  if (remainder) {
    sse += SumSquareError(src_a, src_b, remainder);
    src_a += remainder;
    src_b += remainder;
  }
  remainder = count & 15;
  if (remainder) {
    sse += SumSquareError_C(src_a, src_b, remainder);
  }
  return static_cast<double>(sse);
}
#endif

// PSNR formula: psnr = 10 * log10 (Peak Signal^2 * size / sse)
// Returns 128.0 (kMaxPSNR) if sse is 0 (perfect match).
double ComputePSNR(double sse, double size) {
  const double kMINSSE = 255.0 * 255.0 * size / pow(10.0, kMaxPSNR / 10.0);
  if (sse <= kMINSSE) {
    sse = kMINSSE;  // Produces max PSNR of 128
  }
  return 10.0 * log10(255.0 * 255.0 * size / sse);
}

#ifdef __cplusplus
}  // extern "C"
#endif
