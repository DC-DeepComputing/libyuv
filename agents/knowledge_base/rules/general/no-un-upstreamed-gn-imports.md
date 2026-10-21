---
name: libyuv/no-un-upstreamed-gn-imports
rubric: design
severity: blocking
---

**Rule.** Do not import external GN build configuration files (such as `.gni`
files under `//build/` or `//third_party/`) that do not yet exist upstream in
the repository's pinned `DEPS`. Ensure external build configuration changes have
landed upstream and rolled into `DEPS` before referencing them.

**Why.** External build directories like `//build/` are populated from external
repositories pinned via `DEPS`. Importing un-upstreamed or un-rolled `.gni`
files breaks `gn gen` and build automation for any checkout tracking pinned
dependencies.

## Examples

Bad:

```cpp
# Importing a .gni file before it lands upstream and rolls into DEPS
import("//build/config/loongarch.gni")
```

Good:

```cpp
# Import only after the .gni file is committed upstream and rolled into DEPS
import("//build/config/loongarch64.gni")
```
