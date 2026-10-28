---
name: libyuv/guard-optional-dependency-includes-with-feature-macros
description: When including headers from optional dependencies like Abseil or libjpeg, guard include directives with their feature macros rather than including unconditionally
rubric: correctness
severity: blocking
---

**Rule.** Guard `#include` directives for optional third-party libraries and
feature-dependent modules (such as Abseil or libjpeg) with their corresponding
build configuration preprocessor macros (e.g., `#ifdef LIBYUV_USE_ABSL_FLAGS` or
`#ifdef HAVE_JPEG`). Ensure all headers from the optional dependency—not just
the primary header—are enclosed within the conditional guard block.

**Why.** Optional dependencies are only available when their feature flag or
build dependency is enabled. Including optional headers unconditionally causes
compilation failures in minimal, standalone, or platform builds where those
third-party libraries are not present.

## Examples

Bad:

```cpp
#ifdef LIBYUV_USE_ABSL_FLAGS
#include "absl/flags/flag.h"
#endif
#include "libyuv/cpu_id.h"

// BAD: Optional header included outside the feature macro guard.
#include "absl/flags/parse.h"
```

Good:

```cpp
#include "libyuv/cpu_id.h"

// GOOD: All optional dependency headers are enclosed within the feature guard.
#ifdef LIBYUV_USE_ABSL_FLAGS
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#endif
```
