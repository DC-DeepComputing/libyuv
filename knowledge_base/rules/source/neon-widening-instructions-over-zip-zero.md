---
name: libyuv/source/neon-widening-instructions-over-zip-zero
description: When widening unsigned integer vectors in ARM Neon assembly, use dedicated widening instructions like ushll or vmovl instead of zip1 with zero registers
rubric: safety
severity: should_fix
---

**Rule.** In ARM Neon assembly, use dedicated widening and extension
instructions (such as ushll with shift #0 or uxtl/uxtl2 in AArch64, and
vmovl.u8/vmovl.u16 in AArch32) to zero-extend unsigned vectors rather than
interleaving (zip1/vzip) elements with a zero vector.

**Why.** Zero-extending via zip1 or vzip requires allocating and materializing a
scratch register containing zero and incurs unnecessary instruction and
execution overhead compared to dedicated vector widening instructions.

## Examples

Bad:

```cpp
"movi        v31.16b, #0                   \n"
"zip1        v2.16b, v0.16b, v31.16b       \n"
```

Good:

```cpp
"ushll       v2.8h, v0.8b, #0              \n"
```
