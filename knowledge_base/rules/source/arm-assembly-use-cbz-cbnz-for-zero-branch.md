---
name: libyuv/source/arm-assembly-use-cbz-cbnz-for-zero-branch
description: When branching on zero or non-zero in ARM and AArch64 inline assembly routines, use cbz or cbnz instead of cmp with zero and beq or bne
rubric: safety
severity: should_fix
---

**Rule.** In ARM and AArch64 inline assembly routines, use `cbz` (Compare and
Branch on Zero) or `cbnz` (Compare and Branch on Non-Zero) instead of emitting a
separate `cmp <reg>, #0` followed by a conditional branch (`beq` or `bne`).

**Why.** `cbz` and `cbnz` combine the comparison against zero and conditional
branch into a single instruction, reducing instruction count and code size while
avoiding condition-flag updates.

## Examples

Bad:

```cpp
"cmp         %0, #0                        \n"
"beq         2f                            \n"
// ...
"cmp         %1, #0                        \n"
"bne         1b                            \n"
```

Good:

```cpp
"cbz         %0, 2f                        \n"
// ...
"cbnz        %1, 1b                        \n"
```
