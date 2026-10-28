---
name: libyuv/use-nolint-for-intentional-c-casts
description: When intentional C-style casts in C++ files trigger cpplint readability/casting warnings, append // NOLINT to suppress linter presubmit failures
rubric: readability
severity: should_fix
---

**Rule.** When C-style casts (such as (int)x or (uint32_t)(a)) are intentionally
retained in C++ source or header files rather than converted to static_cast,
append // NOLINT (or /* NOLINT */ inside macros) at the end of the line. This
suppresses [readability/casting] presubmit warnings from cpplint.

**Why.** libyuv enforces cpplint during presubmit checks, which flags C-style
casts in C++ files with [readability/casting]. When C-style casts are
intentionally kept for C compatibility, intrinsics, or macros, adding // NOLINT
satisfies presubmit checks without breaking builds.

## Examples

Bad:

```cpp
xcr0 = (int)_xgetbv(0);  // VS2010 SP1 required.
```

Good:

```cpp
xcr0 = (int)_xgetbv(0);  // VS2010 SP1 required.  NOLINT
```
