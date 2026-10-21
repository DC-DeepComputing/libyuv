---
name: libyuv/api/guard-simd-availability-macros-with-compiler-capabilities
rubric: correctness
severity: blocking
---

**Rule.** When including SIMD intrinsic headers (such as <immintrin.h>),
declaring SIMD row kernels, or defining SIMD availability macros (such as
HAS_*), guard them with dedicated compiler capability macros (such as
VISUALC_HAS_AVX2, CLANG_HAS_AVX2, GCC_HAS_AVX2, or CLANG_HAS_SME from
cpu_support.h) in addition to CPU architecture macros (like _M_X64, __x86_64__,
or __aarch64__). Do not include intrinsic headers or declare/define SIMD kernels
under bare architecture checks alone without verifying toolchain support.
Furthermore, gate availability macros with specific target architectures (such
as defined(__aarch64__) for 64-bit Arm extensions like DotProd, I8MM, SVE, SME),
user disable overrides (!defined(LIBYUV_DISABLE_<ISA>)), and minimum compiler
versions (__clang_major__, __GNUC__, _MSC_VER) rather than bundling them
unconditionally into generic baseline SIMD blocks.

**Why.** Older compiler toolchains (such as MSVC versions prior to VS 2012 /
_MSC_VER < 1700 or older GCC/Clang releases) targeting x86 lack intrinsic
headers or support for newer instruction sets like AVX2 and AVX512, while
advanced Arm SIMD extensions require 64-bit targets and modern compiler
versions. Gating headers, declarations, and HAS_* availability macros with
compiler capability macros (VISUALC_HAS_*), architecture checks (__aarch64__),
explicit minimum compiler versions, and user disable overrides
(LIBYUV_DISABLE_*) prevents compilation failures on older toolchains and 32-bit
platforms while preserving user-level instruction set controls.

## Examples

Bad:

```cpp
// Bad: Including SIMD intrinsics header under raw architecture check alone:
#if defined(_M_X64)
#include <immintrin.h>  // AVX2 intrinsics
#endif

// Bad: Bundling AVX2 into generic SSSE3 block on x86:
#if !defined(LIBYUV_DISABLE_X86) && defined(_MSC_VER) && defined(_M_X64) && \
    (!defined(__clang__) || defined(__SSSE3__))
#define HAS_I422TOARGBROW_SSSE3
#define HAS_I422TOARGBROW_AVX2
#endif

// Bad: Bundling AArch64 DotProd into generic Neon block:
#if !defined(LIBYUV_DISABLE_NEON) && \
    (defined(__aarch64__) || defined(__ARM_NEON__) || defined(LIBYUV_NEON))
#define HAS_ABGRTOYROW_NEON
#define HAS_ABGRTOYROW_NEON_DOTPROD
#endif

// Bad: Bundling compiler check without user-override disable macro:
#if defined(LIBYUV_HAS_SME) && defined(__aarch64__)
#define HAS_TRANSPOSEWXH_SME
#endif
```

Good:

```cpp
// Good: Guard intrinsic header includes with compiler capability macro:
#if defined(_M_X64)
#if defined(VISUALC_HAS_AVX2)
#include <immintrin.h>  // AVX2 intrinsics
#endif                  // VISUALC_HAS_AVX2
#endif

// Good: Guard AVX2 with dedicated compiler capability checks:
#if !defined(LIBYUV_DISABLE_X86) && defined(VISUALC_HAS_AVX2)
#define HAS_I422TOARGBROW_AVX2
#endif

// Good: Guard AArch64-specific extensions with defined(__aarch64__):
#if !defined(LIBYUV_DISABLE_NEON) && defined(__aarch64__)
#define HAS_ABGRTOYROW_NEON_DOTPROD
#endif

// Good: Decouple compiler capability from user-override disable flag:
#if !defined(LIBYUV_DISABLE_SME) && defined(CLANG_HAS_SME) && \
    defined(__aarch64__)
#define HAS_TRANSPOSEWXH_SME
#endif
```
