---
name: libyuv/source/use-w-for-simd-working-width-variable
description: When writing SIMD row and scaling kernels, name the working variable tracking remaining width 'w' or 'width' rather than architecture-specific acronyms like 'avl'
rubric: readability
severity: nit
---

**Rule.** In SIMD row and scaling functions, name the working variable tracking
remaining width or pixel count `w` (or `width`) rather than target- or
architecture-specific acronyms such as `avl`. Maintain consistent `w` naming
across loop iterations and vector length adjustments.

**Why.** libyuv conventionally tracks remaining row width using `w` (or `width`)
across all SIMD architectures. Using architecture-specific acronyms like `avl`
(Application Vector Length) creates inconsistent naming across sibling kernels
and obscures the semantic role of the width counter.

## Examples

Bad:

```cpp
size_t avl = (size_t)width;
size_t vl = __riscv_vsetvl_e8m2(avl);
do {
  // ...
  avl -= vl;
} while (avl > 0);
```

Good:

```cpp
size_t w = (size_t)width;
size_t vl = __riscv_vsetvl_e8m2(w);
do {
  // ...
  w -= vl;
} while (w > 0);
```
