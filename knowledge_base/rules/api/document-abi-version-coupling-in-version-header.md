---
name: libyuv/api/document-abi-version-coupling-in-version-header
description: When updating library version headers that couple with build-file ABI variables, document in the header that ABI major versions require updates on breaking changes
rubric: readability
severity: nit
---

**Rule.** When shared library ABI or package versioning (such as SOVERSION,
YUV_VER_MAJOR, or CPACK_PACKAGE_VERSION_MAJOR in CMake build files) is defined
separately from the primary version header (e.g., include/libyuv/version.h),
document the coupling in the version header. Include an explanatory comment
above the version macro reminding maintainers that breaking changes to the
public ABI require updating the major ABI version variable in the corresponding
build script.

**Why.** Maintainers frequently bump the release or patch version macro in
public headers without checking auxiliary build scripts or package definitions.
Documenting the ABI coupling directly in the version header prevents silent ABI
breakages in shared library builds and ensures package major versions are
updated when incompatible ABI changes occur.

## Examples

Bad:

```cpp
// include/libyuv/version.h
#define LIBYUV_VERSION 1948
```

Good:

```cpp
// include/libyuv/version.h
// Whenever a breaking change happens in libyuv's ABI,
// YUV_VER_MAJOR has to be updated in CM_linux_packages.cmake.
#define LIBYUV_VERSION 1949
```
