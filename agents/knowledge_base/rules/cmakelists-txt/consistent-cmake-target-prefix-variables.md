---
name: libyuv/cmakelists-txt/consistent-cmake-target-prefix-variables
description: When referencing object library targets in CMake generator expressions, use the base prefix variable used in their definition rather than alias or derivative variables
rubric: readability
severity: nit
---

**Rule.** Do not reference CMake object library targets in generator expressions
(such as `$<TARGET_OBJECTS:...>`) using an alias or derivative variable (e.g.,
`${ly_lib_static}`) when the targets were defined using a base prefix variable
(e.g., `${ly_lib_name}`). Consistently use the exact variable name under which
the target was defined.

**Why.** Referencing targets via alias variables that happen to evaluate to the
same value creates fragile and misleading coupling, leading to broken target
references or build failures if the alias variable is later renamed or decoupled
from the base library name.

## Examples

Bad:

```cpp
add_library(${ly_lib_name}_common_objects OBJECT ${ly_common_source_files})
set(ly_lib_parts $<TARGET_OBJECTS:${ly_lib_static}_common_objects>)
```

Good:

```cpp
add_library(${ly_lib_name}_common_objects OBJECT ${ly_common_source_files})
set(ly_lib_parts $<TARGET_OBJECTS:${ly_lib_name}_common_objects>)
```
