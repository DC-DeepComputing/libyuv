---
name: libyuv/document-workarounds-and-future-improvements-with-todo
description: When implementing workarounds, non-ideal types, or deferred improvements due to platform or compiler constraints, document the limitation with an explanatory comment or attributed TODO
rubric: readability
severity: nit
---

**Rule.** When an ideal type, design, or implementation pattern (such as atomic
variables for concurrent access or modern standard library facilities) cannot be
used due to compiler, toolchain, or platform constraints, document the
limitation and rationale in an explanatory comment or an attributed TODO.
Clearly state the intended future improvement and the condition or toolchain
upgrade needed to adopt it.

**Why.** Workarounds and fallback implementations written for legacy compiler or
platform limitations appear sub-optimal or buggy to future maintainers.
Documenting the rationale and tracking the intended improvement prevents
premature or broken refactorings and makes it easy to clean up technical debt
when platform requirements change.

## Examples

Bad:

```cpp
// Global variable accessed across threads without atomic type
LIBYUV_API int cpu_info_ = 0;
```

Good:

```cpp
// TODO(fbarchard): Consider using an atomic int for cpu_info_ once toolchain
// support is ubiquitous across all target platforms.
LIBYUV_API int cpu_info_ = 0;
```
