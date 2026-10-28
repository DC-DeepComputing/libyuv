---
name: libyuv/no-disabled-test-sources-in-build-files
description: When modifying build configuration files, do not comment out or disable test files or targets; resolve underlying build or test failures directly
rubric: testing
severity: should_fix
---

**Rule.** Do not comment out, disable, or remove existing test source files or
test targets from build configuration files (such as `BUILD.gn`, `Android.mk`,
`Android.bp`, or `CMakeLists.txt`). Resolve underlying compilation, linker, or
test issues directly rather than disabling tests and eliminating regression
coverage.

**Why.** Commenting out test sources in build files silently disables test
execution and eliminates test coverage, allowing breakages and regressions to go
unnoticed.

## Examples

Bad:

```cpp
    sources = [
      "unit_test/basictypes_test.cc",
      "unit_test/color_test.cc",
      "unit_test/compare_test.cc",
      #      "unit_test/convert_test.cc",
      "unit_test/cpu_test.cc",
    ]
```

Good:

```cpp
    sources = [
      "unit_test/basictypes_test.cc",
      "unit_test/color_test.cc",
      "unit_test/compare_test.cc",
      "unit_test/convert_test.cc",
      "unit_test/cpu_test.cc",
    ]
```
