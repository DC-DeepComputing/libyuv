---
name: libyuv/source/guard-platform-cpu-detection-with-architecture-macros
rubric: correctness
severity: should_fix
---

**Rule.** When including platform-specific headers (such as <sys/sysctl.h>,
<windows.h>, or <sys/auxv.h>) or implementing architecture-specific CPU feature
detection routines (such as AArch64CpuCaps, LoongArchCpuCaps, or OS-specific
helpers like sysctlbyname or IsProcessorFeaturePresent checks) on
multi-architecture operating systems (Windows, macOS, Linux), guard the code
with both the target architecture and OS preprocessor checks (e.g., #if
defined(_WIN32) && defined(__aarch64__) or nested #if defined(__aarch64__) / #if
defined(_WIN32) blocks). Do not define architecture-specific feature detection
routines or include platform detection headers under bare OS macros without
conjoined or enclosing architecture guards.

**Why.** Multi-architecture operating systems like Windows, macOS, and Linux run
across distinct CPU architectures (such as x86, x64, AArch64, and LoongArch).
Certain architecture features (like NEON on AArch64) are mandatory or queryable
only on specific target ISAs. Guarding OS-specific feature detection or platform
headers solely on OS macros causes unintended headers to be included, causes
compilation errors or invalid invariant assumptions on unrelated architectures
targeting the same OS (such as x86 Windows), and executes architecture detection
logic on the wrong target platform.

## Examples

Bad:

```cpp
// Bad: Guarding only on the OS macro includes headers or compiles architecture-specific
// detection logic across all architectures targeting that OS (e.g., x86 on macOS or Windows).
#ifdef __APPLE__
#include <sys/sysctl.h>  // For sysctlbyname()

static int HaveAppleNeon(void) {
  int feature_present = 0;
  size_t size = sizeof(feature_present);
  return sysctlbyname("hw.optional.neon", &feature_present, &size, NULL, 0) == 0 && feature_present;
}
#endif

#ifdef _WIN32
LIBYUV_API SAFEBUFFERS int AArch64CpuCaps(void) {
  int features = kCpuHasNEON;
  if (IsProcessorFeaturePresent(PF_ARM_V82_DP_INSTRUCTIONS_AVAILABLE)) {
    features |= kCpuHasDotProd;
  }
  return features;
}
#endif  // _WIN32
```

Good:

```cpp
// Good: Guard on both the OS and the specific target CPU architecture.
#if defined(__APPLE__) && defined(__aarch64__)
#include <sys/sysctl.h>  // For sysctlbyname()

static int HaveAppleNeon(void) {
  int feature_present = 0;
  size_t size = sizeof(feature_present);
  return sysctlbyname("hw.optional.neon", &feature_present, &size, NULL, 0) == 0 && feature_present;
}
#endif

#if defined(__aarch64__) && defined(_WIN32)
LIBYUV_API SAFEBUFFERS int AArch64CpuCaps(void) {
  int features = kCpuHasNEON;
  if (IsProcessorFeaturePresent(PF_ARM_V82_DP_INSTRUCTIONS_AVAILABLE)) {
    features |= kCpuHasDotProd;
  }
  return features;
}
#endif  // defined(__aarch64__) && defined(_WIN32)
```
