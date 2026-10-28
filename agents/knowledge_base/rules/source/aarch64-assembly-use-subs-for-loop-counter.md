---
name: libyuv/source/aarch64-assembly-use-subs-for-loop-counter
description: In AArch64 and ARM inline assembly loops, use flag-setting subs after loads to decrement loop counters and evaluate branches without separate cmp
rubric: safety
severity: should_fix
---

**Rule.** In AArch64 and ARM (Neon, SVE, SME) inline assembly routines and row
loops, decrement width or iteration counters using the flag-setting subtract
instruction (`subs`, e.g., `subs %w[width], %w[width], #32` or `subs %w[width],
%w[width], %w[vl]`) rather than executing separate compare (`cmp`) and subtract
(`sub`) instructions. Furthermore, schedule the `subs` instruction early in the
loop body—specifically after initial memory loads (`ldr`, `ldp`, `ld1`) or
vector unpacks—rather than at the very top of the loop before loads or
immediately preceding the conditional branch (`b.gt`, `b.ne`, `b.lt`, `b.hi`).
Branch directly on the updated condition flags.

**Why.** Using `subs` combines counter decrementing and condition flag
calculation into a single instruction, eliminating redundant `cmp` instructions
and reducing code size. Scheduling `subs` after initial loads hides memory
latency and enables superscalar pipelines to dual-issue scalar arithmetic
alongside vector operations, while separating flag generation from conditional
branch evaluation to avoid pipeline dispatch stalls.

## Examples

Bad:

```cpp
// Bad: Redundant cmp instruction alongside sub/subs
"cmp      %w[width], %w[vl]                       \n"
"b.le     2f                                      \n"
"sub      %w[width], %w[width], %w[vl]            \n"

// Bad: Decrement at loop top before loads, or cmp/subs trailing right before branch
"1:                                               \n"
"subs     %w[count], %w[count], #16               \n"
"ldr      q0, [%[src]], #16                       \n"
"umull    v0.8h, v2.8b, v4.8b                     \n"
"cmp      %w[count], #0                           \n"
"b.gt     1b                                      \n"
```

Good:

```cpp
// Good: subs directly sets condition flags, eliminating separate cmp
"subs     %w[width], %w[width], %w[vl]            \n"
"b.lt     2f                                      \n"

// Good: subs scheduled after loads and separated from the conditional branch
"1:                                               \n"
"ldr      q0, [%[src]], #16                       \n"
"subs     %w[count], %w[count], #16               \n"
"umull    v0.8h, v2.8b, v4.8b                     \n"
"b.gt     1b                                      \n"
```
