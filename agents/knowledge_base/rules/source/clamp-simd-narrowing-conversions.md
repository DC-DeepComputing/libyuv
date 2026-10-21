---
name: libyuv/source/clamp-simd-narrowing-conversions
rubric: safety
severity: should_fix
---

**Rule.** When narrowing intermediate pixel values or converting bit depths in
SIMD row kernels (ARM NEON/SVE, x86, RISC-V RVV) and C fallbacks, use saturating
narrowing operations (e.g. vqmovn, vqmovun, vqshrun, x86 pack instructions like
packuswb/packsswb) and C clamp helpers (Clamp255) for unbounded intermediate
calculations to prevent wrap-around. Conversely, when intermediate values are
mathematically guaranteed to stay within destination range (such as box scaling,
interpolation, or averaging), avoid redundant saturating or clipping operations
(such as uqrshrnb/uqrshrn in SVE/NEON or vnclip/vnclipu/__riscv_vnclipu_* in
RVV). Instead, use standard rounding shifts (e.g. urshr) or widening additions
with rounding bias followed by narrowing logical shifts (vwaddu/__riscv_vwaddu_*
with vnsrl/__riscv_vnsrl_*), and omit unused fixed-point rounding mode CSR
writes (csrwi vxrm, ...).

**Why.** x86 pack instructions saturate while ARM NEON non-saturating
instructions truncate, so saturating narrowing ensures consistent cross-platform
clamping on out-of-range intermediate values. However, where values are
mathematically bounded (e.g. averaging four 8-bit components (4 * 255 + 2) >> 2
= 255 or weighted interpolation), saturating and clipping instructions
(uqrshrnb, vnclipu) add unnecessary execution overhead, restrict vector layouts,
and introduce unneeded fixed-point rounding mode (vxrm) dependencies.

## Examples

Bad:

```cpp
// 1. Unbounded intermediate values using truncating narrowing (wraps on NEON):
"vmovn.u16   d0, q0                        \n"

// 2. Mathematically bounded 2x2 average using redundant saturating shift in SVE:
"uqrshrnb    z0.b, " #zU0 ".h, #2          \n"

// 3. Mathematically bounded RVV interpolation using vnclipu with redundant vxrm write:
asm volatile("csrwi vxrm, 0");
vuint16m8_t v_src0_u16 = __riscv_vwaddu_vx_u16m8(v_src0, 0, vl);
v_src0_u16 = __riscv_vwmaccu_vv_u16m8(v_src0_u16, v_3, v_src1, vl);
vuint8m4_t v_dst_odd = __riscv_vnclipu_wx_u8m4(v_src0_u16, 2, vl);
```

Good:

```cpp
// 1. Saturating narrowing ensures cross-platform consistency:
"vqmovn.u16  d0, q0                        \n"

// 2. Standard rounding shift for bounded range ((4 * 255 + 2) >> 2 <= 255):
"urshr       " #zU0 ".h, p0/m, " #zU0 ".h, #2 \n"

// 3. Biased widening add and narrowing shift for bounded RVV interpolation:
vuint16m8_t v_src0_u16 = __riscv_vwaddu_vx_u16m8(v_src0, 2, vl);
v_src0_u16 = __riscv_vwmaccu_vv_u16m8(v_src0_u16, v_3, v_src1, vl);
vuint8m4_t v_dst_odd = __riscv_vnsrl_wx_u8m4(v_src0_u16, 2, vl);
```
