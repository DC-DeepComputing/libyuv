---
name: libyuv/check-unsigned-overflow-before-addition
rubric: correctness
severity: should_fix
---

**Rule.** When computing total buffer allocation sizes or offsets by summing
multiple sizes (such as intermediate source and destination buffers), validate
addition overflow using pre-addition headroom `a > MAX - b` (e.g., `UINT64_MAX -
b` or `SIZE_MAX - b`) prior to adding, and verify that the final sum does not
exceed `SIZE_MAX`. Do not omit addition overflow checks or rely on post-addition
wrap-around comparisons (such as `a + b < a` or `sum < a`), which trigger
Undefined Behavior Sanitizer (UBSan) warnings. Furthermore, omit redundant
individual component checks (such as `a > SIZE_MAX || b > SIZE_MAX`), which are
mathematically subsumed by the headroom check and total sum bound.

**Why.** Summing large 64-bit buffer sizes without pre-addition overflow
validation can wrap around `uint64_t` back to small values, bypassing memory
limit checks and leading to undersized allocations. Performing post-addition
wrap-around checks (`a + b < a`) triggers runtime
`-fsanitize=unsigned-integer-overflow` diagnostics. Pre-addition headroom checks
(`a > MAX - b`) prevent overflow cleanly, and bounding the non-negative sum
(`sum <= SIZE_MAX`) mathematically guarantees that each individual component is
within bounds, rendering separate component checks redundant.

## Examples

Bad:

```cpp
// Bad: Omitted addition overflow check can wrap around uint64_t before SIZE_MAX.
const uint64_t total_size = src_size + dst_size;
if (total_size > SIZE_MAX) {
  return -1;
}

// Bad: Redundant individual component checks and post-addition wrap checks that trigger UBSan.
if (src_argb_size > SIZE_MAX || dst_argb_size > SIZE_MAX) {
  return -1;
}
if (src_argb_size > UINT64_MAX - dst_argb_size) {
  return -1;
}
const uint64_t argb_size = src_argb_size + dst_argb_size;
if (argb_size > SIZE_MAX || argb_size < src_argb_size) {
  return -1;
}
```

Good:

```cpp
// Good: Pre-addition headroom check prevents wrap-around and UBSan errors;
// check total sum against SIZE_MAX without redundant component or wrap checks.
if (src_argb_size > UINT64_MAX - dst_argb_size) {
  return -1;  // Addition overflow.
}
const uint64_t total_size = src_argb_size + dst_argb_size;
#if UINT64_MAX > SIZE_MAX
if (total_size > SIZE_MAX) {
  return -1;  // Exceeds size_t addressable memory.
}
#endif
```
