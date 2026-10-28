---
name: libyuv/source/avoid-loop-invariant-branches-in-simd-row-kernels
description: When optimizing SIMD assembly kernels, avoid branching or duplicating loops to bypass zero-shift instructions; use unified loops or caller-dispatched kernels if profiling justifies
rubric: design
severity: should_fix
---

**Rule.** Do not use loop-invariant branches or duplicate loop bodies inside
SIMD assembly kernels to bypass vector shift instructions (such as ushl or vshl
for zero-shift cases). Keep a single unified loop executing dynamic shifts
unconditionally unless profiling proves a measurable throughput benefit; when
specialization for bit depths or format variations is justified, define a
dedicated row function (e.g. *Row_10_NEON) and dispatch at the caller level.

**Why.** In memory-bound SIMD kernels, executing dynamic vector shift
instructions with zero shift has negligible overhead and is hidden by memory
latency, so branching and duplicating loops increases binary size and complexity
without performance gains. Furthermore, evaluating loop-invariant branches
inside assembly kernels causes misprediction penalties; caller-level dispatch
eliminates branching entirely.

## Examples

Bad:

```cpp
// Branching inside assembly to duplicate loop without shift for zero-shift:
void MergeXR30Row_NEON(const uint16_t* src_r, const uint16_t* src_g,
                       const uint16_t* src_b, uint8_t* dst_ar30,
                       int depth, int width) {
  int shift = 10 - depth;
  asm volatile(
      "cbz         %w6, 2f                       \n"
      "dup         v31.8h, %w6                   \n"
      "1:                                        \n"
      "ldr         q2, [%0], #16                 \n"
      "ushl        v2.8h, v2.8h, v31.8h          \n"
      "b.gt        1b                            \n"
      "b           3f                            \n"
      "2:                                        \n"
      "ldr         q2, [%0], #16                 \n"  // Duplicate loop without ushl
      "3:                                        \n"
      : ...);
}
```

Good:

```cpp
// Option 1: Unified loop executing dynamic shifts unconditionally
void MergeXR30Row_NEON(const uint16_t* src_r, const uint16_t* src_g,
                       const uint16_t* src_b, uint8_t* dst_ar30,
                       int depth, int width) {
  int shift = 10 - depth;
  asm volatile(
      "dup         v31.8h, %w6                   \n"
      "1:                                        \n"
      "ldr         q2, [%0], #16                 \n"
      "ushl        v2.8h, v2.8h, v31.8h          \n"
      "b.gt        1b                            \n"
      : ...);
}

// Option 2: When profiling proves benefit, dispatch dedicated kernel at caller
void MergeXR30Row_10_NEON(const uint16_t* src_r, const uint16_t* src_g,
                          const uint16_t* src_b, uint8_t* dst_ar30,
                          int /* depth */, int width) {
  asm volatile("1: ldr q2, [%0], #16 ... b.gt 1b\n" : ...);
}
```
