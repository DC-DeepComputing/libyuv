---
name: libyuv/source/query-cpu-feature-registers-over-model-strings
description: When detecting runtime CPU features, query architecture feature registers or instructions like cpucfg and cpuid instead of matching CPU model strings in cpuinfo
rubric: correctness
severity: should_fix
---

**Rule.** Query CPU capability flags using architecture-level feature registers,
query instructions (such as cpucfg on LoongArch or cpuid on x86), or OS hardware
capability APIs (such as getauxval(AT_HWCAP)) rather than parsing CPU model name
strings from /proc/cpuinfo. Inspect architectural feature bits directly to
ensure new CPU models and sandboxed environments without filesystem access are
supported.

**Why.** Matching specific CPU model name strings in /proc/cpuinfo is brittle,
fails to recognize newer or alternative processor models supporting the feature
set, and fails in sandboxed environments (such as browser renderers) where
filesystem access is restricted. Direct CPU feature query instructions and OS
HWCAP APIs inspect architectural capability bits directly and work across all
environments.

## Examples

Bad:

```cpp
// BAD: Matching model names in /proc/cpuinfo fails on newer CPUs and in sandboxes.
if (strstr(cpuinfo_line, "Loongson-3A5000")) {
  flag |= kCpuHasLSX | kCpuHasLASX;
}
```

Good:

```cpp
// GOOD: Query architecture feature registers/instructions directly.
__asm__ volatile(
    "cpucfg %0, %1 \n\t"
    : "+&r"(cfg2)
    : "r"(LOONGARCH_CFG2));
if (cfg2 & LOONGARCH_CFG2_LSX)
  flag |= kCpuHasLSX;
if (cfg2 & LOONGARCH_CFG2_LASX)
  flag |= kCpuHasLASX;
```
