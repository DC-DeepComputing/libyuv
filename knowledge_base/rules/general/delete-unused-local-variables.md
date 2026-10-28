---
name: libyuv/delete-unused-local-variables
description: When implementing or refactoring functions and SIMD kernels, delete unused local variables, leftover constants, and dead intermediate computations
rubric: readability
severity: nit
---

**Rule.** Do not declare or leave unused local variables, dead intermediate
calculations, or leftover variables from copy-pasting or refactoring functions
and SIMD kernels. Ensure all declared variables and computed intermediate values
are actively read, passed to downstream operations, or referenced in inline
assembly operand constraints; otherwise, delete them completely.

**Why.** Unused local variables and dead intermediate calculations clutter
function implementations, mislead readers into thinking values contribute to
calculations or control flow, obscure actual inputs used by SIMD kernels or
inline assembly, and can mask incomplete refactoring or copy-paste errors.

## Examples

Bad:

```cpp
__arm_locally_streaming static void HalfRow_16_SME(uint16_t* dst_ptr,
                                                   const uint16_t* src_ptr,
                                                   ptrdiff_t src_stride,
                                                   int width) {
  int y1_fraction = 128;
  int y0_fraction = 256 - y1_fraction;
  const uint16_t* src_ptr1 = src_ptr + src_stride;
  // y0_fraction and y1_fraction are never referenced in downstream logic or asm.
  asm volatile(
      "ld1h {z0.h}, p0/z, [%[src_ptr]] \n"
      : [src_ptr] "+r"(src_ptr)
      :
      : "z0");
}
```

Good:

```cpp
__arm_locally_streaming static void HalfRow_16_SME(uint16_t* dst_ptr,
                                                   const uint16_t* src_ptr,
                                                   ptrdiff_t src_stride,
                                                   int width) {
  const uint16_t* src_ptr1 = src_ptr + src_stride;
  asm volatile(
      "ld1h {z0.h}, p0/z, [%[src_ptr]] \n"
      : [src_ptr] "+r"(src_ptr)
      :
      : "z0");
}
```
