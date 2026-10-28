---
name: libyuv/unit_test/express-strides-in-elements-for-typed-test-macros
description: When defining unit test macros for typed buffer conversions (such as uint16_t*), express pixel sizes and stride alignments in elements per pixel rather than bytes
rubric: readability
severity: nit
---

**Rule.** In unit test macros and helpers testing conversion functions that
operate on typed buffers (such as uint16_t*), express pixel sizes, buffer
widths, and stride alignments in elements per pixel (EPP) rather than bytes per
pixel (BPP). Pass element-based strides directly to libyuv APIs without dividing
or multiplying by sizeof(TYPE).

**Why.** In libyuv, conversion functions operating on typed pointers (such as
const uint16_t*) accept stride parameters measured in element counts rather than
byte counts. Defining test macros and stride alignments in elements per pixel
aligns with API parameter expectations and avoids unnecessary or error-prone
byte-to-element conversions.

## Examples

Bad:

```cpp
// Bad: Using bytes per pixel (BPP) for typed buffers and dividing stride by element size.
#define TESTATOB(FMT_A, TYPE_A, BPP_A, STRIDE_A, FMT_B, TYPE_B, BPP_B, STRIDE_B) \
  const int kStrideA = (kWidth * BPP_A + STRIDE_A - 1) / STRIDE_A * STRIDE_A;     \
  FMT_A##To##FMT_B((TYPE_A*)src, kStrideA / sizeof(TYPE_A),                        \
                   (TYPE_B*)dst, kStrideB / sizeof(TYPE_B), kWidth, kHeight);

TESTATOB(AR64, uint16_t, 8, 8, AB64, uint16_t, 8, 8)
```

Good:

```cpp
// Good: Using elements per pixel (EPP) to match element-based strides in typed APIs.
#define TESTATOB(FMT_A, TYPE_A, EPP_A, STRIDE_A, FMT_B, TYPE_B, EPP_B, STRIDE_B) \
  const int kStrideA = (kWidth * EPP_A + STRIDE_A - 1) / STRIDE_A * STRIDE_A;     \
  FMT_A##To##FMT_B((TYPE_A*)src, kStrideA, (TYPE_B*)dst, kStrideB, kWidth, kHeight);

TESTATOB(AR64, uint16_t, 4, 4, AB64, uint16_t, 4, 4)
```
