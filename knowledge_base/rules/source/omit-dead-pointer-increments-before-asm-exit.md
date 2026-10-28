---
name: libyuv/source/omit-dead-pointer-increments-before-asm-exit
description: When writing trailing loads, stores, or tail blocks in inline assembly, omit dead pointer increments and post-indexing writebacks before routine exit
rubric: readability
severity: nit
---

**Rule.** Do not include dead pointer increment instructions (e.g., explicit
add/sub or post-indexing writeback like [%ptr], #imm or [%ptr]!) at the end of
inline assembly blocks if the pointer will not be dereferenced again before
exiting the routine. On final memory operations and tail blocks, use
non-writeback addressing to avoid executing redundant arithmetic instructions.

**Why.** Advancing an input or output pointer immediately before exiting an
inline assembly routine or kernel is dead computation that adds redundant
instructions to the execution path.

## Examples

Bad:

```cpp
// Bad: Redundant src_ptr increment and dst_ptr writeback before routine exit.
"ldp   q0, q1, [%[src_ptr]]                \n"
"ldp   q2, q3, [%[src_ptr], #32]           \n"
"add   %[src_ptr], %[src_ptr], #64         \n"
"uzp2  v0.8h, v0.8h, v1.8h                 \n"
"uzp2  v1.8h, v2.8h, v3.8h                 \n"
"stp   q0, q1, [%[dst_ptr]], #32           \n"
"99:                                       \n"
```

Good:

```cpp
// Good: Omit dead pointer arithmetic and writeback on final memory operations.
"ldp   q0, q1, [%[src_ptr]]                \n"
"ldp   q2, q3, [%[src_ptr], #32]           \n"
"uzp2  v0.8h, v0.8h, v1.8h                 \n"
"uzp2  v1.8h, v2.8h, v3.8h                 \n"
"stp   q0, q1, [%[dst_ptr]]                \n"
"99:                                       \n"
```
