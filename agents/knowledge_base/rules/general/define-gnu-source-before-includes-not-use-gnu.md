---
name: libyuv/define-gnu-source-before-includes-not-use-gnu
description: When enabling GNU libc extensions, define the _GNU_SOURCE feature test macro before system header includes instead of defining internal __USE_GNU
rubric: readability
severity: should_fix
---

**Rule.** Do not define internal glibc implementation macros like `__USE_GNU`
directly. When enabling GNU extensions (such as CPU affinity in `<sched.h>` or
`<pthread.h>`), define the standard `_GNU_SOURCE` feature test macro before any
system header `#include` directives.

**Why.** `__USE_GNU` is an internal glibc macro controlled by `<features.h>`.
Defining internal macros directly or after headers like `<stdio.h>` have already
evaluated `<features.h>` causes inconsistent declarations and header compilation
errors across standard library implementations.

## Examples

Bad:

```cpp
#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__
#define __USE_GNU
#include <pthread.h>
#include <sched.h>
#endif
```

Good:

```cpp
#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif
```
