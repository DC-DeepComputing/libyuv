---
name: libyuv/avoid-attributing-library-requirements-to-compiler-versions
description: When writing explanatory comments for build dependencies or configurations, avoid attributing general runtime or standard library requirements like libc++ to specific compiler versions
rubric: readability
severity: nit
---

**Rule.** In explanatory comments for build dependencies, flags, or
configurations, avoid attributing general standard-library or runtime
requirements (such as libc++ dependencies) to specific compiler versions when
the requirement is independent of the compiler version. Attribute the
requirement directly to the runtime library instead (e.g., '# for new[] on
libc++' instead of '# for new[] on clang 5 libc++').

**Why.** Attributing general standard-library or runtime requirements to
specific compiler versions creates misleading documentation. As toolchain
versions advance, version-specific comments mislead future maintainers into
assuming a requirement was a temporary bug workaround for an obsolete compiler
release rather than an inherent library requirement.

## Examples

Bad:

```cpp
deps += [
  "//build/config:exe_and_shlib_deps",  # for new[] on clang 5 libc++
]
```

Good:

```cpp
deps += [
  "//build/config:exe_and_shlib_deps",  # for new[] on libc++
]
```
