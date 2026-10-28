---
name: libyuv/place-branch-specific-comments-inside-branch
description: Place branch-, platform-, or version-specific comments and TODOs inside the corresponding conditional or preprocessor branch rather than outside
rubric: readability
severity: nit
---

**Rule.** Place comments and TODOs explaining assumptions, version requirements,
preconditions, or behavior specific to a conditional or preprocessor block
inside the corresponding branch or #if guard directly adjacent to the relevant
code. Do not place branch-specific or platform-specific comments above an #if,
#elif, if, or switch construct where they misleadingly appear globally scoped or
applicable to alternative and fallback branches (#else, else, default).

**Why.** Comments placed outside a conditional or preprocessor directive
visually scope over the entire construct or file, misleading readers into
believing the documented assumption, TODO, or workaround applies globally or
across alternative and fallback paths. Scoping comments inside the relevant
branch or guard keeps context localized to the specific architecture, compiler,
or version requirements.

## Examples

Bad:

```cpp
// Assume RVV if /proc/cpuinfo is unavailable.
// This will occur for Chrome sandbox for Pepper or Render process.
#if defined(__riscv_vector)
  return kCpuHasRVV;
#else
  return 0;
#endif

// TODO: Port to RVV v0.12
#if !defined(LIBYUV_DISABLE_RVV) && defined(__riscv_vector) && \
    defined(__riscv_v_intrinsic)
// The following are available on RVV v0.11 and RVV v1.0
#if __riscv_v_intrinsic == 11000 || __riscv_v_intrinsic >= 100000
#define HAS_SCALEROWDOWN34_RVV
#endif
#endif
```

Good:

```cpp
#if defined(__riscv_vector)
  // Assume RVV if /proc/cpuinfo is unavailable.
  // This will occur for Chrome sandbox for Pepper or Render process.
  return kCpuHasRVV;
#else
  return 0;
#endif

#if !defined(LIBYUV_DISABLE_RVV) && defined(__riscv_vector) && \
    defined(__riscv_v_intrinsic)
// The following are available on RVV v0.11 and RVV v1.0
// TODO: Port to RVV v0.12
#if __riscv_v_intrinsic == 11000 || __riscv_v_intrinsic >= 100000
#define HAS_SCALEROWDOWN34_RVV
#endif
#endif
```
