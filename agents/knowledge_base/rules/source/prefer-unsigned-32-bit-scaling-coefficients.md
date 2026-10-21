---
name: libyuv/source/prefer-unsigned-32-bit-scaling-coefficients
rubric: safety
severity: should_fix
---

**Rule.** In fixed-point scaling, interpolation, and row accumulation
calculations where intermediate products exceed INT32_MAX but fit within
UINT32_MAX, use unsigned 32-bit integer literals (such as 65536u / 9u or 65536u
/ 6u) instead of signed integer arithmetic or 64-bit types (65536LL or int64_t).
Do not introduce 64-bit arithmetic when unsigned 32-bit arithmetic avoids signed
overflow without overhead.

**Why.** In 16-bit box scaling and row accumulation, multiplying summed 16-bit
pixel samples by fixed-point scale factors can exceed INT32_MAX and cause
undefined signed integer overflow. Using unsigned 32-bit arithmetic avoids
signed overflow while preventing the significant performance penalty of 64-bit
multiplications and shifts on 32-bit CPU architectures.

## Examples

Bad:

```cpp
// Bad: Using 64-bit literal forces expensive 64-bit arithmetic on 32-bit CPUs.
dst_ptr[0] =
    (src_ptr[0] + src_ptr[1] + src_ptr[2] + src_ptr[stride + 0] +
     src_ptr[stride + 1] + src_ptr[stride + 2] + src_ptr[stride * 2 + 0] +
     src_ptr[stride * 2 + 1] + src_ptr[stride * 2 + 2]) *
        (65536LL / 9) >>
    16;
```

Good:

```cpp
// Good: Using unsigned 32-bit literals avoids signed overflow and 64-bit overhead.
dst_ptr[0] =
    (src_ptr[0] + src_ptr[1] + src_ptr[2] + src_ptr[stride + 0] +
     src_ptr[stride + 1] + src_ptr[stride + 2] + src_ptr[stride * 2 + 0] +
     src_ptr[stride * 2 + 1] + src_ptr[stride * 2 + 2]) *
        (65536u / 9u) >>
    16;
```
