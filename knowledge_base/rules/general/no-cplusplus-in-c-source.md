---
name: libyuv/no-cplusplus-in-c-source
description: 'When adding or modifying C source (.c) files, do not use #ifdef __cplusplus guards or C++ directives, as they are dead code'
rubric: readability
severity: nit
---

**Rule.** Do not include `#ifdef __cplusplus` guards or C++-specific constructs
(such as `using namespace`, `extern "C"`, or `namespace` blocks) in C source
(`.c`) files. In C translation units, the `__cplusplus` macro is never defined,
making guarded blocks dead code and unguarded C++ syntax invalid.

**Why.** C compilers compiling `.c` source files do not define `__cplusplus`, so
any code enclosed in `#ifdef __cplusplus` is dead and will never be compiled or
tested. C++-only directives belong in shared `.h` headers or `.cc` C++
translation units instead.

## Examples

Bad:

```cpp
#include "libyuv/cpu_id.h"

#ifdef __cplusplus
using namespace libyuv;
#endif

int main(void) {
  return TestCpuFlag(kCpuHasNEON) ? 0 : 1;
}
```

Good:

```cpp
#include "libyuv/cpu_id.h"

int main(void) {
  return TestCpuFlag(kCpuHasNEON) ? 0 : 1;
}
```
