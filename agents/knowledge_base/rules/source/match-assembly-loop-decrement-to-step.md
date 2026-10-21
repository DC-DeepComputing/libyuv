---
name: libyuv/source/match-assembly-loop-decrement-to-step
rubric: correctness
severity: should_fix
---

**Rule.** In SIMD and inline assembly loops, ensure the loop counter decrement
(such as subs or sub on the remaining width count) matches the exact number of
destination elements processed and stored per iteration. When unrolling loops or
processing multi-element vectors, verify that the subtracted step size
corresponds to the total store width across all vector stores in that iteration.

**Why.** If the loop counter decrement is smaller than the number of destination
elements generated and written per iteration (e.g., subtracting 8 when 16
elements are stored), the loop will run excess iterations and cause
out-of-bounds memory writes or buffer corruption.

## Examples

Bad:

```cpp
// Bad: writes 16 dst pixels via st4 {v8.4h, v9.4h, v10.4h, v11.4h} but only decrements width count by 8
"st4         {v8.4h, v9.4h, v10.4h, v11.4h}, [%0], #32 \n"
"subs        %w3, %w3, #8                              \n"
"b.gt        1b                                        \n"
```

Good:

```cpp
// Good: decrement matches the 16 dst pixels written per iteration
"st4         {v8.4h, v9.4h, v10.4h, v11.4h}, [%0], #32 \n"
"subs        %w3, %w3, #16                             \n"
"b.gt        1b                                        \n"
```
