---
name: libyuv/source/declare-all-modified-registers-in-asm-clobber-list
rubric: correctness
severity: should_fix
---

**Rule.** In GCC/Clang extended inline assembly (asm volatile), explicitly
declare every register modified by the assembly instructions in the clobber list
unless it is bound to an output operand. For SIMD architectures with overlapping
sub-registers (such as ARM NEON double registers d0–d7 composing quad registers
q0–q3, or x86 xmm/ymm registers), ensure the clobber list fully covers all
modified register ranges (e.g., list "q0" through "q3" when modifying d0–d7).
Never omit temporary, scratch, or condition-code ("cc", "memory") clobbers.

**Why.** GCC and Clang rely strictly on the clobber list to know which registers
and CPU flags are modified across an inline assembly block. Omitting modified
registers allows the compiler's register allocator to assume their values are
preserved across the asm block or reuse them for other variables, causing silent
register corruption, miscompilation, and undefined behavior.

## Examples

Bad:

```cpp
// Bad: Instructions write d0-d7 (q0-q3), but clobber list only names d0-d3
asm volatile(
    "1:                                        \n"
    "vld4.8      {d0, d2, d4, d6}, [%0]!       \n"
    "vld4.8      {d1, d3, d5, d7}, [%0]!       \n"
    "subs        %4, %4, #16                   \n"
    "vst1.8      {q0}, [%3]!                   \n"
    "vst1.8      {q1}, [%2]!                   \n"
    "vst1.8      {q2}, [%1]!                   \n"
    "vst1.8      {q3}, [%4]!                   \n"
    "bgt         1b                            \n"
    : "+r"(src_rgba), "+r"(dst_r), "+r"(dst_g), "+r"(dst_b), "+r"(width)
    :
    : "cc", "memory", "d0", "d1", "d2", "d3");  // Incomplete: d4-d7 (q2, q3) also modified
```

Good:

```cpp
// Good: Declaring q0-q3 accurately accounts for all modified registers d0-d7
asm volatile(
    "1:                                        \n"
    "vld4.8      {d0, d2, d4, d6}, [%0]!       \n"
    "vld4.8      {d1, d3, d5, d7}, [%0]!       \n"
    "subs        %4, %4, #16                   \n"
    "vst1.8      {q0}, [%3]!                   \n"
    "vst1.8      {q1}, [%2]!                   \n"
    "vst1.8      {q2}, [%1]!                   \n"
    "vst1.8      {q3}, [%4]!                   \n"
    "bgt         1b                            \n"
    : "+r"(src_rgba), "+r"(dst_r), "+r"(dst_g), "+r"(dst_b), "+r"(width)
    :
    : "cc", "memory", "q0", "q1", "q2", "q3");  // Complete: covers d0-d7
```
