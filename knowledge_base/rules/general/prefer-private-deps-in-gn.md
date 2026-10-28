---
name: libyuv/prefer-private-deps-in-gn
description: When declaring target dependencies in GN build files, use private deps instead of public_deps unless the dependency is exposed in public headers.
rubric: design
severity: nit
---

**Rule.** In GN build files (`BUILD.gn`, `*.gni`), specify dependencies using
private `deps` rather than `public_deps` when the dependency is not exposed in
the target's public headers. Reserve `public_deps` for dependencies whose
headers, defines, or flags are part of the target's public API and must be
forwarded to downstream consumers.

**Why.** Using `public_deps` for internal dependencies leaks transitive header
include paths and compiler configs to all downstream targets, increasing build
coupling and obscuring true dependency boundaries.

## Examples

Bad:

```cpp
# BAD: jpeg is used only internally in libyuv implementation files.
static_library("libyuv_internal") {
  public_deps = [
    "//third_party:jpeg",
  ]
}
```

Good:

```cpp
# GOOD: Internal dependencies stay private to avoid leaking headers/configs.
static_library("libyuv_internal") {
  deps = [
    "//third_party:jpeg",
  ]
}
```
