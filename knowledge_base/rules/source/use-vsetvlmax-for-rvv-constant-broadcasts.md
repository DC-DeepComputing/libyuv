---
name: libyuv/source/use-vsetvlmax-for-rvv-constant-broadcasts
description: When initializing loop-invariant constant vector registers outside RVV strip-mining loops, use vsetvlmax rather than vsetvl with runtime width to ensure full VLMAX initialization
rubric: correctness
severity: should_fix
---

**Rule.** In RISC-V Vector (RVV) kernels, use `__riscv_vsetvlmax_*()` (or
`vsetvli rd, zero, ...` in assembly) rather than `vsetvl(avl)` / `vsetvli zero,
%[avl]` when initializing or broadcasting constant vector registers and splat
masks outside strip-mining loops. Never configure vector length using runtime
width or remaining AVL when broadcasting loop-invariant constants before a loop.

**Why.** Using `vsetvl(w)` to configure vector length for pre-broadcasting a
constant across registers before a loop is non-portable: if `w < VLMAX`, upper
elements of the vector register remain uninitialized, and the RISC-V vector
specification permits implementations to return `vl < VLMAX` whenever `w < 2 *
VLMAX`. Using `vsetvlmax` guarantees that vector registers are populated up to
their full VLMAX capacity across all iterations.

## Examples

Bad:

```cpp
// Setting up loop-invariant constant with runtime width can leave upper elements uninitialized:
size_t vl = __riscv_vsetvl_e8m2(w);
vuint8m2_t v_255 = __riscv_vmv_v_x_u8m2(255u, vl);
do {
  vl = __riscv_vsetvl_e8m2(w);
  // ... loop body using v_255 ...
  w -= vl;
} while (w > 0);
```

Good:

```cpp
// Use vsetvlmax to populate the full VLMAX capacity of constant vector registers:
size_t vl = __riscv_vsetvlmax_e8m2();
vuint8m2_t v_255 = __riscv_vmv_v_x_u8m2(255u, vl);
do {
  vl = __riscv_vsetvl_e8m2(w);
  // ... loop body using v_255 ...
  w -= vl;
} while (w > 0);
```
