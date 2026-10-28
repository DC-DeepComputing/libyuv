---
name: libyuv/document-non-obvious-build-rules-and-toolchain-overrides
description: When adding non-obvious build dependencies, target forwarding, toolchain overrides, or platform conditionals in build files, explain the technical rationale in comments
rubric: readability
severity: nit
---

**Rule.** When introducing non-obvious build rules, target dependencies, target
forwarding, toolchain overrides (such as directing targets to Clang toolchains),
or platform-specific conditionals in build files (such as `BUILD.gn`, `.gni`, or
`CMakeLists.txt`), precede them with an explanatory comment detailing the
technical rationale and constraints (such as inline assembly support, avoiding
duplicate library copies, or preventing linker symbol conflicts).

**Why.** Unusual build target decoupling, dependency structures, and toolchain
redirects risk being broken, accidentally cleaned up, or refactored away during
maintenance unless the underlying architectural constraints, compiler
requirements, or toolchain conflict preventions are documented directly at the
configuration site.

## Examples

Bad:

```cpp
# Variant 1: Unexplained build dependency placement / forwarding
if (!is_ios) {
  deps = [
    "//third_party:jpeg",
  ]
}

# Variant 2: Unexplained toolchain override
if (is_win && target_cpu == "x64" && current_toolchain == default_toolchain) {
  public_deps = [ ":libyuv_internal(//build/toolchain/win:win_clang_x64)" ]
}
```

Good:

```cpp
# Variant 1: Documented dependency constraint
if (!is_ios) {
  # Make sure clients of libyuv link with libjpeg. This cannot go in
  # libyuv_internal because in Windows x64 builds that will generate a clang
  # build of libjpeg, resulting in duplicate library copies.
  deps = [
    "//third_party:jpeg",
  ]
}

# Variant 2: Documented toolchain requirement
if (is_win && target_cpu == "x64" && current_toolchain == default_toolchain) {
  # Compile with clang on Windows x64 to support inline assembly.
  public_deps = [ ":libyuv_internal(//build/toolchain/win:win_clang_x64)" ]
}
```
