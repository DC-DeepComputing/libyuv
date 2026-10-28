---
name: libyuv/api/no-implicit-init-flag-in-cpu-setters
description: When implementing or modifying low-level CPU flag setters or overrides, accept exact caller masks without implicitly setting kCpuInitialized so passing zero resets detection
rubric: correctness
severity: should_fix
---

**Rule.** Do not implicitly set status flags such as `kCpuInitialized` inside
low-level CPU flag setters or override helpers (e.g., `SetCpuFlags`). Require
callers to provide the full mask including `kCpuInitialized` explicitly,
preserving `0` as the reset value that re-enables automatic CPU feature
detection.

**Why.** Implicitly OR-ing initialization flags inside flag override APIs
prevents callers from passing `0` to clear cached state and re-trigger
auto-initialization, breaking unit tests and dynamic reconfiguration in
sandboxed environments.

## Examples

Bad:

```cpp
// BAD: Implicitly setting kCpuInitialized prevents passing 0 to reset state.
void SetCpuFlags(int cpu_flags) {
  cpu_info_ = cpu_flags | kCpuInitialized;
}
```

Good:

```cpp
// GOOD: Caller must explicitly supply kCpuInitialized; passing 0 resets state.
// |cpu_flags| should include kCpuHas constants and kCpuInitialized (or 0 to reset).
static __inline void SetCpuFlags(int cpu_flags) {
  LIBYUV_API extern int cpu_info_;
  cpu_info_ = cpu_flags;
}
```
