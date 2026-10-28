---
name: libyuv/validate-buffer-allocation-bounds-against-size-max
description: When validating buffer or plane allocation sizes prior to dynamic memory allocation, check bounds against SIZE_MAX rather than INT_MAX
rubric: correctness
severity: should_fix
---

**Rule.** When validating buffer or plane sizes prior to dynamic memory
allocation (such as new[] or malloc) or in multiplication overflow guards, check
sizes against SIZE_MAX (or SIZE_MAX / element_size) rather than INT_MAX or
INT32_MAX. For intermediate signed 64-bit size calculations, verify
non-negativity and ensure (uint64_t)size <= SIZE_MAX before allocating.

**Why.** Dynamic memory allocators (operator new[], malloc) accept allocation
sizes of type size_t, whose upper limit is SIZE_MAX. Validating allocation sizes
against INT_MAX or 32-bit signed integer limits incorrectly rejects valid
allocations on 64-bit systems while failing to represent the true architectural
memory bound.

## Examples

Bad:

```cpp
// Bad: validates allocation bound against INT_MAX instead of SIZE_MAX
int64_t databuf_size = (int64_t)scanlines_size * databuf_stride;
if (databuf_size < 0 || databuf_size > INT_MAX) {
  return LIBYUV_FALSE;
}
databuf_[i] = new uint8_t[databuf_size];

if ((size_t)width > INT_MAX / (size_t)height) {
  return -1;
}
```

Good:

```cpp
// Good: validates against SIZE_MAX for size_t allocations
int64_t databuf_size = (int64_t)scanlines_size * databuf_stride;
if (databuf_size < 0 || (uint64_t)databuf_size > SIZE_MAX) {
  return LIBYUV_FALSE;
}
databuf_[i] = new uint8_t[databuf_size];

if ((size_t)width > SIZE_MAX / (size_t)height) {
  return -1;
}
```
