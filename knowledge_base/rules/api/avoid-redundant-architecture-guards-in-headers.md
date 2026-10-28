---
name: libyuv/api/avoid-redundant-architecture-guards-in-headers
description: When declaring cross-platform helper functions or CPU capability parsers in headers, declare them unconditionally rather than wrapping in architecture preprocessor guards
rubric: readability
severity: should_fix
---

**Rule.** Do not wrap function declarations in redundant architecture-specific
preprocessor guards (e.g. #ifdef __aarch64__) in headers when the API is exposed
across architectures or only varies by operating system. Declare cross-platform
helper functions and CPU capability parsers unconditionally across architectures
so unit tests can exercise parsing logic on any host platform.

**Why.** Redundant architecture preprocessor guards in headers add unnecessary
nesting and prevent cross-architecture unit testing. In libyuv, CPU capability
parsing functions (such as ArmCpuCaps, RiscvCpuCaps, and AArch64CpuCaps) are
declared across all architectures so tests can exercise parsing logic with mock
data on any host machine.

## Examples

Bad:

```cpp
#ifdef __aarch64__
#ifdef __linux__
LIBYUV_API
int AArch64CpuCaps(unsigned long hwcap, unsigned long hwcap2);
#else
LIBYUV_API
int AArch64CpuCaps();
#endif
#endif
```

Good:

```cpp
#ifdef __linux__
// On Linux, parse AArch64 features from getauxval(AT_HWCAP{,2}).
LIBYUV_API
int AArch64CpuCaps(unsigned long hwcap, unsigned long hwcap2);
#else
LIBYUV_API
int AArch64CpuCaps();
#endif
```
