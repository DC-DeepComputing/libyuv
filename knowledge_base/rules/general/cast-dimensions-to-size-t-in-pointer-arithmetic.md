---
name: libyuv/cast-dimensions-to-size-t-in-pointer-arithmetic
description: When computing pointer offsets or buffer sizes from integer dimensions or strides, cast an operand to `size_t` to avoid 32-bit integer overflow
rubric: safety
severity: should_fix
---

**Rule.** When performing pointer arithmetic or computing buffer sizes and plane
offsets from integer dimensions, strides, or coordinates, cast at least one
multiplication operand to `size_t` (or compute intermediate offset values in
`size_t` / 64-bit integer width). Do not multiply signed 32-bit `int` dimensions
directly in pointer addition expressions.

**Why.** In C and C++, multiplying `int` operands evaluates in 32-bit signed
precision. If the product of dimensions (e.g., `width * height * bpp` or `row *
stride`) exceeds `INT_MAX`, the expression suffers signed integer overflow
(undefined behavior) and wraps before pointer addition, producing invalid
out-of-bounds pointer offsets and memory corruption.

## Examples

Bad:

```cpp
// 32-bit signed integer multiplication overflows for large dimensions.
uint8_t* dst_argb = src_argb + src_width * src_height * 4;

size_t y_size = crop_width * crop_height;
uint8_t* row_ptr = src_ptr + y * src_stride;
```

Good:

```cpp
// Casting an operand to size_t promotes arithmetic to full pointer width.
uint8_t* dst_argb = src_argb + (size_t)src_width * src_height * 4;

size_t y_size = (size_t)crop_width * crop_height;
uint8_t* row_ptr = src_ptr + (size_t)y * src_stride;
```
