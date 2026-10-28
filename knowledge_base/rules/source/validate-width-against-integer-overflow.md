---
name: libyuv/source/validate-width-against-integer-overflow
description: When computing byte strides or allocating row buffers with format multipliers, validate width against integer overflow (e.g., width > INT_MAX / 4) before allocation
rubric: safety
severity: blocking
---

**Rule.** Validate width, height, and stride inputs against integer overflow
before multiplying by bytes-per-pixel or format multipliers (e.g., verifying
width <= INT_MAX / 4 for 4-byte/ARGB formats or INT_MAX / 2 for 2-byte formats)
when computing strides or allocating row buffers with align_buffer_64. Return an
error code (such as -1 or 1) immediately if the dimension exceeds the safe
bound.

**Why.** Multiplying an unvalidated 32-bit signed integer dimension by
bytes-per-pixel (such as width * 4 for ARGB or width * 2 for 16-bit formats) can
overflow INT_MAX, causing integer wrap-around to negative values or undersized
buffer allocations that lead to out-of-bounds heap memory corruption.

## Examples

Bad:

```cpp
// Missing integer overflow check before multiplying width by bytes-per-pixel
align_buffer_64(row, width * 4);
if (!row) {
  return 1;
}
```

Good:

```cpp
// Validate width against integer overflow before allocation or stride math
if (width > INT_MAX / 4) {
  return -1;
}
align_buffer_64(row, width * 4);
if (!row) {
  return 1;
}
```
