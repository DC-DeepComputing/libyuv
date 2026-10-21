---
name: libyuv/source/aarch64-neon-ld1-register-post-index-stride
rubric: safety
severity: blocking
---

**Rule.** In AArch64 NEON inline assembly loops or routines where the consumed
step size or custom stride differs from the total loaded vector register width
(such as overlapping sliding-window convolution or multi-tap filter operations),
use multi-register ld1 with register post-indexing (passing the step size in a
general-purpose register operand, e.g., ld1 {v0.8h}, [%0], %<reg>) rather than
fixed immediate increments (#<bytes>), separate unaligned loads (ldur), or
manual pointer arithmetic.

**Why.** An immediate post-index offset in AArch64 SIMD loads (ld1 {v0.8h},
[%0], #16) requires the immediate value to exactly match the total byte width of
the loaded register list. Advancing by the immediate vector width when consuming
fewer elements per iteration skips input samples, while issuing trailing
unaligned loads (ldur) increases instruction count and incurs load latency
penalties. Passing a custom stride in a register operand allows loading
contiguous elements across up to four vector registers in a single instruction
while advancing the pointer by the exact consumed element count.

## Examples

Bad:

```cpp
// Bad Variant 1: Immediate post-index advances by loaded size (16B), skipping samples when only 8B are consumed.
"1: \n"
"ld1        {v0.8h}, [%0], #16                 \n"
"subs       %w2, %w2, #4                       \n"
"b.gt       1b                                 \n"

// Bad Variant 2: Loading vector then issuing extra unaligned load for overlapping tap.
"ld1        {v2.4s, v3.4s}, [%0], #32          \n"
"ldur       q20, [%0, #-12]                    \n"
```

Good:

```cpp
// Good Variant 1: Register post-index advances pointer by exact consumed step (%4 = 8LL).
"1: \n"
"ld1        {v0.8h}, [%0], %4                  \n"
"subs       %w2, %w2, #4                       \n"
"b.gt       1b                                 \n"
: "+r"(src), "+r"(dst), "+r"(width) : "r"(coeffs), "r"(8LL)

// Good Variant 2: Multi-register ld1 loads all taps (12 samples) and advances by stride (%6 = 32LL).
"ld1        {v1.4s, v2.4s, v3.4s}, [%0], %6    \n"
: "+r"(src), ... : "r"(32LL)
```
