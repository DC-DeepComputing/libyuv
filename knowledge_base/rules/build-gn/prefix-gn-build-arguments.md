---
name: libyuv/build-gn/prefix-gn-build-arguments
description: When declaring build arguments in GN files via declare_args(), prefix all project-specific argument names with libyuv_ instead of unprefixed names
rubric: readability
severity: should_fix
---

**Rule.** In GN build files (`BUILD.gn`, `*.gni`), prefix all project-specific
build arguments defined in `declare_args()` with the `libyuv_` project prefix
(e.g., `libyuv_use_gflags` rather than `use_gflags`). When referencing these
arguments across conditionals, dependencies, or configs, consistently use the
prefixed name.

**Why.** GN build arguments defined in `declare_args()` share a single global
namespace across all dependencies in a checkout (e.g., when libyuv is embedded
in Chromium). Unprefixed build arguments risk naming collisions with root
configurations or other third-party projects.

## Examples

Bad:

```cpp
declare_args() {
  # Anti-pattern: Unprefixed build argument pollutes global namespace.
  use_custom_feature = true
}

if (use_custom_feature) {
  defines += [ "LIBYUV_CUSTOM_FEATURE" ]
  deps += [ "//third_party/custom" ]
}
```

Good:

```cpp
declare_args() {
  # Project-specific argument prefixed with libyuv_.
  libyuv_use_custom_feature = true
}

if (libyuv_use_custom_feature) {
  defines += [ "LIBYUV_CUSTOM_FEATURE" ]
  deps += [ "//third_party/custom" ]
}
```
