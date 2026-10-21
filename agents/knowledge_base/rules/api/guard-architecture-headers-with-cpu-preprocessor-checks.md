---
name: libyuv/api/guard-architecture-headers-with-cpu-preprocessor-checks
rubric: correctness
severity: blocking
---

**Rule.** When authoring or modifying architecture-specific headers or source
files—especially those applying compiler target attributes (such as #pragma
clang attribute push(__attribute__((target(...))))) or ISA intrinsics—wrap the
entire file content in top-level architecture preprocessor checks (such as #if
defined(__loongarch__) or #if defined(__aarch64__)). Ensure the file stubs out
cleanly when compiled or included on other CPU architectures.

**Why.** Compilers like Clang validate target attributes against the current
compilation target; processing attributes for unsupported instruction sets (e.g.
LoongArch LASX on ARM or x86) triggers compilation errors or incorrect code
generation. Top-level architecture guards ensure cross-platform builds can
include architecture headers safely without platform breakages.

## Examples

Bad:

```cpp
#ifndef INCLUDE_LIBYUV_LOONGSON_SET_LASX_H_
#define INCLUDE_LIBYUV_LOONGSON_SET_LASX_H_

#ifndef __loongarch_asx
#ifdef __clang__
#pragma clang attribute push(__attribute__((target("lsx,lasx"))), apply_to=function)
#endif
#endif

// ... functions using LASX ...

#endif  // INCLUDE_LIBYUV_LOONGSON_SET_LASX_H_
```

Good:

```cpp
#ifndef INCLUDE_LIBYUV_LOONGSON_SET_LASX_H_
#define INCLUDE_LIBYUV_LOONGSON_SET_LASX_H_

#if defined(__loongarch__)

#ifndef __loongarch_asx
#ifdef __clang__
#pragma clang attribute push(__attribute__((target("lsx,lasx"))), apply_to=function)
#endif
#endif

// ... functions using LASX ...

#endif  // defined(__loongarch__)
#endif  // INCLUDE_LIBYUV_LOONGSON_SET_LASX_H_
```
