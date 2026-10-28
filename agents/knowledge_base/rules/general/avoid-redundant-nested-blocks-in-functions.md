---
name: libyuv/avoid-redundant-nested-blocks-in-functions
description: When structuring function logic, avoid enclosing statements in redundant nested compound blocks unless required for RAII lifetime scoping or name disambiguation
rubric: readability
severity: nit
---

**Rule.** Avoid enclosing function logic, local variable declarations, or
temporary buffer allocations in redundant nested `{ ... }` blocks when an
isolated scope is not required. Declare variables and buffers inline in the
function body unless a nested scope is necessary for RAII lifetime management or
name disambiguation.

**Why.** Artificial compound statement blocks introduce unnecessary indentation
and visual clutter. In C++, local variables can be declared inline where needed,
so nested blocks should only be used when an object's destructor must run at a
specific scope boundary or to avoid variable name collisions.

## Examples

Bad:

```cpp
int ConvertRow(const uint8_t* src, uint8_t* dst, int width) {
  // Setup and CPU detection...
  {
    align_buffer_64(row_u, ((width + 31) & ~31) * 2);
    ProcessRow(src, row_u, width);
    free_aligned_buffer_64(row_u);
  }
  return 0;
}
```

Good:

```cpp
int ConvertRow(const uint8_t* src, uint8_t* dst, int width) {
  // Setup and CPU detection...
  align_buffer_64(row_u, ((width + 31) & ~31) * 2);
  ProcessRow(src, row_u, width);
  free_aligned_buffer_64(row_u);
  return 0;
}
```
