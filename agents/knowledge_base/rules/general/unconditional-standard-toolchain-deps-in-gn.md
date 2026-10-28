---
name: libyuv/unconditional-standard-toolchain-deps-in-gn
description: When adding standard toolchain dependencies like //build/config:exe_and_shlib_deps in GN build targets, specify them unconditionally instead of wrapping in platform conditionals
rubric: readability
severity: should_fix
---

**Rule.** In GN build files (`BUILD.gn`, `*.gni`), add standard toolchain
dependencies (such as `//build/config:exe_and_shlib_deps`) unconditionally in
`deps` rather than guarding them behind platform conditionals (such as `if
(!is_ios)`). Keep platform-specific conditionals only around actual
platform-exclusive dependencies.

**Why.** Standard toolchain targets are designed to be portable across all build
configurations and safely evaluate to no-ops on platforms where they are
unneeded. Wrapping them in OS-specific checks adds unnecessary branching and
maintenance complexity to build targets.

## Examples

Bad:

```cpp
# Bad: Standard toolchain dependency guarded behind a platform check
if (!is_ios) {
  deps = [
    "//build/config:exe_and_shlib_deps",
    "//third_party:jpeg",
  ]
}
```

Good:

```cpp
# Good: Toolchain dependency added unconditionally; platform check only for platform-specific library
deps = [
  "//build/config:exe_and_shlib_deps",
]
if (!is_ios) {
  deps += [
    "//third_party:jpeg",
  ]
}
```
