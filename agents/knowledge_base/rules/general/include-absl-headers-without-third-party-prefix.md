---
name: libyuv/include-absl-headers-without-third-party-prefix
description: When including Abseil C++ headers, use quoted paths starting directly with absl/... without third_party prefixes and depend on //third_party/abseil-cpp/absl/...
rubric: readability
severity: should_fix
---

**Rule.** Include Abseil headers using quoted paths relative to the Abseil root
starting with "absl/..." (e.g., #include "absl/flags/flag.h"), rather than
prefixing with "third_party/absl/" or "third_party/abseil-cpp/". In BUILD.gn,
add the corresponding GN target under //third_party/abseil-cpp/absl/... to deps.

**Why.** The Abseil repository root is exported directly to include search paths
in GN configuration. Prefixing Abseil include paths with third_party/ breaks
include resolution and violates project include path conventions.

## Examples

Bad:

```cpp
// In unit_test.cc or other C++ files:
#include "third_party/absl/flags/flag.h"
#include "third_party/abseil-cpp/absl/flags/parse.h"
```

Good:

```cpp
// In unit_test.cc or other C++ files:
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
```
