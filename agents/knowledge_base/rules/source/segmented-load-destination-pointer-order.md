---
name: libyuv/source/segmented-load-destination-pointer-order
description: When calling segmented SIMD load intrinsics, pass destination vector pointers in sequential interleaved order matching even and odd element variables
rubric: readability
severity: should_fix
---

**Rule.** When invoking segmented or deinterleaving SIMD load intrinsics (such
as RISC-V `__riscv_vlseg2*` or `__riscv_vlseg4*`), pass destination pointers in
sequential interleaved stream order. Assign even or first-channel elements into
the first destination argument and odd or subsequent-channel elements into
succeeding arguments so that variable names match the loaded vector contents.

**Why.** Segmented loads unpack interleaved data sequentially into destination
registers, placing offset 0 into the first destination and offset 1 into the
second. Inverting destination pointer arguments (such as passing `&v_odd` before
`&v_even`) assigns even elements to odd variables, creating misleading variable
semantics and causing logic bugs in non-commutative operations.

## Examples

Bad:

```cpp
// Inverted arguments assign even elements into v_odd and odd into v_even.
__riscv_vlseg2e32_v_u32m4(&v_odd_32, &v_even_32, src, vl);
```

Good:

```cpp
// Sequential arguments match stream order: offset 0 (even) then offset 1 (odd).
__riscv_vlseg2e32_v_u32m4(&v_even_32, &v_odd_32, src, vl);
```
