---
name: libyuv/source/interleave-neon-compute-and-memory-instructions
rubric: safety
severity: should_fix
---

**Rule.** In ARM and AArch64 NEON inline assembly loops, interleave vector
compute and shuffle instructions (such as ushl, vshl, vadd, vsub, vmul, fmla,
tbl, trn1/trn2, zip1/zip2, uzp1/uzp2) with memory and prefetch operations (prfm,
pld, ld1-ld4, st1-st4, ldr, str, ldp, stp). Avoid placing consecutive blocks of
clustered memory or prefetch instructions back-to-back. Additionally, avoid
consecutive load instructions that post-increment the same address register
without intervening compute instructions.

**Why.** Superscalar and in-order ARM cores feature separate execution pipelines
for vector compute and memory/prefetch operations, allowing independent vector
arithmetic and load/store/prefetch instructions to dual-issue in parallel within
the same clock cycle. Grouping memory or prefetch instructions adjacently
saturates the load/store unit and causes structural execution stalls.
Furthermore, executing consecutive load instructions that post-increment the
same base register creates an address-generation and writeback dependency hazard
(AGI stall) on in-order cores; interleaving independent compute or shuffle
instructions between address-updating memory operations hides latency without
increasing instruction count.

## Examples

Bad:

```cpp
// Bad Variant 1: Clustered prefetch and store operations without interleaving compute.
"prfm        pldl1keep, [%0, 448]                  \n"
"prfm        pldl1keep, [%1, 448]                  \n"
"tbl         v0.16b, {v0.16b, v1.16b}, v29.16b     \n"
"tbl         v1.16b, {v1.16b, v2.16b}, v30.16b     \n"
"st1         {v0.16b, v1.16b}, [%[dst_ptr]], #32   \n"

// Bad Variant 2: Back-to-back loads modifying the same base register (%0) cause AGI stalls.
"ld1        {v2.4s, v3.4s}, [%0], %4               \n"
"ld1        {v4.4s, v5.4s}, [%0], %6               \n"
"fmla       v1.4s, v5.4s, v7.4s                    \n"
```

Good:

```cpp
// Good Variant 1: Interleaving vector compute/shuffles with prefetch and store ops for dual-issue.
"tbl         v0.16b, {v0.16b, v1.16b}, v29.16b     \n"
"prfm        pldl1keep, [%0, 448]                  \n"
"tbl         v1.16b, {v1.16b, v2.16b}, v30.16b     \n"
"prfm        pldl1keep, [%1, 448]                  \n"
"st1         {v0.16b, v1.16b}, [%[dst_ptr]], #32   \n"

// Good Variant 2: Interleaving independent compute between loads hides address register update latency.
"ld1        {v2.4s, v3.4s}, [%0], %4               \n"
"fmla       v1.4s, v5.4s, v7.4s                    \n"
"ld1        {v4.4s, v5.4s}, [%0], %6               \n"
```
