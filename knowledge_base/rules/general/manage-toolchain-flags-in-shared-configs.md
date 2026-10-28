---
name: libyuv/manage-toolchain-flags-in-shared-configs
description: When handling toolchain incompatibilities for shared GN configs, manage compiler checks inside config definitions instead of removing configs per-target
rubric: design
severity: nit
---

**Rule.** In GN build files, do not manually remove shared build configs
(`configs -= [...]`) from individual targets using proxy platform or
architecture conditions. Manage compiler and toolchain compatibility checks
directly inside the shared config definitions using direct toolchain variables
like `is_clang`.

**Why.** Removing shared configs target-by-target using proxy platform
conditions (like `target_cpu == "x64"`) is brittle, duplicates logic across
targets, and breaks when compiler or platform configurations change.
Centralizing toolchain checks in config definitions ensures consistent flag
management across all targets.

## Examples

Bad:

```cpp
# Bad: Stripping a shared config per-target using a proxy CPU/OS condition
if (is_win && target_cpu == "x64") {
  configs -= [ "//build/config/win:vs_code_analysis" ]
}
```

Good:

```cpp
# Good: Handling toolchain compatibility inside the shared config definition
config("vs_code_analysis") {
  if (!is_clang) {
    cflags = [ "/analyze" ]
  }
}
```
