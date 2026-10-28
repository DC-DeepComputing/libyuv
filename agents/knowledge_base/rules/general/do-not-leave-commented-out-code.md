---
name: libyuv/do-not-leave-commented-out-code
description: When writing or modifying source or header files, do not leave commented-out code or macro declarations; delete unused code or enable active implementations
rubric: readability
severity: should_fix
---

**Rule.** Do not leave commented-out code, statements, or macro declarations
(such as `//#define ...`) in source files. Either re-enable the declarations
once the underlying implementation is ready or delete the unused code entirely
rather than leaving dead or disabled code behind.

**Why.** Commented-out code creates ambiguity over whether functionality is
supported, clutters headers and source files, and bitrots over time as APIs
evolve. Removing unused code keeps files clean and maintainable, while version
control preserves history.

## Examples

Bad:

```cpp
// In include/libyuv/row.h:
//#define HAS_ABGRTOUVJROW_SVE2
//#define HAS_ABGRTOUVROW_SVE2

// In source/scale.cc:
// void ScalePlane_SVE2(const uint8_t* src, int src_stride, ...);
// dst_ptr = ScaleRowDown2_C(src_ptr, src_stride, dst_ptr, width);
```

Good:

```cpp
// In include/libyuv/row.h (enabled when implementation is ready):
#define HAS_ABGRTOUVJROW_SVE2
#define HAS_ABGRTOUVROW_SVE2

// In source/scale.cc (unused code is deleted):
dst_ptr = ScaleRowDown2_SVE2(src_ptr, src_stride, dst_ptr, width);
```
