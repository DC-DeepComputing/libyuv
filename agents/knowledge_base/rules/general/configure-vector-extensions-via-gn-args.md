---
name: libyuv/configure-vector-extensions-via-gn-args
description: When configuring or disabling CPU vector extensions and feature flags in GN build files, expose configurable arguments in declare_args() rather than hardcoding platform defines
rubric: readability
severity: should_fix
---

**Rule.** Do not hardcode OS- or architecture-specific preprocessor defines
(such as LIBYUV_DISABLE_RVV or LIBYUV_DISABLE_NEON) directly in GN config blocks
to disable CPU vector extensions. Expose configurable build arguments in
declare_args() (in libyuv.gni or BUILD.gn) and condition preprocessor
definitions or source lists on those arguments instead.

**Why.** Hardcoding feature disablement macros for specific platform
combinations directly in build configs prevents developers, downstream projects,
and continuous integration tests from overriding vector features via build
arguments. Declaring configurable GN arguments preserves build flexibility and
ensures uniform flag management across all architectures.

## Examples

Bad:

```cpp
# In BUILD.gn
config("libyuv_config") {
  defines = []
  if (is_android && current_cpu == "riscv64") {
    defines += [ "LIBYUV_DISABLE_RVV" ]
  }
}
```

Good:

```cpp
# In libyuv.gni
declare_args() {
  libyuv_disable_rvv = false
}

# In BUILD.gn
config("libyuv_config") {
  defines = []
  if (libyuv_disable_rvv) {
    defines += [ "LIBYUV_DISABLE_RVV" ]
  }
}
```
