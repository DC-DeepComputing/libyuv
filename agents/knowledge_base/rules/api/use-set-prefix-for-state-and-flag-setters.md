---
name: libyuv/api/use-set-prefix-for-state-and-flag-setters
description: When declaring functions that configure, set, or override library state and CPU flags, use the standard Set* prefix instead of Force*
rubric: readability
severity: should_fix
---

**Rule.** Name functions that configure, set, or override library flags,
features, or global state using the standard `Set<Property>()` convention (such
as `SetCpuFlags()` or `SetUseReferenceImpl()`), rather than imperative or
idiosyncratic verbs like `Force<Property>()`.

**Why.** Consistent `Set*` naming conforms to standard library API conventions
and clearly conveys that the function sets or modifies state (including
resetting it when passing 0 or default values), whereas `Force*` is non-standard
and implies an inflexible override.

## Examples

Bad:

```cpp
// Non-standard naming for configuration and state setters.
LIBYUV_API
void ForceCpuFlags(int cpu_flags);

void ForceUseReferenceImpl(int use);
```

Good:

```cpp
// Standard Set* convention for setting, overriding, or resetting state.
LIBYUV_API
void SetCpuFlags(int cpu_flags);

void SetUseReferenceImpl(int use);
```
