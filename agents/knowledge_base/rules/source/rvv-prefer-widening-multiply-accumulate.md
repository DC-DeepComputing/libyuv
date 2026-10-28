---
name: libyuv/source/rvv-prefer-widening-multiply-accumulate
description: When performing cross-accumulation or weighted sums in RVV vector routines, use widening multiply-accumulate (vwmaccu/vwmacc) on narrow sources and assign directly to the accumulator.
rubric: safety
severity: should_fix
---

**Rule.** In RVV vector routines (using C intrinsics or inline assembly) where
values are cross-accumulated across channels, filter taps, or adjacent pixels,
use widening multiply-accumulate (`vwmaccu`/`vwmacc` or `__riscv_vwmaccu_*`)
with narrow source vector operands rather than pre-widening inputs with `vmacc`.
Assign the result directly back into the destination accumulator variable or
register (`vd`) to avoid extra vector copies and redundant temporaries.

**Why.** Widening multiply-accumulate instructions accumulate the product of
narrow (e.g. 8-bit) source vectors directly into a widened (e.g. 16-bit)
destination register. Retaining narrow inputs allows symmetric
cross-accumulation (such as src0 + 3*src1 and src1 + 3*src0) without mutating
source data or requiring vector copy instructions, and assigning directly back
into vd matches hardware accumulator semantics.

## Examples

Bad:

```cpp
// Pre-widening inputs or using temporary variables for widening multiply-accumulate:
vuint16m8_t v_src0_u16 = __riscv_vwaddu_vx_u16m8(v_src0, 0, vl);
vuint16m8_t v_src1_u16 = __riscv_vwaddu_vx_u16m8(v_src1, 0, vl);
vuint16m8_t v_tmp0 = __riscv_vwmaccu_vv_u16m8(v_src0_u16, v_3, v_src1, vl);
vuint16m8_t v_tmp1 = __riscv_vwmaccu_vv_u16m8(v_src1_u16, v_3, v_src0, vl);
```

Good:

```cpp
// Retain narrow 8-bit source operands and assign directly back into the accumulator:
v_src0_u16 = __riscv_vwaddu_vx_u16m8(v_src0, 2, vl);
v_src1_u16 = __riscv_vwaddu_vx_u16m8(v_src1, 2, vl);
v_src0_u16 = __riscv_vwmaccu_vv_u16m8(v_src0_u16, v_3, v_src1, vl);
v_src1_u16 = __riscv_vwmaccu_vv_u16m8(v_src1_u16, v_3, v_src0, vl);
```
