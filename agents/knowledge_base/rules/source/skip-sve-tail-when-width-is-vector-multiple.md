---
name: libyuv/source/skip-sve-tail-when-width-is-vector-multiple
rubric: safety
severity: should_fix
---

**Rule.** In AArch64 SVE, SVE2, and SME inline assembly kernels with
unpredicated bulk loops and predicated tails, use `subs` and branch with `b.ge`
(not `b.gt`) for the main vector loop so exact vector multiples execute in the
unpredicated path. After the main loop, restore the remaining width using
condition-setting `adds` (not `add`) and branch directly to the exit (`b.eq
99f`) to skip `whilelt` predicate generation and tail execution when the
remainder is zero.

**Why.** Branching on `b.gt` after `subs` terminates the unpredicated loop when
remaining width equals vector length (`width == vl`, subtracting to 0),
unnecessarily sending a full vector to the slower predicated tail. Using `b.ge`
keeps all full vectors in the unpredicated loop. Furthermore, when width is an
exact multiple of the vector length, restoring the remainder with `adds` and
checking `b.eq` allows bypassing `whilelt` predicate setup and masked
instructions entirely on zero remainder.

## Examples

Bad:

```cpp
"1:                                               \n"
"subs     %w[width], %w[width], %w[vl]            \n"
"st2h     {z16.h, z17.h}, p1, [%[dst_argb]]       \n"
"add      %[dst_argb], %[dst_argb], %[vl], lsl #2 \n"
"b.gt     1b                                      \n"
"2:                                               \n"
"add      %w[width], %w[width], %w[vl]            \n"
"whilelt  p1.h, wzr, %w[width]                    \n"
READYUV444_SVE I4XXTORGB_SVE RGBTOARGB8_SVE
"st2h     {z16.h, z17.h}, p1, [%[dst_argb]]       \n"
"99:                                              \n"
```

Good:

```cpp
"1:                                               \n"
"subs     %w[width], %w[width], %w[vl]            \n"
"st2h     {z16.h, z17.h}, p1, [%[dst_argb]]       \n"
"add      %[dst_argb], %[dst_argb], %[vl], lsl #2 \n"
"b.ge     1b                                      \n"
"2:                                               \n"
"adds     %w[width], %w[width], %w[vl]            \n"
"b.eq     99f                                     \n"
"whilelt  p1.h, wzr, %w[width]                    \n"
READYUV444_SVE I4XXTORGB_SVE RGBTOARGB8_SVE
"st2h     {z16.h, z17.h}, p1, [%[dst_argb]]       \n"
"99:                                              \n"
```
