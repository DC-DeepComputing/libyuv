/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <ctype.h>
#include <sys/utsname.h>
#endif

#include "../unit_test/unit_test.h"
#include "libyuv/basic_types.h"
#include "libyuv/cpu_id.h"
#include "libyuv/version.h"

namespace libyuv {

#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || \
    defined(_M_X64)
TEST_F(LibYUVBaseTest, TestCpuId) {
  int has_x86 = TestCpuFlag(kCpuHasX86);
  if (has_x86) {
    int cpu_info[4];
    // Vendor ID:
    // AuthenticAMD AMD processor
    // CentaurHauls Centaur processor
    // CyrixInstead Cyrix processor
    // GenuineIntel Intel processor
    // GenuineTMx86 Transmeta processor
    // Geode by NSC National Semiconductor processor
    // NexGenDriven NexGen processor
    // RiseRiseRise Rise Technology processor
    // SiS SiS SiS  SiS processor
    // UMC UMC UMC  UMC processor
    CpuId(0, 0, cpu_info);
    cpu_info[0] = cpu_info[1];  // Reorder output
    cpu_info[1] = cpu_info[3];
    cpu_info[3] = 0;
    printf("Cpu Vendor: %s 0x%x 0x%x 0x%x\n",
           reinterpret_cast<char*>(&cpu_info[0]), cpu_info[0], cpu_info[1],
           cpu_info[2]);
    EXPECT_EQ(12u, strlen(reinterpret_cast<char*>(&cpu_info[0])));

    // CPU Family and Model
    // 3:0 - Stepping
    // 7:4 - Model
    // 11:8 - Family
    // 13:12 - Processor Type
    // 19:16 - Extended Model
    // 27:20 - Extended Family
    CpuId(1, 0, cpu_info);
    int family = ((cpu_info[0] >> 8) & 0x0f) | ((cpu_info[0] >> 16) & 0xff0);
    int model = ((cpu_info[0] >> 4) & 0x0f) | ((cpu_info[0] >> 12) & 0xf0);
    printf("Cpu Family %d (0x%x), Model %d (0x%x)\n", family, family, model,
           model);
  }
}
#endif

#ifdef __linux__
static void KernelVersion(int* version) {
  struct utsname buffer;
  int i = 0;

  version[0] = version[1] = 0;
  if (uname(&buffer) == 0) {
    char* v = buffer.release;
    for (i = 0; *v && i < 2; ++v) {
      if (isdigit(*v)) {
        version[i++] = (int)strtol(v, &v, 10);
      }
    }
  }
}
#endif

TEST_F(LibYUVBaseTest, TestCpuHas) {
#if defined(__linux__)
  {
    int kernelversion[2];
    KernelVersion(kernelversion);
    printf("Kernel Version %d.%d\n", kernelversion[0], kernelversion[1]);
  }
#endif  // defined(__linux__)

#if defined(__arm__) || defined(__aarch64__)
  int has_arm = TestCpuFlag(kCpuHasARM);
  if (has_arm) {
    int has_neon = TestCpuFlag(kCpuHasNEON);
    int has_neon_dotprod = TestCpuFlag(kCpuHasNeonDotProd);
    int has_neon_i8mm = TestCpuFlag(kCpuHasNeonI8MM);
    int has_sve = TestCpuFlag(kCpuHasSVE);
    int has_sve2 = TestCpuFlag(kCpuHasSVE2);
    int has_sve_f32mm = TestCpuFlag(kCpuHasSVEF32MM);
    int has_sme = TestCpuFlag(kCpuHasSME);
    int has_sme2 = TestCpuFlag(kCpuHasSME2);
    printf("Has Arm 0x%x\n", has_arm);
    printf("Has Neon 0x%x\n", has_neon);
    printf("Has Neon DotProd 0x%x\n", has_neon_dotprod);
    printf("Has Neon I8MM 0x%x\n", has_neon_i8mm);
    printf("Has SVE 0x%x\n", has_sve);
    printf("Has SVE2 0x%x\n", has_sve2);
    printf("Has SVE F32MM 0x%x\n", has_sve_f32mm);
    printf("Has SME 0x%x\n", has_sme);
    printf("Has SME2 0x%x\n", has_sme2);

#if defined(__aarch64__)
    // Read and print the SVE and SME vector lengths.
    if (has_sve) {
      int sve_vl;
      asm(".inst 0x04bf5020    \n"  // rdvl x0, #1
          "mov %w[sve_vl], w0  \n"
          : [sve_vl] "=r"(sve_vl)  // %[sve_vl]
          :
          : "x0");
      printf("SVE vector length: %d bytes\n", sve_vl);
    }
    if (has_sme) {
      int sme_vl;
      asm(".inst 0x04bf5820    \n"  // rdsvl x0, #1
          "mov %w[sme_vl], w0  \n"
          : [sme_vl] "=r"(sme_vl)  // %[sme_vl]
          :
          : "x0");
      printf("SME vector length: %d bytes\n", sme_vl);
    }
#endif  // defined(__aarch64__)
  }
#endif  // if defined(__arm__) || defined(__aarch64__)

#if defined(__riscv)
  int has_riscv = TestCpuFlag(kCpuHasRISCV);
  if (has_riscv) {
    int has_rvv = TestCpuFlag(kCpuHasRVV);
    printf("Has RISCV 0x%x\n", has_riscv);
    printf("Has RVV 0x%x\n", has_rvv);

    // Read and print the RVV vector length.
    if (has_rvv) {
      register uint32_t vlenb __asm__("t0");
      __asm__(".word 0xC22022F3" /* CSRR t0, vlenb */ : "=r"(vlenb));
      printf("RVV vector length: %d bytes\n", vlenb);
    }
  }
#endif  // defined(__riscv)

#if defined(__loongarch__)
  int has_loongarch = TestCpuFlag(kCpuHasLOONGARCH);
  if (has_loongarch) {
    int has_lsx = TestCpuFlag(kCpuHasLSX);
    int has_lasx = TestCpuFlag(kCpuHasLASX);
    printf("Has LOONGARCH 0x%x\n", has_loongarch);
    printf("Has LSX 0x%x\n", has_lsx);
    printf("Has LASX 0x%x\n", has_lasx);
  }
#endif  // defined(__loongarch__)

#if defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) || \
    defined(_M_X64)
  int has_x86 = TestCpuFlag(kCpuHasX86);
  if (has_x86) {
    int has_sse2 = TestCpuFlag(kCpuHasSSE2);
    int has_ssse3 = TestCpuFlag(kCpuHasSSSE3);
    int has_sse41 = TestCpuFlag(kCpuHasSSE41);
    int has_sse42 = TestCpuFlag(kCpuHasSSE42);
    int has_avx = TestCpuFlag(kCpuHasAVX);
    int has_avx2 = TestCpuFlag(kCpuHasAVX2);
    int has_erms = TestCpuFlag(kCpuHasERMS);
    int has_fsmr = TestCpuFlag(kCpuHasFSMR);
    int has_fma3 = TestCpuFlag(kCpuHasFMA3);
    int has_f16c = TestCpuFlag(kCpuHasF16C);
    int has_avx512bw = TestCpuFlag(kCpuHasAVX512BW);
    int has_avx512vl = TestCpuFlag(kCpuHasAVX512VL);
    int has_avx512vnni = TestCpuFlag(kCpuHasAVX512VNNI);
    int has_avx512vbmi = TestCpuFlag(kCpuHasAVX512VBMI);
    int has_avx512vbmi2 = TestCpuFlag(kCpuHasAVX512VBMI2);
    int has_avx512vbitalg = TestCpuFlag(kCpuHasAVX512VBITALG);
    int has_avx10 = TestCpuFlag(kCpuHasAVX10);
    int has_avx10_2 = TestCpuFlag(kCpuHasAVX10_2);
    int has_avxvnni = TestCpuFlag(kCpuHasAVXVNNI);
    int has_avxvnniint8 = TestCpuFlag(kCpuHasAVXVNNIINT8);
    int has_amxint8 = TestCpuFlag(kCpuHasAMXINT8);
    printf("Has X86 0x%x\n", has_x86);
    printf("Has SSE2 0x%x\n", has_sse2);
    printf("Has SSSE3 0x%x\n", has_ssse3);
    printf("Has SSE4.1 0x%x\n", has_sse41);
    printf("Has SSE4.2 0x%x\n", has_sse42);
    printf("Has AVX 0x%x\n", has_avx);
    printf("Has AVX2 0x%x\n", has_avx2);
    printf("Has ERMS 0x%x\n", has_erms);
    printf("Has FSMR 0x%x\n", has_fsmr);
    printf("Has FMA3 0x%x\n", has_fma3);
    printf("Has F16C 0x%x\n", has_f16c);
    printf("Has AVX512BW 0x%x\n", has_avx512bw);
    printf("Has AVX512VL 0x%x\n", has_avx512vl);
    printf("Has AVX512VNNI 0x%x\n", has_avx512vnni);
    printf("Has AVX512VBMI 0x%x\n", has_avx512vbmi);
    printf("Has AVX512VBMI2 0x%x\n", has_avx512vbmi2);
    printf("Has AVX512VBITALG 0x%x\n", has_avx512vbitalg);
    printf("Has AVX10 0x%x\n", has_avx10);
    printf("Has AVX10_2 0x%x\n", has_avx10_2);
    printf("HAS AVXVNNI 0x%x\n", has_avxvnni);
    printf("Has AVXVNNIINT8 0x%x\n", has_avxvnniint8);
    printf("Has AMXINT8 0x%x\n", has_amxint8);
  }
#endif  // defined(__i386__) || defined(__x86_64__) || defined(_M_IX86) ||
        // defined(_M_X64)
}

TEST_F(LibYUVBaseTest, TestCompilerMacros) {
  // Tests all macros used in public headers.
#ifdef __ATOMIC_RELAXED
  printf("__ATOMIC_RELAXED %d\n", __ATOMIC_RELAXED);
#endif
#ifdef __clang__
  printf("__clang__ %d\n", __clang__);
#endif
#ifdef __clang_major__
  printf("__clang_major__ %d\n", __clang_major__);
#endif
#ifdef __clang_minor__
  printf("__clang_minor__ %d\n", __clang_minor__);
#endif
#ifdef __GNUC__
  printf("__GNUC__ %d\n", __GNUC__);
#endif
#ifdef __GNUC_MINOR__
  printf("__GNUC_MINOR__ %d\n", __GNUC_MINOR__);
#endif
#ifdef __mips_msa
  printf("__mips_msa %d\n", __mips_msa);
#endif
#ifdef __pic__
  printf("__pic__ %d\n", __pic__);
#endif
#ifdef __pnacl__
  printf("__pnacl__ %d\n", __pnacl__);
#endif
#if defined(__mips__) && defined(__mips_dsp) && (__mips_dsp_rev >= 2)
  printf("__mips_dsp_rev %d\n", __mips_dsp_rev);
#endif
#ifdef _M_ARM
  printf("_M_ARM %d\n", _M_ARM);
#endif
#ifdef _M_ARM64
  printf("_M_ARM64 %d\n", _M_ARM64);
#endif
#ifdef _MSC_VER
  printf("_MSC_VER %d\n", _MSC_VER);
#endif
#ifdef __AARCH64EB__
  printf("__AARCH64EB__ %d\n", __AARCH64EB__);
#endif
#ifdef __aarch64__
  printf("__aarch64__ %d\n", __aarch64__);
#endif
#ifdef __ARMEB__
  printf("__ARMEB__ %d\n", __ARMEB__);
#endif
#ifdef __arm__
  printf("__arm__ %d\n", __arm__);
#endif
#ifdef __APPLE__
  printf("__APPLE__ %d\n", __APPLE__);
#endif
#ifdef __CLR_VER
  printf("__CLR_VER %d\n", __CLR_VER);
#endif
#ifdef __CYGWIN__
  printf("__CYGWIN__ %d\n", __CYGWIN__);
#endif
#ifdef __has_feature
  printf("__has_feature %d\n", __has_feature);
#endif
#ifdef __has_include
  printf("__has_include %d\n", __has_include);
#endif
#ifdef __i386__
  printf("__i386__ %d\n", __i386__);
#endif
#ifdef __linux__
  printf("__linux__ %d\n", __linux__);
#endif
#ifdef __mips__
  printf("__mips__ %d\n", __mips__);
#endif
#ifdef __native_client__
  printf("__native_client__ %d\n", __native_client__);
#endif
#ifdef __powerpc__
  printf("__powerpc__ %d\n", __powerpc__);
#endif
#ifdef __x86_64__
  printf("__x86_64__ %d\n", __x86_64__);
#endif
#ifdef _M_IX86
  printf("_M_IX86 %d\n", _M_IX86);
#endif
#ifdef _M_X64
  printf("_M_X64 %d\n", _M_X64);
#endif
}

TEST_F(LibYUVBaseTest, TestCpuMacro) {
#if defined(HAS_HASHDJB2_AVX2)
  printf("HAS_HASHDJB2_AVX2\n");
#endif
#if defined(HAS_SUMSQUAREERROR_AVX2)
  printf("HAS_SUMSQUAREERROR_AVX2\n");
#endif
#if defined(HAS_HAMMINGDISTANCE_AVX2)
  printf("HAS_HAMMINGDISTANCE_AVX2\n");
#endif
#if defined(HAS_HASHDJB2_NEON)
  printf("HAS_HASHDJB2_NEON\n");
#endif
#if defined(HAS_HAMMINGDISTANCE_NEON)
  printf("HAS_HAMMINGDISTANCE_NEON\n");
#endif
#if defined(HAS_SUMSQUAREERROR_NEON)
  printf("HAS_SUMSQUAREERROR_NEON\n");
#endif
#if defined(HAS_HAMMINGDISTANCE_NEON_DOTPROD)
  printf("HAS_HAMMINGDISTANCE_NEON_DOTPROD\n");
#endif
#if defined(HAS_SUMSQUAREERROR_NEON_DOTPROD)
  printf("HAS_SUMSQUAREERROR_NEON_DOTPROD\n");
#endif
#if defined(HAS_TRANSPOSE4X4_32_AVX2)
  printf("HAS_TRANSPOSE4X4_32_AVX2\n");
#endif
#if defined(HAS_TRANSPOSE4X4_32_NEON)
  printf("HAS_TRANSPOSE4X4_32_NEON\n");
#endif
#if defined(HAS_TRANSPOSE4X4_32_SME)
  printf("HAS_TRANSPOSE4X4_32_SME\n");
#endif
#if defined(HAS_TRANSPOSE4X4_32_SVE2)
  printf("HAS_TRANSPOSE4X4_32_SVE2\n");
#endif
#if defined(HAS_SCALEROWDOWN2_SSSE3)
  printf("HAS_SCALEROWDOWN2_SSSE3\n");
#endif
#if defined(HAS_SCALEROWDOWN2_AVX2)
  printf("HAS_SCALEROWDOWN2_AVX2\n");
#endif
#if defined(HAS_SCALEROWDOWN2_NEON)
  printf("HAS_SCALEROWDOWN2_NEON\n");
#endif
#if defined(HAS_SCALEROWDOWN2_SME)
  printf("HAS_SCALEROWDOWN2_SME\n");
#endif
#if defined(HAS_SCALEROWDOWN2_SVE2)
  printf("HAS_SCALEROWDOWN2_SVE2\n");
#endif
#if defined(HAS_SCALEROWDOWN4_SSSE3)
  printf("HAS_SCALEROWDOWN4_SSSE3\n");
#endif
#if defined(HAS_SCALEROWDOWN4_AVX2)
  printf("HAS_SCALEROWDOWN4_AVX2\n");
#endif
#if defined(HAS_SCALEROWDOWN4_NEON)
  printf("HAS_SCALEROWDOWN4_NEON\n");
#endif
#if defined(HAS_SCALEROWDOWN4_SME)
  printf("HAS_SCALEROWDOWN4_SME\n");
#endif
#if defined(HAS_SCALEROWDOWN4_SVE2)
  printf("HAS_SCALEROWDOWN4_SVE2\n");
#endif
#if defined(HAS_SCALEROWDOWN34_SSSE3)
  printf("HAS_SCALEROWDOWN34_SSSE3\n");
#endif
#if defined(HAS_SCALEROWDOWN34_NEON)
  printf("HAS_SCALEROWDOWN34_NEON\n");
#endif
#if defined(HAS_SCALEROWDOWN34_SME)
  printf("HAS_SCALEROWDOWN34_SME\n");
#endif
#if defined(HAS_SCALEROWDOWN34_SVE2)
  printf("HAS_SCALEROWDOWN34_SVE2\n");
#endif
#if defined(HAS_SCALEROWDOWN38_SSSE3)
  printf("HAS_SCALEROWDOWN38_SSSE3\n");
#endif
#if defined(HAS_SCALEROWDOWN38_NEON)
  printf("HAS_SCALEROWDOWN38_NEON\n");
#endif
#if defined(HAS_SCALEROWDOWN38_SME)
  printf("HAS_SCALEROWDOWN38_SME\n");
#endif
#if defined(HAS_SCALEROWDOWN38_SVE2)
  printf("HAS_SCALEROWDOWN38_SVE2\n");
#endif
#if defined(HAS_SCALEARGBROWDOWN2_SSE2)
  printf("HAS_SCALEARGBROWDOWN2_SSE2\n");
#endif
#if defined(HAS_SCALEARGBROWDOWN2_NEON)
  printf("HAS_SCALEARGBROWDOWN2_NEON\n");
#endif
#if defined(HAS_SCALEARGBROWDOWN2_SME)
  printf("HAS_SCALEARGBROWDOWN2_SME\n");
#endif
#if defined(HAS_SCALEARGBROWDOWN2_SVE2)
  printf("HAS_SCALEARGBROWDOWN2_SVE2\n");
#endif
#if defined(HAS_SCALEARGBROWDOWNEVEN_SSE2)
  printf("HAS_SCALEARGBROWDOWNEVEN_SSE2\n");
#endif
#if defined(HAS_SCALEARGBROWDOWNEVEN_NEON)
  printf("HAS_SCALEARGBROWDOWNEVEN_NEON\n");
#endif
#if defined(HAS_SCALEARGBROWDOWNEVEN_SME)
  printf("HAS_SCALEARGBROWDOWNEVEN_SME\n");
#endif
#if defined(HAS_SCALEARGBROWDOWNEVEN_SVE2)
  printf("HAS_SCALEARGBROWDOWNEVEN_SVE2\n");
#endif
#if defined(HAS_SCALEARGBCOLS_SSE2)
  printf("HAS_SCALEARGBCOLS_SSE2\n");
#endif
#if defined(HAS_SCALEARGBCOLS_NEON)
  printf("HAS_SCALEARGBCOLS_NEON\n");
#endif
#if defined(HAS_SCALEARGBCOLS_SME)
  printf("HAS_SCALEARGBCOLS_SME\n");
#endif
#if defined(HAS_SCALEARGBCOLS_SVE2)
  printf("HAS_SCALEARGBCOLS_SVE2\n");
#endif
#if defined(HAS_SCALEARGBFILTERCOLS_SSSE3)
  printf("HAS_SCALEARGBFILTERCOLS_SSSE3\n");
#endif
#if defined(HAS_SCALEARGBFILTERCOLS_NEON)
  printf("HAS_SCALEARGBFILTERCOLS_NEON\n");
#endif
#if defined(HAS_SCALEARGBFILTERCOLS_SME)
  printf("HAS_SCALEARGBFILTERCOLS_SME\n");
#endif
#if defined(HAS_SCALEARGBFILTERCOLS_SVE2)
  printf("HAS_SCALEARGBFILTERCOLS_SVE2\n");
#endif
#if defined(HAS_SCALEARGBCOLSUP2_SSE2)
  printf("HAS_SCALEARGBCOLSUP2_SSE2\n");
#endif
#if defined(HAS_SCALEROWDOWN2_16_SSE2)
  printf("HAS_SCALEROWDOWN2_16_SSE2\n");
#endif
#if defined(HAS_SCALEROWDOWN2_16_AVX2)
  printf("HAS_SCALEROWDOWN2_16_AVX2\n");
#endif
#if defined(HAS_SCALEROWDOWN2_16_NEON)
  printf("HAS_SCALEROWDOWN2_16_NEON\n");
#endif
#if defined(HAS_SCALEROWDOWN2_16_SME)
  printf("HAS_SCALEROWDOWN2_16_SME\n");
#endif
#if defined(HAS_SCALEROWDOWN2_16_SVE2)
  printf("HAS_SCALEROWDOWN2_16_SVE2\n");
#endif
#if defined(HAS_SCALEROWDOWN4_16_SSE2)
  printf("HAS_SCALEROWDOWN4_16_SSE2\n");
#endif
#if defined(HAS_SCALEROWDOWN4_16_AVX2)
  printf("HAS_SCALEROWDOWN4_16_AVX2\n");
#endif
#if defined(HAS_SCALEROWDOWN4_16_NEON)
  printf("HAS_SCALEROWDOWN4_16_NEON\n");
#endif
#if defined(HAS_SCALEROWDOWN4_16_SME)
  printf("HAS_SCALEROWDOWN4_16_SME\n");
#endif
#if defined(HAS_SCALEROWDOWN4_16_SVE2)
  printf("HAS_SCALEROWDOWN4_16_SVE2\n");
#endif
#if defined(HAS_SCALEROWDOWN34_16_SSSE3)
  printf("HAS_SCALEROWDOWN34_16_SSSE3\n");
#endif
#if defined(HAS_SCALEROWDOWN34_16_NEON)
  printf("HAS_SCALEROWDOWN34_16_NEON\n");
#endif
#if defined(HAS_SCALEROWDOWN34_16_SME)
  printf("HAS_SCALEROWDOWN34_16_SME\n");
#endif
#if defined(HAS_SCALEROWDOWN34_16_SVE2)
  printf("HAS_SCALEROWDOWN34_16_SVE2\n");
#endif
#if defined(HAS_SCALEROWDOWN38_16_SSSE3)
  printf("HAS_SCALEROWDOWN38_16_SSSE3\n");
#endif
#if defined(HAS_SCALEROWDOWN38_16_NEON)
  printf("HAS_SCALEROWDOWN38_16_NEON\n");
#endif
#if defined(HAS_SCALEROWDOWN38_16_SME)
  printf("HAS_SCALEROWDOWN38_16_SME\n");
#endif
#if defined(HAS_SCALEROWDOWN38_16_SVE2)
  printf("HAS_SCALEROWDOWN38_16_SVE2\n");
#endif
#if defined(HAS_SCALEADDROW_16_SSE2)
  printf("HAS_SCALEADDROW_16_SSE2\n");
#endif
#if defined(HAS_SCALEADDROW_16_AVX2)
  printf("HAS_SCALEADDROW_16_AVX2\n");
#endif
#if defined(HAS_SCALEADDROW_16_NEON)
  printf("HAS_SCALEADDROW_16_NEON\n");
#endif
#if defined(HAS_SCALEADDROW_16_SME)
  printf("HAS_SCALEADDROW_16_SME\n");
#endif
#if defined(HAS_SCALEADDROW_16_SVE2)
  printf("HAS_SCALEADDROW_16_SVE2\n");
#endif
#if defined(HAS_SCALEFILTERCOLS_16_SSSE3)
  printf("HAS_SCALEFILTERCOLS_16_SSSE3\n");
#endif
#if defined(HAS_SCALEFILTERCOLS_16_NEON)
  printf("HAS_SCALEFILTERCOLS_16_NEON\n");
#endif
#if defined(HAS_SCALEFILTERCOLS_16_SME)
  printf("HAS_SCALEFILTERCOLS_16_SME\n");
#endif
#if defined(HAS_SCALEFILTERCOLS_16_SVE2)
  printf("HAS_SCALEFILTERCOLS_16_SVE2\n");
#endif
#if defined(HAS_SCALECOLS_16_SSE2)
  printf("HAS_SCALECOLS_16_SSE2\n");
#endif
#if defined(HAS_SCALECOLS_16_NEON)
  printf("HAS_SCALECOLS_16_NEON\n");
#endif
#if defined(HAS_SCALECOLS_16_SME)
  printf("HAS_SCALECOLS_16_SME\n");
#endif
#if defined(HAS_SCALECOLS_16_SVE2)
  printf("HAS_SCALECOLS_16_SVE2\n");
#endif
#if defined(HAS_SCALEROWUP2_16_SSE2)
  printf("HAS_SCALEROWUP2_16_SSE2\n");
#endif
#if defined(HAS_SCALEROWUP2_16_SSSE3)
  printf("HAS_SCALEROWUP2_16_SSSE3\n");
#endif
#if defined(HAS_SCALEROWUP2_16_AVX2)
  printf("HAS_SCALEROWUP2_16_AVX2\n");
#endif
#if defined(HAS_SCALEROWUP2_16_NEON)
  printf("HAS_SCALEROWUP2_16_NEON\n");
#endif
#if defined(HAS_SCALEROWUP2_16_SME)
  printf("HAS_SCALEROWUP2_16_SME\n");
#endif
#if defined(HAS_SCALEROWUP2_16_SVE2)
  printf("HAS_SCALEROWUP2_16_SVE2\n");
#endif
#if defined(HAS_SCALEUVROWDOWN2_SSSE3)
  printf("HAS_SCALEUVROWDOWN2_SSSE3\n");
#endif
#if defined(HAS_SCALEUVROWDOWN2_AVX2)
  printf("HAS_SCALEUVROWDOWN2_AVX2\n");
#endif
#if defined(HAS_SCALEUVROWDOWN2_NEON)
  printf("HAS_SCALEUVROWDOWN2_NEON\n");
#endif
#if defined(HAS_SCALEUVROWDOWN2_SME)
  printf("HAS_SCALEUVROWDOWN2_SME\n");
#endif
#if defined(HAS_SCALEUVROWDOWN2_SVE2)
  printf("HAS_SCALEUVROWDOWN2_SVE2\n");
#endif
#if defined(HAS_SCALEUVROWUP2_SSSE3)
  printf("HAS_SCALEUVROWUP2_SSSE3\n");
#endif
#if defined(HAS_SCALEUVROWUP2_AVX2)
  printf("HAS_SCALEUVROWUP2_AVX2\n");
#endif
#if defined(HAS_SCALEUVROWUP2_NEON)
  printf("HAS_SCALEUVROWUP2_NEON\n");
#endif
#if defined(HAS_SCALEUVROWUP2_SME)
  printf("HAS_SCALEUVROWUP2_SME\n");
#endif
#if defined(HAS_SCALEUVROWUP2_SVE2)
  printf("HAS_SCALEUVROWUP2_SVE2\n");
#endif
#if defined(HAS_SCALEUVROWUP2_16_SSE41)
  printf("HAS_SCALEUVROWUP2_16_SSE41\n");
#endif
#if defined(HAS_SCALEUVROWUP2_16_AVX2)
  printf("HAS_SCALEUVROWUP2_16_AVX2\n");
#endif
#if defined(HAS_SCALEUVROWUP2_16_NEON)
  printf("HAS_SCALEUVROWUP2_16_NEON\n");
#endif
#if defined(HAS_SCALEUVROWUP2_16_SME)
  printf("HAS_SCALEUVROWUP2_16_SME\n");
#endif
#if defined(HAS_SCALEUVROWUP2_16_SVE2)
  printf("HAS_SCALEUVROWUP2_16_SVE2\n");
#endif
#if defined(HAS_SCALEADDROW_SSE2)
  printf("HAS_SCALEADDROW_SSE2\n");
#endif
#if defined(HAS_SCALEADDROW_AVX2)
  printf("HAS_SCALEADDROW_AVX2\n");
#endif
#if defined(HAS_SCALEADDROW_NEON)
  printf("HAS_SCALEADDROW_NEON\n");
#endif
#if defined(HAS_SCALEADDROW_SME)
  printf("HAS_SCALEADDROW_SME\n");
#endif
#if defined(HAS_SCALEADDROW_SVE2)
  printf("HAS_SCALEADDROW_SVE2\n");
#endif
#if defined(HAS_SCALEFILTERCOLS_SSSE3)
  printf("HAS_SCALEFILTERCOLS_SSSE3\n");
#endif
#if defined(HAS_SCALEFILTERCOLS_NEON)
  printf("HAS_SCALEFILTERCOLS_NEON\n");
#endif
#if defined(HAS_SCALEFILTERCOLS_SME)
  printf("HAS_SCALEFILTERCOLS_SME\n");
#endif
#if defined(HAS_SCALEFILTERCOLS_SVE2)
  printf("HAS_SCALEFILTERCOLS_SVE2\n");
#endif
#if defined(HAS_SCALECOLS_SSE2)
  printf("HAS_SCALECOLS_SSE2\n");
#endif
#if defined(HAS_SCALECOLS_NEON)
  printf("HAS_SCALECOLS_NEON\n");
#endif
#if defined(HAS_SCALECOLS_SME)
  printf("HAS_SCALECOLS_SME\n");
#endif
#if defined(HAS_SCALECOLS_SVE2)
  printf("HAS_SCALECOLS_SVE2\n");
#endif
#if defined(HAS_SCALEROWUP2_SSE2)
  printf("HAS_SCALEROWUP2_SSE2\n");
#endif
#if defined(HAS_SCALEROWUP2_SSSE3)
  printf("HAS_SCALEROWUP2_SSSE3\n");
#endif
#if defined(HAS_SCALEROWUP2_AVX2)
  printf("HAS_SCALEROWUP2_AVX2\n");
#endif
#if defined(HAS_SCALEROWUP2_NEON)
  printf("HAS_SCALEROWUP2_NEON\n");
#endif
#if defined(HAS_SCALEROWUP2_SME)
  printf("HAS_SCALEROWUP2_SME\n");
#endif
#if defined(HAS_SCALEROWUP2_SVE2)
  printf("HAS_SCALEROWUP2_SVE2\n");
#endif
#if defined(HAS_ARGBBLENDROW_SSSE3)
  printf("HAS_ARGBBLENDROW_SSSE3\n");
#endif
#if defined(HAS_ARGBBLENDROW_NEON)
  printf("HAS_ARGBBLENDROW_NEON\n");
#endif
#if defined(HAS_ARGBBLENDROW_SME)
  printf("HAS_ARGBBLENDROW_SME\n");
#endif
#if defined(HAS_ARGBBLENDROW_SVE2)
  printf("HAS_ARGBBLENDROW_SVE2\n");
#endif
#if defined(HAS_ARGBATTENUATEROW_SSSE3)
  printf("HAS_ARGBATTENUATEROW_SSSE3\n");
#endif
#if defined(HAS_ARGBATTENUATEROW_AVX2)
  printf("HAS_ARGBATTENUATEROW_AVX2\n");
#endif
#if defined(HAS_ARGBATTENUATEROW_NEON)
  printf("HAS_ARGBATTENUATEROW_NEON\n");
#endif
#if defined(HAS_ARGBATTENUATEROW_SME)
  printf("HAS_ARGBATTENUATEROW_SME\n");
#endif
#if defined(HAS_ARGBATTENUATEROW_SVE2)
  printf("HAS_ARGBATTENUATEROW_SVE2\n");
#endif
#if defined(HAS_ARGBUNATTENUATEROW_SSE2)
  printf("HAS_ARGBUNATTENUATEROW_SSE2\n");
#endif
#if defined(HAS_ARGBUNATTENUATEROW_AVX2)
  printf("HAS_ARGBUNATTENUATEROW_AVX2\n");
#endif
#if defined(HAS_ARGBQUANTIZEROW_SSE2)
  printf("HAS_ARGBQUANTIZEROW_SSE2\n");
#endif
#if defined(HAS_ARGBQUANTIZEROW_NEON)
  printf("HAS_ARGBQUANTIZEROW_NEON\n");
#endif
#if defined(HAS_ARGBQUANTIZEROW_SME)
  printf("HAS_ARGBQUANTIZEROW_SME\n");
#endif
#if defined(HAS_ARGBQUANTIZEROW_SVE2)
  printf("HAS_ARGBQUANTIZEROW_SVE2\n");
#endif
#if defined(HAS_COMPUTEBINS_C)
  printf("HAS_COMPUTEBINS_C\n");
#endif
#if defined(HAS_COMPUTEBINS_SSSE3)
  printf("HAS_COMPUTEBINS_SSSE3\n");
#endif
#if defined(HAS_COMPUTEBINS_AVX2)
  printf("HAS_COMPUTEBINS_AVX2\n");
#endif
#if defined(HAS_COMPUTEBINS_NEON)
  printf("HAS_COMPUTEBINS_NEON\n");
#endif
#if defined(HAS_COMPUTEBINS_SME)
  printf("HAS_COMPUTEBINS_SME\n");
#endif
#if defined(HAS_COMPUTEBINS_SVE2)
  printf("HAS_COMPUTEBINS_SVE2\n");
#endif
#if defined(HAS_ARGBSHADEROW_SSE2)
  printf("HAS_ARGBSHADEROW_SSE2\n");
#endif
#if defined(HAS_ARGBSHADEROW_NEON)
  printf("HAS_ARGBSHADEROW_NEON\n");
#endif
#if defined(HAS_ARGBSHADEROW_SME)
  printf("HAS_ARGBSHADEROW_SME\n");
#endif
#if defined(HAS_ARGBSHADEROW_SVE2)
  printf("HAS_ARGBSHADEROW_SVE2\n");
#endif
#if defined(HAS_ARGBCOLORMATRIXROW_SSSE3)
  printf("HAS_ARGBCOLORMATRIXROW_SSSE3\n");
#endif
#if defined(HAS_ARGBCOLORMATRIXROW_NEON)
  printf("HAS_ARGBCOLORMATRIXROW_NEON\n");
#endif
#if defined(HAS_ARGBCOLORMATRIXROW_SME)
  printf("HAS_ARGBCOLORMATRIXROW_SME\n");
#endif
#if defined(HAS_ARGBCOLORMATRIXROW_SVE2)
  printf("HAS_ARGBCOLORMATRIXROW_SVE2\n");
#endif
#if defined(HAS_ARGBMULTIPLYROW_SSE2)
  printf("HAS_ARGBMULTIPLYROW_SSE2\n");
#endif
#if defined(HAS_ARGBMULTIPLYROW_AVX2)
  printf("HAS_ARGBMULTIPLYROW_AVX2\n");
#endif
#if defined(HAS_ARGBMULTIPLYROW_NEON)
  printf("HAS_ARGBMULTIPLYROW_NEON\n");
#endif
#if defined(HAS_ARGBMULTIPLYROW_SME)
  printf("HAS_ARGBMULTIPLYROW_SME\n");
#endif
#if defined(HAS_ARGBMULTIPLYROW_SVE2)
  printf("HAS_ARGBMULTIPLYROW_SVE2\n");
#endif
#if defined(HAS_ARGBADDROW_SSE2)
  printf("HAS_ARGBADDROW_SSE2\n");
#endif
#if defined(HAS_ARGBADDROW_AVX2)
  printf("HAS_ARGBADDROW_AVX2\n");
#endif
#if defined(HAS_ARGBADDROW_NEON)
  printf("HAS_ARGBADDROW_NEON\n");
#endif
#if defined(HAS_ARGBADDROW_SME)
  printf("HAS_ARGBADDROW_SME\n");
#endif
#if defined(HAS_ARGBADDROW_SVE2)
  printf("HAS_ARGBADDROW_SVE2\n");
#endif
#if defined(HAS_ARGBSUBTRACTROW_SSE2)
  printf("HAS_ARGBSUBTRACTROW_SSE2\n");
#endif
#if defined(HAS_ARGBSUBTRACTROW_AVX2)
  printf("HAS_ARGBSUBTRACTROW_AVX2\n");
#endif
#if defined(HAS_ARGBSUBTRACTROW_NEON)
  printf("HAS_ARGBSUBTRACTROW_NEON\n");
#endif
#if defined(HAS_ARGBSUBTRACTROW_SME)
  printf("HAS_ARGBSUBTRACTROW_SME\n");
#endif
#if defined(HAS_ARGBSUBTRACTROW_SVE2)
  printf("HAS_ARGBSUBTRACTROW_SVE2\n");
#endif
#if defined(HAS_ARGBCOLORMATRIXROW_NEON_I8MM)
  printf("HAS_ARGBCOLORMATRIXROW_NEON_I8MM\n");
#endif
#if defined(HAS_SOBELROW_SSE2)
  printf("HAS_SOBELROW_SSE2\n");
#endif
#if defined(HAS_SOBELROW_NEON)
  printf("HAS_SOBELROW_NEON\n");
#endif
#if defined(HAS_SOBELROW_SME)
  printf("HAS_SOBELROW_SME\n");
#endif
#if defined(HAS_SOBELROW_SVE2)
  printf("HAS_SOBELROW_SVE2\n");
#endif
#if defined(HAS_SOBELTOPLANEROW_SSE2)
  printf("HAS_SOBELTOPLANEROW_SSE2\n");
#endif
#if defined(HAS_SOBELTOPLANEROW_NEON)
  printf("HAS_SOBELTOPLANEROW_NEON\n");
#endif
#if defined(HAS_SOBELTOPLANEROW_SME)
  printf("HAS_SOBELTOPLANEROW_SME\n");
#endif
#if defined(HAS_SOBELTOPLANEROW_SVE2)
  printf("HAS_SOBELTOPLANEROW_SVE2\n");
#endif
#if defined(HAS_SOBELXROW_SSE2)
  printf("HAS_SOBELXROW_SSE2\n");
#endif
#if defined(HAS_SOBELXROW_NEON)
  printf("HAS_SOBELXROW_NEON\n");
#endif
#if defined(HAS_SOBELXROW_SME)
  printf("HAS_SOBELXROW_SME\n");
#endif
#if defined(HAS_SOBELXROW_SVE2)
  printf("HAS_SOBELXROW_SVE2\n");
#endif
#if defined(HAS_SOBELYROW_SSE2)
  printf("HAS_SOBELYROW_SSE2\n");
#endif
#if defined(HAS_SOBELYROW_NEON)
  printf("HAS_SOBELYROW_NEON\n");
#endif
#if defined(HAS_SOBELYROW_SME)
  printf("HAS_SOBELYROW_SME\n");
#endif
#if defined(HAS_SOBELYROW_SVE2)
  printf("HAS_SOBELYROW_SVE2\n");
#endif
#if defined(HAS_SOBELXYROW_SSE2)
  printf("HAS_SOBELXYROW_SSE2\n");
#endif
#if defined(HAS_SOBELXYROW_NEON)
  printf("HAS_SOBELXYROW_NEON\n");
#endif
#if defined(HAS_SOBELXYROW_SME)
  printf("HAS_SOBELXYROW_SME\n");
#endif
#if defined(HAS_SOBELXYROW_SVE2)
  printf("HAS_SOBELXYROW_SVE2\n");
#endif
#if defined(HAS_ARGBSEPIAROW_SSSE3)
  printf("HAS_ARGBSEPIAROW_SSSE3\n");
#endif
#if defined(HAS_ARGBSEPIAROW_NEON)
  printf("HAS_ARGBSEPIAROW_NEON\n");
#endif
#if defined(HAS_ARGBSEPIAROW_SME)
  printf("HAS_ARGBSEPIAROW_SME\n");
#endif
#if defined(HAS_ARGBSEPIAROW_SVE2)
  printf("HAS_ARGBSEPIAROW_SVE2\n");
#endif
#if defined(HAS_ARGBSEPIAROW_NEON_DOTPROD)
  printf("HAS_ARGBSEPIAROW_NEON_DOTPROD\n");
#endif
}

}  // namespace libyuv
