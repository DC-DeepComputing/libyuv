---
name: libyuv/api/preserve-public-constant-and-flag-values
description: When adding constants, bit flags, or enums to public headers, assign new unused values and preserve existing numeric values for ABI compatibility
rubric: correctness
severity: should_fix
---

**Rule.** Do not renumber or change the numeric values of existing constants,
bit flags, or enum values in public headers. When introducing new feature flags
or constants, assign them to new, unused bit positions or integer values to
preserve binary (ABI) and backward compatibility with existing compiled
applications.

**Why.** Public constants, such as CPU architecture feature flags and format
identifiers, form part of libyuv's external ABI contract. Changing existing
numeric values or bitmasks breaks binary compatibility for applications compiled
against earlier headers or linking dynamically, causing incorrect runtime
feature detection or corrupted bitmask operations.

## Examples

Bad:

```cpp
// Existing released flags:
// kCpuHasSVE = 0x800, kCpuHasSVE2 = 0x1000, kCpuHasSME = 0x2000, kCpuHasSME2 = 0x4000
static const int kCpuHasSVE = 0x800;
static const int kCpuHasSVE2 = 0x1000;
// BAD: Inserting kCpuHasSVEF32MM here shifts existing kCpuHasSME / kCpuHasSME2 values.
static const int kCpuHasSVEF32MM = 0x2000;
static const int kCpuHasSME = 0x4000;
static const int kCpuHasSME2 = 0x8000;
```

Good:

```cpp
// Existing released flags retain their stable numeric values.
static const int kCpuHasSVE = 0x800;
static const int kCpuHasSVE2 = 0x1000;
static const int kCpuHasSME = 0x2000;
static const int kCpuHasSME2 = 0x4000;
// GOOD: Assign new flag to next available unused bitmask value.
static const int kCpuHasSVEF32MM = 0x8000;
```
