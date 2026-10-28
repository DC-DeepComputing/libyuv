---
name: libyuv/prefer-boolean-pointer-null-checks
description: When checking pointers or buffer allocations for nullness in conditionals and assertions, prefer implicit boolean checks over explicit comparisons to NULL or nullptr
rubric: readability
severity: should_fix
---

**Rule.** When testing pointer validity or buffer allocation results in
conditionals (such as if, while) or assertions (such as assert, DCHECK, CHECK),
evaluate the pointer directly in boolean context (e.g., if (!ptr), if (ptr),
assert(ptr)) rather than using explicit comparisons against NULL, nullptr, or 0
(e.g., ptr == NULL, ptr != nullptr, assert(row != NULL)).

**Why.** Implicit boolean evaluation for pointer validity (!ptr and ptr) is
idiomatic in Google C++ and the established convention across libyuv (including
buffer allocation helpers like align_buffer_64). It avoids verbose comparison
boilerplate and keeps null-pointer checks uniform and concise.

## Examples

Bad:

```cpp
// Bad: explicit comparisons against NULL or nullptr in conditionals and assertions
align_buffer_64(row, width);
assert(row != NULL);
if (row == NULL) {
  return 1;
}
if (src_y == nullptr || dst_y == nullptr) {
  return -1;
}
```

Good:

```cpp
// Good: evaluate pointers directly in boolean context
align_buffer_64(row, width);
assert(row);
if (!row) {
  return 1;
}
if (!src_y || !dst_y) {
  return -1;
}
```
