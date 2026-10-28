---
name: libyuv/api/inline-atomic-flag-accessors
description: When managing atomic state or CPU feature flags, implement inline accessors in the header and reuse them rather than duplicating raw atomic operations
rubric: design
severity: nit
---

**Rule.** Implement trivial atomic flag setters (such as SetCpuFlags) as inline
functions in the header alongside their corresponding atomic getters (such as
TestCpuFlag). Reuse these inline helpers across implementation files rather than
duplicating raw __atomic store and load operations or out-of-line setter calls.

**Why.** Pairing atomic getters and setters as inline header functions
centralizes atomic memory operations and fallback paths in one place, eliminates
boilerplate in .cc files, and avoids function call overhead for trivial state
updates.

## Examples

Bad:

```cpp
// In include/libyuv/cpu_id.h:
LIBYUV_API void ForceCpuFlags(int cpu_flags);

// In source/cpu_id.cc:
LIBYUV_API int MaskCpuFlags(int enable_flags) {
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
// In include/libyuv/cpu_id.h:
static __inline void SetCpuFlags(int cpu_flags) {
  LIBYUV_API extern int cpu_info_;
#ifdef __ATOMIC_RELAXED
  __atomic_store_n(&cpu_info_, cpu_flags, __ATOMIC_RELAXED);
#else
  cpu_info_ = cpu_flags;
#endif
}

// In source/cpu_id.cc:
LIBYUV_API int MaskCpuFlags(int enable_flags) {
  int cpu_info = GetCpuFlags() & enable_flags;
  SetCpuFlags(cpu_info);
  return cpu_info;
}
```
