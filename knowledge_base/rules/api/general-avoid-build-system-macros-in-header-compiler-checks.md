---
name: libyuv/api/general-avoid-build-system-macros-in-header-compiler-checks
description: When checking compiler capabilities or disabling architecture features in headers, use standard compiler predefined macros instead of build-system-specific macros
rubric: correctness
severity: should_fix
---

**Rule.** Do not condition compiler capability checks or feature disabling in
headers on build-system-specific macros (such as LIBYUV_BUILD_GYP or
LIBYUV_BUILD_GN). Rely strictly on standard compiler predefined macros (such as
__clang_major__, __GNUC__, and _MSC_VER) and architecture macros (such as
__loongarch__, __aarch64__, or __riscv) so feature guards apply consistently
across all supported build environments.

**Why.** Public and internal headers are consumed across diverse build systems
(GN, CMake, Bazel, Make, and external downstream builds). Conditioning compiler
capability checks or feature disabling on a specific build system macro causes
build failures, missing optimizations, or misconfigurations on other build
systems using the same compilers.

## Examples

Bad:

```cpp
// Bad: Feature disabling is conditioned on a build-system-specific macro:
#if defined(__loongarch__) && defined(LIBYUV_BUILD_GYP) && \
    (!defined(__clang__) || __clang_major__ < 18) &&        \
    (!defined(__GNUC__) || __GNUC__ < 15)
#define LIBYUV_DISABLE_LSX
#endif
```

Good:

```cpp
// Good: Rely only on standard architecture and compiler version macros:
#if defined(__loongarch__) &&                         \
    (!defined(__clang__) || __clang_major__ < 18) && \
    (!defined(__GNUC__) || __GNUC__ < 15)
#define LIBYUV_DISABLE_LSX
#endif
```
