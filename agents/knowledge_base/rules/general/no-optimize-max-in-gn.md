---
name: libyuv/no-optimize-max-in-gn
rubric: safety
severity: should_fix
---

**Rule.** In GN build files (BUILD.gn, *.gni), do not override compiler
optimization settings by removing //build/config/compiler:default_optimization
and adding //build/config/compiler:optimize_max. Allow standard platform and
toolchain default optimization configurations to apply.

**Why.** Forcing -O2 via optimize_max triggers aggressive compiler loop
unrolling that causes binary size bloat without meaningful performance gains.
Libyuv relies on hand-crafted SIMD assembly row functions for throughput, so
respecting platform defaults (such as -Oz on Android) minimizes binary footprint
while preserving performance.

## Examples

Bad:

```cpp
# Bad: Overriding default optimization with optimize_max in build targets
static_library("libyuv_sve") {
  if (!is_debug) {
    configs -= [ "//build/config/compiler:default_optimization" ]
    configs += [ "//build/config/compiler:optimize_max" ]
  }
}
```

Good:

```cpp
# Good: Allowing default optimization configs to apply
static_library("libyuv_sve") {
  # Retains default_optimization (e.g. -Oz on Android) without forced -O2 unrolling
}
```
