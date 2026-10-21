---
name: libyuv/source/verify-simd-pairwise-reduction-registers
rubric: correctness
severity: blocking
---

**Rule.** When performing SIMD pairwise reductions (such as ARM NEON vpadd or
vpadal in assembly or intrinsics) to reduce vectors into a scalar or smaller
vector, verify that intra-register pairwise reduction instructions pass the
identical target register as both source operands (e.g., vpadd d0, d0, d0). Do
not reference stale, preceding, or adjacent registers in the second operand
unless explicitly combining two distinct vector halves.

**Why.** Pairwise reduction instructions add adjacent pairs within each source
operand. Passing a mismatched or adjacent register instead of the accumulator
register into an intra-vector reduction corrupts the calculation by accumulating
unintended lane values from an unrelated vector.

## Examples

Bad:

```cpp
vpadd.u32 d2, d2, d3  // Typo: adds d3 instead of intra-register reduction on d2
vadd.u32  d0, d0, d2
```

Good:

```cpp
vpadd.u32 d2, d2, d2  // Correct: adds pairwise lanes within d2
vadd.u32  d0, d0, d2
```
