---
name: libyuv/source/unroll-neon-reduction-loops-with-paired-loads-and-dual-accum
rubric: safety
severity: should_fix
---

**Rule.** In AArch64 Neon reduction and dot-product assembly loops, unroll the
loop by at least 2, use ldp for paired 128-bit vector loads, and accumulate into
multiple distinct vector registers across consecutive operations. Do not process
single 16-byte chunks with ld1 or accumulate into a single vector register
inside the loop body.

**Why.** Accumulating into a single vector register creates a loop-carried
dependency chain with instruction latency stalls on each iteration. Unrolling
the loop, issuing paired ldp loads, and interleaving accumulation across
multiple vector registers allows superscalar execution pipelines to dispatch
independent vector operations in parallel and maximize throughput.

## Examples

Bad:

```cpp
      "1:                                        \n"
      "ld1         {v0.16b}, [%0], #16           \n"
      "ld1         {v1.16b}, [%1], #16           \n"
      "subs        %w2, %w2, #16                 \n"
      "uabd        v0.16b, v0.16b, v1.16b        \n"
      "prfm        pldl1keep, [%0, 448]          \n"
      "udot        v2.4s, v0.16b, v0.16b         \n"
      "prfm        pldl1keep, [%1, 448]          \n"
      "b.gt        1b                            \n"
```

Good:

```cpp
      "1:                                        \n"
      "ldp         q0, q2, [%0], #32             \n"
      "ldp         q1, q3, [%1], #32             \n"
      "subs        %w2, %w2, #32                 \n"
      "uabd        v0.16b, v0.16b, v1.16b        \n"
      "uabd        v1.16b, v2.16b, v3.16b        \n"
      "prfm        pldl1keep, [%0, 448]          \n"
      "udot        v4.4s, v0.16b, v0.16b         \n"
      "udot        v5.4s, v1.16b, v1.16b         \n"
      "prfm        pldl1keep, [%1, 448]          \n"
      "b.gt        1b                            \n"
```
