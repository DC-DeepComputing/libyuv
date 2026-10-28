---
name: libyuv/source/avoid-redundant-vxrm-configuration-in-rvv-kernels
description: When implementing RISC-V Vector (RVV) kernels using logical shifts like vnsrl, do not configure vxrm unless fixed-point instructions like vnclipu are executed
rubric: readability
severity: should_fix
---

**Rule.** Do not configure the RISC-V Vector fixed-point rounding mode register
(vxrm, e.g. via csrwi vxrm, ...) unless the kernel executes fixed-point
instructions that read vxrm (such as vnclip, vnclipu, vaadd, vaaddu, vsmul,
vssrl, or vssra). When narrowing or shifting using standard logical shifts (such
as vnsrl or vsrl), omit vxrm initialization because logical shift instructions
do not read or depend on the rounding mode.

**Why.** The RISC-V Vector fixed-point rounding mode CSR (vxrm) only affects
vector fixed-point instructions (vnclip, vaadd, etc.). Vector logical shifts
(like vnsrl) ignore vxrm, making CSR configuration redundant dead code that adds
instruction overhead.

## Examples

Bad:

```cpp
// Redundantly setting vxrm when using logical narrowing shift:
void ARGBToYRow_RVV(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  asm volatile("csrwi vxrm, 0");
  ...
  v_y16 = __riscv_vadd_vx_u16m4(v_y16, 255u, vl);
  v_y = __riscv_vnsrl_wx_u8m2(v_y16, 8, vl);
}
```

Good:

```cpp
// Omitting vxrm when using logical shift; setting vxrm only for fixed-point clip:
void ARGBToYRow_RVV(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  v_y16 = __riscv_vadd_vx_u16m4(v_y16, 255u, vl);
  v_y = __riscv_vnsrl_wx_u8m2(v_y16, 8, vl);
}

void ScaleRowDown2Box_RVV(const uint8_t* src_ptr, uint8_t* dst_ptr, int width) {
  asm volatile("csrwi vxrm, 0");  // Required: vnclipu reads vxrm.
  ...
  "vnclipu.wi  v16, v8, 2 \n"
}
```
