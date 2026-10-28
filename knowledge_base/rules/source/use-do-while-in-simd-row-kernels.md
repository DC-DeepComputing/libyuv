---
name: libyuv/source/use-do-while-in-simd-row-kernels
description: When implementing vector or SIMD row processing kernels over pixel width, use do-while loops instead of while loops since width is guaranteed positive
rubric: safety
severity: should_fix
---

**Rule.** In SIMD, vector intrinsic, and C row processing kernels (such as
`*Row_RVV` or `*Row_C`), iterate over remaining width using `do { ... } while
(width > 0);` instead of a leading `while (width > 0)`. Higher-level libyuv
conversion and planar functions validate dimensions and guarantee that `width >
0` before invoking row kernels.

**Why.** Because higher-level callers guarantee non-empty rows, a `while (width
> 0)` loop incurs an unnecessary upfront conditional branch on loop entry. Using
a `do { ... } while (width > 0);` loop eliminates the redundant initial check,
keeping kernel entry direct and saving branch evaluation overhead on every row.

## Examples

Bad:

```cpp
void ARGBToRAWRow_RVV(const uint8_t* src_argb, uint8_t* dst_raw, int width) {
  while (width > 0) {
    vuint8m2_t v_b, v_g, v_r, v_a;
    size_t vl = __riscv_vsetvl_e8m2(width);
    __riscv_vlseg4e8_v_u8m2(&v_b, &v_g, &v_r, &v_a, src_argb, vl);
    __riscv_vsseg3e8_v_u8m2(dst_raw, v_r, v_g, v_b, vl);
    width -= vl;
    src_argb += (4 * vl);
    dst_raw += (3 * vl);
  }
}
```

Good:

```cpp
void ARGBToRAWRow_RVV(const uint8_t* src_argb, uint8_t* dst_raw, int width) {
  do {
    vuint8m2_t v_b, v_g, v_r, v_a;
    size_t vl = __riscv_vsetvl_e8m2(width);
    __riscv_vlseg4e8_v_u8m2(&v_b, &v_g, &v_r, &v_a, src_argb, vl);
    __riscv_vsseg3e8_v_u8m2(dst_raw, v_r, v_g, v_b, vl);
    width -= vl;
    src_argb += (4 * vl);
    dst_raw += (3 * vl);
  } while (width > 0);
}
```
