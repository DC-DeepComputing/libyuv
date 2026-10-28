---
name: libyuv/source/assert-parameter-preconditions-and-bounds
description: When enforcing parameter preconditions, group assert() checks at function entry—splitting range checks into separate lower and upper assertions—rather than using comments or inline defensive assertions
rubric: readability
severity: should_fix
---

**Rule.** Enforce function parameter preconditions and valid value ranges using
runtime assert() checks grouped at function entry rather than passive comments
or inline defensive assertions. When asserting range intervals, split the check
into separate assert() statements for lower and upper bounds (e.g., assert(val
>= min); assert(val <= max);) so failures pinpoint the violated bound. Do not
sprinkle defensive assertions immediately before individual arithmetic
operations (e.g., asserting src_width != INT_MIN before negation); validate
untrusted input at public API boundaries and keep internal invariant assertions
grouped at entry.

**Why.** Comments stating preconditions cannot be programmatically verified and
easily become stale during refactoring. Inlining defensive assertions
immediately before arithmetic operations clutters implementation code and
obscures control flow. Grouping assertions at function entry clarifies
preconditions at a glance, catches out-of-range arguments in debug builds, and
separate bound checks immediately pinpoint whether an underflow or overflow
boundary was violated.

## Examples

Bad:

```cpp
// Variant 1: Passive comment instead of runtime assert with separate bounds
// Require 10 <= depth && depth <= 16
void MergeAR30Row_C(const uint16_t* src_r,
                    const uint16_t* src_g,
                    const uint16_t* src_b,
                    uint8_t* dst_ar30,
                    int depth,
                    int width) {
  // Process row...
}

// Variant 2: Defensive assertion sprinkled inline before arithmetic operation
void ScaleSlope(int src_width, int src_height, int dst_width, int dst_height) {
  // ...
  if (src_width < 0) {
    assert(src_width != INT_MIN);
    src_width = -src_width;
  }
}
```

Good:

```cpp
// Variant 1: Assertions split into separate lower and upper bound checks at entry
void MergeAR30Row_C(const uint16_t* src_r,
                    const uint16_t* src_g,
                    const uint16_t* src_b,
                    uint8_t* dst_ar30,
                    int depth,
                    int width) {
  assert(depth >= 10);
  assert(depth <= 16);
  // Process row...
}

// Variant 2: Preconditions grouped at function entry rather than inline
void ScaleSlope(int src_width, int src_height, int dst_width, int dst_height) {
  assert(src_width != 0);
  assert(src_height != 0);
  assert(dst_width > 0);
  assert(dst_height > 0);
  // ...
  if (src_width < 0) {
    src_width = -src_width;
  }
}
```
