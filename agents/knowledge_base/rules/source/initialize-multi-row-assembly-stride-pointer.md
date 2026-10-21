---
name: libyuv/source/initialize-multi-row-assembly-stride-pointer
rubric: correctness
severity: blocking
---

**Rule.** In multi-row SIMD assembly kernels (such as box filter or bilinear
scaling functions) that receive a base source pointer and row stride in
registers, initialize subsequent row pointers by adding the base pointer to the
stride before entering the load loop. Never dereference the raw stride register
directly in memory load instructions.

**Why.** Omitting the pre-loop pointer calculation causes load instructions
indexing the second row register to dereference the raw stride offset as an
absolute memory address. This leads to silent data corruption or an immediate
segmentation fault.

## Examples

Bad:

```cpp
void ScaleRowDown2Box_NEON(const uint8_t* src_ptr,
                           ptrdiff_t src_stride,
                           uint8_t* dst_ptr,
                           int dst_width) {
  asm volatile(
      // BAD: Dereferences raw stride offset (%1) without adding src_ptr (%0)
      "1:                                        \n"
      "ld2        {v0.8h, v1.8h}, [%0], #32      \n"  // row 1
      "ld2        {v2.8h, v3.8h}, [%1], #32      \n"  // row 2 (crashes!)
      : "+r"(src_ptr), "+r"(src_stride), "+r"(dst_ptr), "+r"(dst_width)
      :
      : "memory", "cc", "v0", "v1", "v2", "v3");
}
```

Good:

```cpp
void ScaleRowDown2Box_NEON(const uint8_t* src_ptr,
                           ptrdiff_t src_stride,
                           uint8_t* dst_ptr,
                           int dst_width) {
  asm volatile(
      // GOOD: Add base pointer (%0) to stride (%1) to form row 2 pointer
      "add        %1, %1, %0                     \n"
      "1:                                        \n"
      "ld2        {v0.8h, v1.8h}, [%0], #32      \n"  // row 1
      "ld2        {v2.8h, v3.8h}, [%1], #32      \n"  // row 2
      : "+r"(src_ptr), "+r"(src_stride), "+r"(dst_ptr), "+r"(dst_width)
      :
      : "memory", "cc", "v0", "v1", "v2", "v3");
}
```
