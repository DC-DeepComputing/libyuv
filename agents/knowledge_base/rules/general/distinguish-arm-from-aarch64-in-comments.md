---
name: libyuv/distinguish-arm-from-aarch64-in-comments
description: When commenting architecture-specific build rules, conditionals, or source configurations, accurately distinguish 32-bit Arm from 64-bit Arm (AArch64) without mislabeling targets.
rubric: readability
severity: should_fix
---

**Rule.** Ensure comments and documentation accurately describe the target CPU
architecture and do not contradict surrounding architecture checks or source
configurations. Specifically, distinguish 32-bit Arm (Arm/Armv7/AArch32) from
64-bit Arm (AArch64/Arm64) and verify that comments correctly name the
architecture and kernel files being configured.

**Why.** Mislabelling 32-bit Arm configurations as AArch64 (or vice-versa)
confuses code reviewers and maintainers, contradicts surrounding architecture
conditionals, and obscures which instruction sets or source files are actually
being compiled.

## Examples

Bad:

```cpp
if(arch_lowercase STREQUAL "arm")
  # Enable AArch64 Neon kernels.
  ADD_DEFINITIONS(-DLIBYUV_NEON=1)
endif()
```

Good:

```cpp
if(arch_lowercase STREQUAL "arm")
  # Enable Arm Neon kernels.
  ADD_DEFINITIONS(-DLIBYUV_NEON=1)
endif()
```
