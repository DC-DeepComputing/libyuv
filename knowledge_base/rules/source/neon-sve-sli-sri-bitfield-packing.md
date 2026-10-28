---
name: libyuv/source/neon-sve-sli-sri-bitfield-packing
description: In ARM NEON and SVE assembly routines packing or merging bitfields, use shift-and-insert (sli, sri) instead of separate shift and bitwise OR
rubric: safety
severity: should_fix
---

**Rule.** In ARM NEON and SVE assembly routines that pack or merge bitfields
(such as planar-to-packed RGB/ARGB pixel conversions), use shift-and-insert
instructions (sli, sri) or shift-with-accumulate (ssra, usra) instead of
separate shift (shl) and bitwise OR (orr) operations.

**Why.** Combined shift-and-insert instructions merge shift and bitwise OR into
a single instruction and avoid intermediate registers, eliminating extra
instructions and reducing register pressure in SIMD inner loops.

## Examples

Bad:

```cpp
"shl         v0.4s, v0.4s, #20             \n"  // R
"shl         v1.4s, v1.4s, #10             \n"  // G
"orr         v1.16b, v1.16b, v2.16b        \n"  // GB
"orr         v0.16b, v0.16b, v1.16b        \n"  // ARGB
```

Good:

```cpp
"sli         v2.4s, v1.4s, #10             \n"  // 00GB
"sli         v2.4s, v0.4s, #20             \n"  // 0RGB
```
