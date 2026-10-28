---
name: libyuv/api/document-flag-values-sentinels-and-hazards
description: When documenting public APIs taking configuration flags or bitmasks, specify valid constants, describe sentinel reset values like zero, and warn of undefined behavior
rubric: readability
severity: nit
---

**Rule.** In public API documentation comments for functions that accept
configuration flags or bitmasks, specify the expected valid constants and
required combinations. Explicitly document any special sentinel values (such as
passing 0 to reset state or re-enable automatic detection), and warn callers of
safety hazards or undefined behavior if invalid or unsupported flags are
supplied.

**Why.** Callers of low-level configuration and initialization functions cannot
infer valid flag combinations or reset sentinels from integer parameter types
alone. Furthermore, enabling unsupported hardware features or passing invalid
bitmasks can lead to illegal instructions, crashes, or undefined behavior at
runtime.

## Examples

Bad:

```cpp
// Can be used in place of InitCpuFlags to force cpu_info_ to a value and bypass
// the auto init. Useful in a sandboxed process where InitCpuFlags might fail.
LIBYUV_API
void SetCpuFlags(int cpu_flags);
```

Good:

```cpp
// Sets CPU flags to |cpu_flags|, bypassing auto-detection. |cpu_flags| should
// be a valid combination of kCpuHas* constants and include kCpuInitialized.
// Notes:
// - Specifying 0 for |cpu_flags| re-enables auto-initialization.
// - Enabling CPU features not supported by the hardware results in undefined
//   behavior.
LIBYUV_API
void SetCpuFlags(int cpu_flags);
```
