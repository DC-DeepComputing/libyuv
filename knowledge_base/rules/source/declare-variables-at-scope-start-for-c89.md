---
name: libyuv/source/declare-variables-at-scope-start-for-c89
description: When implementing C or extern "C" functions for C89 compatibility, declare all local variables at the beginning of the block before executable statements
rubric: readability
severity: should_fix
---

**Rule.** In C and extern "C" functions, declare all local variables at the
beginning of the enclosing block or function scope before any executable
statements or assignments. Do not mix declarations with statements or place
declarations after executable code.

**Why.** C89 standard syntax requires that variable declarations within a block
precede all executable statements. Placing declarations after statements or
assignments breaks compilation on strict C89 toolchains and legacy C compilers.

## Examples

Bad:

```cpp
while (width > 0) {
  vl = __riscv_vsetvl_e8m2(width);
  vuint8m2_t v_b, v_g, v_r;
  __riscv_vlseg3e8_v_u8m2(&v_r, &v_g, &v_b, src_raw, vl);
}
```

Good:

```cpp
while (width > 0) {
  vuint8m2_t v_b, v_g, v_r;
  vl = __riscv_vsetvl_e8m2(width);
  __riscv_vlseg3e8_v_u8m2(&v_r, &v_g, &v_b, src_raw, vl);
}
```
