---
name: libyuv/source/factor-simd-row-conversion-macros
description: When implementing SIMD row conversion kernels, factor repeated register setup, rounding modes, pixel loading, and color conversion math into shared reusable macros
rubric: design
severity: should_fix
---

**Rule.** In SIMD row conversion kernels, factor repeated register setup and
rounding mode configuration (such as YUVTORGB_SETUP, RGBTOUV_SETUP_REG, or
RISC-V Vector csrwi vxrm), pixel loading and format unpacking (such as READYUV*
and READNV*), and color conversion math (such as YUVTORGB) into reusable
architecture-specific macros instead of duplicating vector sequences or
configuration instructions across row functions. When deferring macro
unification during an initial port, add an attributed TODO comment documenting
the planned refactoring.

**Why.** Centralizing environment setup, register initialization, and vector
conversion math in shared macros eliminates redundant boilerplate, avoids
duplicating hundreds of assembly and intrinsic instructions across row
conversion routines, guarantees consistent rounding and register state across
all kernels for that architecture, prevents subtle conversion discrepancies, and
simplifies maintenance when adding new pixel formats.

## Examples

Bad:

```cpp
// Duplicating mode setup and raw vector math across row functions:
void I422ToARGBRow_RVV(const uint8_t* src_y, ...) {
  asm volatile(
      "csrwi       vxrm, 0                       \n"
      YUVTORGB_SETUP
      ...
  );
}

void I422ToARGBRow_MSA(const uint8_t* src_y, const uint8_t* src_u,
                       const uint8_t* src_v, uint8_t* dst_argb, int width) {
  for (int x = 0; x < width; x += 8) {
    v16u8 y = LD(src_y);
    v16u8 u = LD(src_u);
    v16u8 v = LD(src_v);
    reg0 = (v8i16)__msa_ilvr_b(zero, y) * vec_yg;
    CLIP_0TO255(reg0, reg1, reg2, reg3, reg4, reg5);
  }
}
```

Good:

```cpp
// Consolidating setup in setup macros and factoring load/math macros:
#define YUVTORGB_SETUP                           \
  "csrwi       vxrm, 0                       \n" \
  "lbu         %[ub], 0(%[yuvconst])         \n"

// TODO(author): Factor MSA row kernels using READYUV and YUVTORGB macros.
void I422ToARGBRow_MSA(const uint8_t* src_y, const uint8_t* src_u,
                       const uint8_t* src_v, uint8_t* dst_argb, int width) {
  YUVTORGB_SETUP(yuvconstants, vec_ubvr, vec_ugvg, vec_yg);
  for (int x = 0; x < width; x += 8) {
    READYUV422(src_y, src_u, src_v, src0, src1, src2);
    YUVTORGB(src0, src1, src2, vec_ubvr, vec_ugvg, vec_yg, out_b, out_g, out_r);
  }
}
```
