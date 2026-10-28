---
name: libyuv/align-build-option-names-and-polarity
description: When defining build options or configuration flags in CMake, GN, GYP, or Android build files, align option names, prefixes, and boolean polarity across build systems
rubric: readability
severity: should_fix
---

**Rule.** When defining build options or configuration arguments in CMake, GN,
GYP, or Android build files, match the naming convention, project prefix
(`LIBYUV_` / `libyuv_`), and boolean polarity (e.g., disable flags defaulting to
false/off vs. enable flags) of equivalent flags in the project's other build
configurations. Avoid introducing inverted polarity flags (such as
`JPEG_SUPPORT` defaulting to `ON`) when counterpart build systems use disable
flags (such as `libyuv_disable_jpeg = false` or `LIBYUV_DISABLE_JPEG`).

**Why.** Libyuv maintains multiple build configurations (GN, CMake, GYP,
Android). Inconsistent flag names and inverted boolean polarity across build
systems create confusion for developers and package maintainers, complicate
cross-platform CI matrix scripting, and increase the risk of building with
unintended feature configurations.

## Examples

Bad:

```cpp
# CMakeLists.txt
option( JPEG_SUPPORT "Build with JPEG support if available" ON )

if (JPEG_SUPPORT)
  include_directories( ${JPEG_INCLUDE_DIR} )
  target_link_libraries( ${ly_lib_name} ${JPEG_LIBRARY} )
endif()
```

Good:

```cpp
# CMakeLists.txt
option( LIBYUV_DISABLE_JPEG "Disable JPEG support" OFF )

if (NOT LIBYUV_DISABLE_JPEG)
  include_directories( ${JPEG_INCLUDE_DIR} )
  target_link_libraries( ${ly_lib_name} ${JPEG_LIBRARY} )
endif()
```
