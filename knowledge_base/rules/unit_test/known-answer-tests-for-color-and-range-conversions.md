---
name: libyuv/unit_test/known-answer-tests-for-color-and-range-conversions
description: When adding color space, pixel format, or range conversions, supplement differential tests with known-answer tests verifying boundary and midpoint values against specifications
rubric: testing
severity: should_fix
---

**Rule.** When adding or modifying color space, pixel format, or range
conversion functions, supplement randomized differential tests (such as
TESTPLANARTOP) with explicit known-answer tests (KAT). Use representative
boundary and midpoint input values (such as 0, 128, and 255 for 8-bit channels,
or 0, 512, and 1023 for 10-bit channels) and assert expected output values
against standard specification formulas rather than relying solely on
C-versus-SIMD differential equivalence.

**Why.** Differential tests only verify that optimized SIMD routines produce the
same output as the C reference implementation, but both implementations can
share identical formula, range-scaling, or rounding bugs. Known-answer tests
asserting exact outputs on fixed boundary and midpoint values verify
mathematical correctness and specification compliance across all
implementations.

## Examples

Bad:

```cpp
// Only differential testing between C and SIMD on randomized buffers:
TESTPLANARTOP(J420, uint8_t, 1, 2, 2, I420, uint8_t, 1, 2, 2, 8)
```

Good:

```cpp
// Differential test plus known-answer test (KAT) checking exact expected values:
TESTPLANARTOP(J420, uint8_t, 1, 2, 2, I420, uint8_t, 1, 2, 2, 8)

TEST_F(LibYUVConvertTest, TestJ420ToI420) {
  const uint8_t src_y[12] = {0, 0, 128, 128, 255, 255,
                             0, 0, 128, 128, 255, 255};
  const uint8_t src_u[3] = {0, 128, 255};
  const uint8_t src_v[3] = {0, 128, 255};
  uint8_t dst_y[12];
  uint8_t dst_u[3];
  uint8_t dst_v[3];
  ASSERT_EQ(0, J420ToI420(src_y, 6, src_u, 3, src_v, 3, dst_y, 6, dst_u, 3,
                          dst_v, 3, 6, 2));
  EXPECT_EQ(16, dst_y[0]);
  EXPECT_EQ(126, dst_y[2]);
  EXPECT_EQ(235, dst_y[4]);
  EXPECT_EQ(16, dst_u[0]);
  EXPECT_EQ(128, dst_u[1]);
  EXPECT_EQ(240, dst_u[2]);
}
```
