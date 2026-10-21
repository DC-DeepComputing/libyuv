---
name: libyuv/source/neon-use-d-registers-for-8-byte-loads
rubric: correctness
severity: blocking
---

**Rule.** In AArch64 NEON inline assembly, match vector register widths to
memory operand sizes: use 64-bit `dX` registers (e.g., `ldr d0, [%0], #8` or
`str d0, [%0], #8`) when loading or storing 8-byte chunks, and reserve 128-bit
`qX` registers (`ldr q0, [%0], #16`) for full 16-byte vector operations.

**Why.** In AArch64, vector `q` registers are 128-bit (16 bytes), whereas `d`
registers are 64-bit (8 bytes). Performing a memory load or store with `qX`
accesses 16 bytes; when handling 8-byte chunks or advancing pointers by `#8`,
using `qX` accesses 8 bytes past the allocated buffer, causing out-of-bounds
reads or memory corruption.

## Examples

Bad:

```cpp
// Bad: Loading 8-byte chunks into 128-bit qX registers reads 16 bytes (OOB read)
"ldr         q0, [%0], #8                  \n"
"ldr         q1, [%1], #8                  \n"
```

Good:

```cpp
// Good: Loading 8-byte chunks into 64-bit dX registers
"ldr         d0, [%0], #8                  \n"
"ldr         d1, [%1], #8                  \n"
```
