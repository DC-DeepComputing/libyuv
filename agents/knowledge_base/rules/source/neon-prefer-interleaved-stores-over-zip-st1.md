---
name: libyuv/source/neon-prefer-interleaved-stores-over-zip-st1
rubric: safety
severity: should_fix
---

**Rule.** In ARM NEON assembly routines, prefer multi-register interleaved store
instructions (`st2`, `st3`, `st4` in AArch64 or `vst2`, `vst3`, `vst4` in
AArch32) over manually shuffling vector lanes with interleaving instructions
(such as `zip1`/`zip2` or `vzip`) followed by contiguous single-register stores
(`st1`/`vst1`). Use interleaved stores whenever packing multi-channel or
alternating sample data to the destination buffer.

**Why.** Structured NEON store instructions interleave vector elements directly
during the memory write operation. Using them avoids extra vector shuffle and
permutation instructions, saving execution cycles and freeing SIMD vector
registers in performance-critical inner loops.

## Examples

Bad:

```cpp
// Manual interleaving followed by contiguous st1 stores
"zip1        v3.16b, v3.16b, v2.16b        \n"
"st1         {v3.16b}, [%1], #16           \n"
```

Good:

```cpp
// Direct interleaved store with st2
"st2         {v1.8b, v2.8b}, [%2], #16     \n"
```
