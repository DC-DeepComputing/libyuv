---
name: libyuv/source/neon-widening-multiply-accumulate-over-moves
description: In ARM NEON assembly scaling or interpolation routines, use widening multiply-accumulate instructions (vmlal/umlal) instead of vector moves and 16-bit vmla
rubric: safety
severity: nit
---

**Rule.** In ARM NEON assembly routines performing weighted interpolation,
blending, or scaling on 8-bit inputs, prefer widening multiply and
multiply-accumulate instructions (`vmlal.u8`/`vmull.u8` in AArch32 or
`umlal`/`umull`/`umlal2`/`umull2` in AArch64) over vector moves (`vmov`/`vmovq`)
and 16-bit multiply-accumulate (`vmla.u16` or `mla`). Accumulate directly from
8-bit source vectors into 16-bit destination registers.

**Why.** ARM NEON widening multiply-accumulate instructions pipeline with
1-cycle throughput. Accumulating directly from 8-bit source vectors into 16-bit
accumulator registers eliminates redundant vector copy instructions and
intermediate 16-bit multiplications, reducing instruction count and register
pressure in inner loops.

## Examples

Bad:

```cpp
"vmovl.u8    q0, d0                        \n"
"vmovl.u8    q1, d2                        \n"
"vmovq       q2, q0                        \n"
"vmla.u16    q2, q1, q15                   \n"
"vmla.u16    q1, q0, q15                   \n"
```

Good:

```cpp
"vmovl.u8    q0, d4                        \n"
"vmovl.u8    q1, d5                        \n"
"vmlal.u8    q0, d5, d30                   \n"
"vmlal.u8    q1, d4, d30                   \n"
```
