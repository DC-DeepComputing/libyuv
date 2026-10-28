---
name: libyuv/api/sync-readme-chromium-version
description: When adding public API functions or bumping LIBYUV_VERSION in include/libyuv/version.h, increment the version and keep README.chromium synchronized
rubric: correctness
severity: should_fix
---

**Rule.** When adding a new public API function or capability (marked with
LIBYUV_API) in include/libyuv/, bump LIBYUV_VERSION in include/libyuv/version.h
and update the matching Version: field in README.chromium. Whenever incrementing
or updating LIBYUV_VERSION, always keep the version numbers identical across
both files in the same change.

**Why.** Libyuv tracks its release version in include/libyuv/version.h for
compiled code and API consumers, and in README.chromium for third-party
packaging and dependency tracking. Bumping the version when adding new public
APIs allows downstream consumers to detect feature availability, and keeping
both files synchronized prevents tooling from misidentifying the library
version.

## Examples

Bad:

```cpp
// 1. Adding a new public API without updating version.h and README.chromium:
// include/libyuv/convert.h
LIBYUV_API
int RAWToI444(const uint8_t* src_raw, ...);

// 2. Bumping LIBYUV_VERSION without updating README.chromium:
// include/libyuv/version.h
-#define LIBYUV_VERSION 1902
+#define LIBYUV_VERSION 1903

// README.chromium (left unchanged)
Version: 1902
```

Good:

```cpp
// 1. Adding a new public API and updating both version files:
// include/libyuv/convert.h
LIBYUV_API
int RAWToI444(const uint8_t* src_raw, ...);

// include/libyuv/version.h
-#define LIBYUV_VERSION 1902
+#define LIBYUV_VERSION 1903

// README.chromium
-Version: 1902
+Version: 1903
```
