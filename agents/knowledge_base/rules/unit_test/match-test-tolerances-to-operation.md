---
name: libyuv/unit_test/match-test-tolerances-to-operation
description: When asserting precision or difference thresholds in unit tests, verify assertions match the specific operation's exactness rather than reusing loose or inverse tolerance macros
rubric: correctness
severity: should_fix
---

**Rule.** When testing SIMD or optimized functions against reference
implementations, assert exact equality (such as EXPECT_EQ(max_diff, 0) or
ASSERT_EQ(max_diff, 0)) for bit-exact operations. Do not reuse loose tolerance
thresholds or tolerance macros defined for lossy or inverse operations (e.g.,
using EXPECTED_UNATTENUATE_DIFF for attenuate tests).

**Why.** Different image operations have distinct precision characteristics:
bit-exact transforms guarantee zero difference, whereas lossy or inverse
operations (such as division in unattenuation) may tolerate rounding errors.
Using a loose tolerance on a bit-exact operation masks SIMD calculation bugs,
rounding errors, and regressions.

## Examples

Bad:

```cpp
// ARGBAttenuate is bit-exact, but reuses tolerance from inverse operation.
TEST_F(LibYUVPlanarTest, ARGBAttenuate_Opt) {
  int max_diff =
      TestAttenuateI(benchmark_width_, benchmark_height_, benchmark_iterations_,
                     disable_cpu_flags_, benchmark_cpu_info_, +1, 0);
  EXPECT_LE(max_diff, EXPECTED_UNATTENUATE_DIFF);
}
```

Good:

```cpp
// ARGBAttenuate is bit-exact, so diff must be exactly 0.
TEST_F(LibYUVPlanarTest, ARGBAttenuate_Opt) {
  int max_diff =
      TestAttenuateI(benchmark_width_, benchmark_height_, benchmark_iterations_,
                     disable_cpu_flags_, benchmark_cpu_info_, +1, 0);
  EXPECT_EQ(max_diff, 0);
}
```
