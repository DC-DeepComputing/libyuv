---
name: libyuv/source/document-multi-stage-loop-remainder-and-tail-calculations
description: When writing multi-stage loops with SIMD-aligned remainder blocks and scalar fallback tails, add comments explaining each remainder calculation and alignment step
rubric: readability
severity: nit
---

**Rule.** Precede multi-stage loop remainder and tail calculations with comments
clarifying the chunk size or alignment mask for each stage (e.g., distinguishing
SIMD-aligned remainder blocks from unaligned scalar C fallback iterations).

**Why.** In vectorized processing routines, bitwise remainder masks (such as `&
~(kSimdSize - 1)` vs. `& (kSimdSize - 1)`) and multi-stage block sizes can be
difficult to interpret; comments document the purpose, alignment, and fallback
strategy of each stage.

## Examples

Bad:

```cpp
remainder = (count - i) & ~(kSimdSize - 1);
if (remainder) {
  diff += HammingDistance(src_a + i, src_b + i, remainder);
  i += remainder;
}
remainder = (count - i);
if (remainder) {
  diff += HammingDistance_C(src_a + i, src_b + i, remainder);
}
```

Good:

```cpp
// SIMD for multiple of kSimdSize (64).
remainder = (count - i) & ~(kSimdSize - 1);
if (remainder) {
  diff += HammingDistance(src_a + i, src_b + i, remainder);
  i += remainder;
}
// Scalar C fallback for remaining unaligned bytes.
remainder = (count - i);
if (remainder) {
  diff += HammingDistance_C(src_a + i, src_b + i, remainder);
}
```
