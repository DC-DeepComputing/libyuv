---
name: libyuv/source/prefer-unit-stride-vector-loads-when-channel-order-unchanged
description: When implementing vector format conversions that maintain channel order, prefer unit-stride vector loads and stores over segmented operations like vlseg and vsseg
rubric: safety
severity: should_fix
---

**Rule.** In vector implementations (such as RISC-V RVV intrinsics or assembly),
use unit-stride vector loads and stores (e.g. vle/vse) rather than segmented
loads and stores (e.g. vlseg/vsseg) when converting between formats that
preserve the same channel order. Treat all channels as contiguous elements with
unit-stride memory operations combined with vector narrowing, widening, or
arithmetic instructions.

**Why.** Segmented loads and stores deinterleave and interleave color channels
into separate vector registers, introducing significant instruction and hardware
latency overhead. When format conversions (such as 8-bit to 16-bit or vice
versa) do not swizzle or reorder channels, processing channels as contiguous
streams avoids deinterleaving overhead and maximizes memory throughput.

## Examples

Bad:

```cpp
// Deinterleaving ARGB channels with segmented operations when order is unchanged
size_t vl = __riscv_vsetvl_e16m2(avl);
__riscv_vlseg4e16_v_u16m2(&v_b_16, &v_g_16, &v_r_16, &v_a_16, src_ar64, vl);
v_b = __riscv_vnsrl_wx_u8m1(v_b_16, 8, vl);
v_g = __riscv_vnsrl_wx_u8m1(v_g_16, 8, vl);
v_r = __riscv_vnsrl_wx_u8m1(v_r_16, 8, vl);
v_a = __riscv_vnsrl_wx_u8m1(v_a_16, 8, vl);
__riscv_vsseg4e8_v_u8m1(dst_argb, v_b, v_g, v_r, v_a, vl);
```

Good:

```cpp
// Unit-stride operations treating all 4 channels as contiguous elements
size_t avl = (size_t)4 * width;
do {
  size_t vl = __riscv_vsetvl_e16m8(avl);
  vuint16m8_t v_ar64 = __riscv_vle16_v_u16m8(src_ar64, vl);
  vuint8m4_t v_argb = __riscv_vnsrl_wx_u8m4(v_ar64, 8, vl);
  __riscv_vse8_v_u8m4(dst_argb, v_argb, vl);
  avl -= vl;
  src_ar64 += vl;
  dst_argb += vl;
} while (avl > 0);
```
