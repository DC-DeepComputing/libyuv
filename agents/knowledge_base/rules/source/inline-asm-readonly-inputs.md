---
name: libyuv/source/inline-asm-readonly-inputs
description: When writing GCC or Clang extended inline assembly, place read-only parameters and constants in the input operands section rather than read-write outputs
rubric: correctness
severity: should_fix
---

**Rule.** In GCC and Clang extended inline assembly (asm volatile), place
read-only variables and constants (such as shift amounts, masks, lookup tables,
bit depths, or fixed constants) in the input operands section (after the second
colon :) using input constraints like "r", "m", or "rm". Reserve output operands
(= or +) strictly for variables whose storage is mutated by the inline assembly
instructions.

**Why.** Declaring read-only variables as read-write outputs (+r or +m) falsely
informs the compiler that their values are mutated by the assembly block. This
violates GCC inline assembly semantics, forces unnecessary register moves or
spills, disables compiler optimizations (such as common subexpression
elimination and constant propagation), and misleads readers about variable
mutation.

## Examples

Bad:

```cpp
// Bad: read-only parameters (shift, MergeAR64Permute) declared as read-write outputs (+)
asm volatile(
    "dup         v31.4s, %w3                   \n"
    "1:                                        \n"
    "subs        %w2, %w2, #1                  \n"
    "b.gt        1b                            \n"
    : "+r"(src_r),     // %0
      "+r"(dst_ar30),  // %1
      "+r"(width),     // %2
      "+r"(shift)      // %3: read-only shift declared as read-write output
    : "m"(MergeAR64Permute)
    : "memory", "cc", "v0", "v1", "v31");
```

Good:

```cpp
// Good: read-only parameters passed in input operands list (: "r", "m", "rm")
asm volatile(
    "dup         v31.4s, %w3                   \n"
    "1:                                        \n"
    "subs        %w2, %w2, #1                  \n"
    "b.gt        1b                            \n"
    : "+r"(src_r),     // %0
      "+r"(dst_ar30),  // %1
      "+r"(width)      // %2
    : "r"(shift),      // %3: read-only input
      "m"(MergeAR64Permute)
    : "memory", "cc", "v0", "v1", "v31");
```
