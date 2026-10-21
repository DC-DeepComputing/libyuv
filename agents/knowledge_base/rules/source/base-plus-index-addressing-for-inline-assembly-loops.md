---
name: libyuv/source/base-plus-index-addressing-for-inline-assembly-loops
rubric: correctness
severity: should_fix
---

**Rule.** In x86 and x86-64 inline assembly loops operating on multiple planar
or sibling buffer pointers, precalculate relative pointer offsets before the
loop with sub/subq and access buffers using base-plus-index addressing (e.g.,
(%0,%1) or 0x00(%base,%offset,1)). Inside the loop body, advance memory pointers
by incrementing only the shared base register (via add or lea); do not increment
each buffer pointer individually, and never increment the relative offset
register.

**Why.** Incrementing multiple buffer pointers individually on every iteration
wastes instruction bandwidth and execution slots inside tight assembly loops.
Precomputing differences relative to a shared base pointer reduces loop pointer
updates to bumping just one base register. Incrementing the offset register
instead of the base register corrupts the relative offset distance between
buffers and fails to advance the base pointer across iterations.

## Examples

Bad:

```cpp
// Bad: Incrementing multiple buffer pointers individually inside the loop
"movq        (%2),%%xmm0                   \n"  // B
"movq        (%0),%%xmm1                   \n"  // R
"movq        (%1),%%xmm2                   \n"  // G
"lea         8(%0),%0                      \n"
"lea         8(%1),%1                      \n"
"lea         8(%2),%2                      \n"
"sub         $0x8,%5                       \n"
"jg          1b                            \n"

// Bad: Incrementing the offset register (%2) instead of the base register (%1)
"sub        %1,%2                          \n"
"1:                                        \n"
"vpmovzxbw  (%1),%%ymm1                    \n"
"vpmovzxbw  0x00(%1,%2,1),%%ymm2           \n"
"add        $0x10,%2                       \n"
```

Good:

```cpp
// Good: Precompute offsets relative to base (%0) and advance only the base register
"subq        %0,%1                         \n"
"subq        %0,%2                         \n"
LABELALIGN
"1:                                        \n"
"movq        (%0,%2),%%xmm0                \n"  // B
"movq        (%0),%%xmm1                   \n"  // R
"movq        (%0,%1),%%xmm2                \n"  // G
"lea         8(%0),%0                      \n"
"sub         $0x8,%5                       \n"
"jg          1b                            \n"

// Good: Advance both buffers by incrementing the shared base register (%1)
"sub        %1,%2                          \n"
"1:                                        \n"
"vpmovzxbw  (%1),%%ymm1                    \n"
"vpmovzxbw  0x00(%1,%2,1),%%ymm2           \n"
"add        $0x10,%1                       \n"
```
