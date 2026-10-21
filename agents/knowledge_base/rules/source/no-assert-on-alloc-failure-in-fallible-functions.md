---
name: libyuv/source/no-assert-on-alloc-failure-in-fallible-functions
rubric: correctness
severity: should_fix
---

**Rule.** In functions that return an error status code (e.g., returning
non-zero on failure), do not use assert() (such as assert(ptr) or assert(ptr !=
NULL)) to check for dynamic memory or buffer allocation failures (such as from
align_buffer_64, align_buffer_page_end, or malloc); handle failure through
regular control flow (e.g., if (!ptr) return 1;). Conversely, in internal helper
functions with a void return type where allocation failure cannot be propagated
to callers, assert on the allocated pointer (e.g., assert(row);) immediately
after allocation and before the early return check (if (!row) return;).

**Why.** Memory allocation failure is an external resource exhaustion condition
rather than a programmer invariant violation. In fallible functions designed to
return error codes, assert() inappropriately aborts debug builds and masks error
propagation testing. In void-returning helper functions where error propagation
is impossible, asserting on allocation catches out-of-memory bugs during debug
runs while the subsequent null check guarantees safe bailout in release builds.

## Examples

Bad:

```cpp
// Bad (fallible function): asserts on alloc failure instead of returning error code
int ConvertRow(const uint8_t* src, uint8_t* dst, int width) {
  align_buffer_64(row, width);
  assert(row != NULL);
  if (!row) {
    return 1;
  }
  // ...
  free_aligned_buffer_64(row);
  return 0;
}

// Bad (void function): missing assert before early return
void RotatePlane180(const uint8_t* src, int src_stride,
                    uint8_t* dst, int dst_stride, int width, int height) {
  align_buffer_64(row, width);
  if (!row) {
    return;
  }
  // Process row...
  free_aligned_buffer_64(row);
}
```

Good:

```cpp
// Good (fallible function): handles failure via regular control flow without assert
int ConvertRow(const uint8_t* src, uint8_t* dst, int width) {
  align_buffer_64(row, width);
  if (!row) {
    return 1;
  }
  // ...
  free_aligned_buffer_64(row);
  return 0;
}

// Good (void function): asserts for debug diagnostics then safely early returns
void RotatePlane180(const uint8_t* src, int src_stride,
                    uint8_t* dst, int dst_stride, int width, int height) {
  align_buffer_64(row, width);
  assert(row);
  if (!row) {
    return;
  }
  // Process row...
  free_aligned_buffer_64(row);
}
```
