---
name: libyuv/record-speculative-optimizations-in-todo
description: When considering speculative optimizations or alternative implementations requiring complex benchmarking or refactoring, record the hypothesis in an attributed TODO comment instead of blocking the change
rubric: readability
severity: nit
---

**Rule.** When exploring speculative optimizations or architectural alternatives
that require extensive refactoring, vectorization, or performance benchmarking
to validate, record the optimization idea in an attributed TODO comment (or
tracking bug). Do not hold up or overcomplicate targeted changes with unverified
optimization hypotheses when the current implementation is correct.

**Why.** Speculative algorithmic or memory access optimizations (such as
streaming single-row writes versus multi-row writes) require substantial
restructuring and multi-platform performance verification. Documenting the
hypothesis in an attributed TODO preserves the optimization idea for future
benchmarking without delaying or destabilizing the immediate change.

## Examples

Bad:

```cpp
// Writing two rows at a time without recording the single-row optimization alternative
for (x = 0; x < src_height - 1; ++x) {
  Scale2RowUp(src_ptr, src_stride, dst_ptr, dst_stride, dst_width);
  src_ptr += src_stride;
  dst_ptr += 2 * dst_stride;
}
```

Good:

```cpp
for (x = 0; x < src_height - 1; ++x) {
  Scale2RowUp(src_ptr, src_stride, dst_ptr, dst_stride, dst_width);
  src_ptr += src_stride;
  // TODO(username): Test performance of writing one row of destination at a time.
  dst_ptr += 2 * dst_stride;
}
```
