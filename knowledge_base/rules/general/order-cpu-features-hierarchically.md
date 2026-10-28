---
name: libyuv/order-cpu-features-hierarchically
description: When defining, checking, configuring, or documenting CPU SIMD features, group by architecture family and order extensions from baseline to advanced supersets
rubric: readability
severity: should_fix
---

**Rule.** Group architecture-specific build targets, configuration flags
(libyuv_use_*), disable macros and test environment variable overrides
(LIBYUV_DISABLE_*), CPU feature constants (kCpuHas*), and diagnostic prints by
CPU architecture family (e.g., Arm, x86, MIPS, LoongArch, RISC-V). Within each
architecture family, order SIMD extensions, bitflag definitions, runtime checks
(TestCpuFlag), and documentation in progressive architectural order from
baseline or narrower vector extensions to wider, advanced supersets (e.g., LSX
before LASX, SSE2 before AVX2, NEON before SVE and SME). Maintain this
consistent ordering across header definitions (include/libyuv/cpu_id.h), CPU
probing and detection logic (source/cpu_id.cc), build configs, unit tests,
diagnostic utilities, and documentation.

**Why.** Consistent grouping by CPU architecture family and ordering by
progressive architectural hierarchy matches bitmask flag allocations, follows
CPU evolution, and ensures predictable and traceable navigation across build
configurations, header definitions, runtime detection, test environment
overrides, diagnostic logging, and documentation.

## Examples

Bad:

```cpp
// In build configurations (e.g., BUILD.gn, libyuv.gni):
if (libyuv_use_lasx) { ... }
if (libyuv_use_lsx) { ... }

// In CPU detection, environment overrides, or unit tests:
#if defined(__loongarch__) && defined(__linux__)
  if (TestEnv("LIBYUV_DISABLE_LASX")) {
    cpu_info &= ~libyuv::kCpuHasLASX;
  }
  if (TestEnv("LIBYUV_DISABLE_LSX")) {
    cpu_info &= ~libyuv::kCpuHasLSX;
  }
#endif
int has_lasx = TestCpuFlag(kCpuHasLASX);
printf("Has LASX %d\n", has_lasx);
int has_lsx = TestCpuFlag(kCpuHasLSX);
printf("Has LSX %d\n", has_lsx);
```

Good:

```cpp
// In build configurations (e.g., BUILD.gn, libyuv.gni):
if (libyuv_use_lsx) { ... }
if (libyuv_use_lasx) { ... }

// In CPU detection, environment overrides, or unit tests:
#if defined(__loongarch__) && defined(__linux__)
  if (TestEnv("LIBYUV_DISABLE_LSX")) {
    cpu_info &= ~libyuv::kCpuHasLSX;
  }
  if (TestEnv("LIBYUV_DISABLE_LASX")) {
    cpu_info &= ~libyuv::kCpuHasLASX;
  }
#endif
int has_lsx = TestCpuFlag(kCpuHasLSX);
printf("Has LSX %d\n", has_lsx);
int has_lasx = TestCpuFlag(kCpuHasLASX);
printf("Has LASX %d\n", has_lasx);
```
