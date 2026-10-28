---
name: libyuv/document-non-obvious-validation-limits
description: When introducing non-obvious validation limits, bounds checks, or magic numeric constants, document the rationale and prevented failure modes in a preceding comment
rubric: readability
severity: nit
---

**Rule.** When adding non-obvious validation limits, bounds checks, or magic
numeric constants, precede them with a comment explaining the rationale,
derivation, and specific failure modes prevented (such as integer overflows,
division by zero, hardware constraints, or buffer overreads). Do not leave
numeric thresholds unexplained or documented only in commit logs.

**Why.** Numeric validation limits and thresholds cannot be inferred from code
structure alone. Documenting the underlying invariant or failure mode directly
in source comments ensures future maintainers understand why the bounds exist
and prevents accidental loosening or regressions without needing git
archaeology.

## Examples

Bad:

```cpp
// Validation check without explaining where limits come from.
if (!src || src_width <= 0 || src_height == 0 ||
    src_width > 32768 || src_height < -32768 || src_height > 32768 ||
    !dst || dst_width <= 0 || dst_height <= 0) {
  return -1;
}
```

Good:

```cpp
// Reject dimensions larger than 32768 (or smaller than -32768 for height).
// This prevents FixedDiv signed integer overflows that can lead to division
// by zero crashes (SIGFPE on x86) or incorrect coordinate step calculations.
if (!src || src_width <= 0 || src_height == 0 ||
    src_width > 32768 || src_height < -32768 || src_height > 32768 ||
    !dst || dst_width <= 0 || dst_height <= 0) {
  return -1;
}
```
