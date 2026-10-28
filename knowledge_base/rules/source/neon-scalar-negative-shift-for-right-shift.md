---
name: libyuv/source/neon-scalar-negative-shift-for-right-shift
description: When implementing ARM NEON variable vector right shifts with ushl or vshl, compute negative shift amounts in scalar code instead of negating with SIMD neg
rubric: readability
severity: should_fix
---

**Rule.** In ARM NEON kernels using variable vector shifts (ushl on AArch64 or
vshl on AArch32), compute negative shift amounts directly in scalar C/C++ code
(e.g., 'shift = target_depth - src_depth' or '8 - depth') for right shifts
instead of calculating a positive offset and negating it with SIMD instructions
(neg or vneg) in inline assembly.

**Why.** ARM NEON variable shift instructions treat the vector shift amount as a
signed integer, where positive values perform a left shift and negative values
perform a right shift. Computing the negative shift count in scalar code before
passing it into assembly eliminates redundant SIMD negation instructions (neg /
vneg) outside or inside the kernel loop.

## Examples

Bad:

```cpp
// Bad: Computes positive shift and uses vector 'neg' in inline assembly.
int shift = depth - 8;
asm volatile(
    "dup         v31.8h, %w6                   \n"
    "neg         v31.8h, v31.8h                \n"
    "1:                                        \n"
    "ushl        v2.8h, v2.8h, v31.8h          \n"
    // ...
);
```

Good:

```cpp
// Good: Computes negative shift directly in scalar code.
int shift = 8 - depth;
asm volatile(
    "dup         v31.8h, %w6                   \n"
    "1:                                        \n"
    "ushl        v2.8h, v2.8h, v31.8h          \n"
    // ...
);
```
