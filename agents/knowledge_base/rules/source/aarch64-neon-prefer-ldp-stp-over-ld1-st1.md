---
name: libyuv/source/aarch64-neon-prefer-ldp-stp-over-ld1-st1
description: In AArch64 Neon inline assembly loading contiguous memory without de-interleaving, prefer paired 128-bit Q-register ldp and stp over multi-D-register ld1 and st1
rubric: safety
severity: nit
---

**Rule.** In AArch64 Neon inline assembly routines, prefer paired 128-bit
Q-register loads and stores (ldp and stp) and 16-byte vector operations (e.g.,
v0.16b) over multi-D-register ld1 or st1 (with multiple 8-byte operations like
v0.8b) when accessing contiguous memory that does not require channel
de-interleaving. Reserve ld1/st1 or structured multi-vector loads (ld2/ld3/ld4)
for cases requiring channel interleaving/de-interleaving or custom strides.

**Why.** Loading and storing pairs of 128-bit Q registers using ldp and stp
minimizes instruction count, maximizes memory bus throughput, and enables full
128-bit SIMD vector arithmetic (.16b) instead of issuing multiple 64-bit (.8b)
operations.

## Examples

Bad:

```cpp
"ld1         {v0.8b,v1.8b,v2.8b,v3.8b}, [%0], #32 \n"
"ld1         {v4.8b,v5.8b,v6.8b,v7.8b}, [%1], #32 \n"
"uqadd       v0.8b, v0.8b, v4.8b                   \n"
"uqadd       v1.8b, v1.8b, v5.8b                   \n"
"uqadd       v2.8b, v2.8b, v6.8b                   \n"
"uqadd       v3.8b, v3.8b, v7.8b                   \n"
"st1         {v0.8b,v1.8b,v2.8b,v3.8b}, [%2], #32 \n"
```

Good:

```cpp
"ldp         q0, q1, [%0], #32                     \n"
"ldp         q4, q5, [%1], #32                     \n"
"uqadd       v0.16b, v0.16b, v4.16b                \n"
"uqadd       v1.16b, v1.16b, v5.16b                \n"
"stp         q0, q1, [%2], #32                     \n"
```
