---
name: libyuv/api/encapsulate-shared-state-mutation-in-setter
description: When modifying shared or global variables requiring atomic stores or synchronization, route updates through a dedicated setter helper instead of duplicating mutation code.
rubric: design
severity: should_fix
---

**Rule.** Centralize updates to shared or global state into a single dedicated
setter function rather than duplicating mutation logic (such as atomic stores,
fallback assignments, or preprocessor branches) across multiple functions. Route
all modifier and masking operations through this centralized setter.

**Why.** Consolidating state mutation into a single setter ensures consistent
atomic or fallback synchronization behavior across all update paths, prevents
duplicated logic, and simplifies future maintenance if synchronization mechanics
change.

## Examples

Bad:

```cpp
int MaskCpuFlags(int enable_flags) {
  int cpu_info = GetCpuFlags() & enable_flags;
#ifdef __ATOMIC_RELAXED
  __atomic_store_n(&cpu_info_, cpu_info, __ATOMIC_RELAXED);
#else
  cpu_info_ = cpu_info;
#endif
  return cpu_info;
}
```

Good:

```cpp
int MaskCpuFlags(int enable_flags) {
  int cpu_info = GetCpuFlags() & enable_flags;
  SetCpuFlags(cpu_info);
  return cpu_info;
}
```
