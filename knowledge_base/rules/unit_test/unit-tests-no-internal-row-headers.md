---
name: libyuv/unit_test/unit-tests-no-internal-row-headers
description: When writing unit tests that need buffer alignment macros such as SIMD_ALIGNED, include unit_test/unit_test.h instead of internal libyuv/row.h
rubric: design
severity: should_fix
---

**Rule.** Do not include internal row headers such as "libyuv/row.h" in unit
tests solely to obtain alignment macros or buffer definitions. Rely on
"unit_test/unit_test.h" instead, which defines SIMD_ALIGNED and test utilities
independently of internal library headers.

**Why.** libyuv/row.h is an internal header intended for library source files;
including it in general unit tests can introduce unwanted dependencies or build
breakages across target architectures and compiler configurations. Unit tests
should validate public APIs and obtain test alignment macros from
unit_test/unit_test.h.

## Examples

Bad:

```cpp
// Bad: including internal row.h in public API unit tests for SIMD_ALIGNED
#include "libyuv/row.h"

#include "../unit_test/unit_test.h"
#include "libyuv/convert.h"

TEST_F(LibYUVColorTest, TestYUV) {
  SIMD_ALIGNED(uint8_t orig_y[16]);
  // ...
}
```

Good:

```cpp
// Good: unit_test.h provides SIMD_ALIGNED for test buffers without internal headers
#include "../unit_test/unit_test.h"
#include "libyuv/convert.h"

TEST_F(LibYUVColorTest, TestYUV) {
  SIMD_ALIGNED(uint8_t orig_y[16]);
  // ...
}
```
