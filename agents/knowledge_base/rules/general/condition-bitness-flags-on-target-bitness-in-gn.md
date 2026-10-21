---
name: libyuv/condition-bitness-flags-on-target-bitness-in-gn
rubric: correctness
severity: should_fix
---

**Rule.** In GN build files (BUILD.gn, *.gni), do not enumerate individual
64-bit CPU architectures (such as current_cpu == "arm64" || current_cpu ==
"riscv64") when setting bitness-dependent compiler flags, linker paths, or
system directories (such as Android /system/bin/linker64). Condition on target
bitness, architecture word size, or default bitness rules so newly added 64-bit
architectures are handled automatically without manual per-CPU list updates.

**Why.** Enumerating individual 64-bit CPU architecture strings for
bitness-dependent toolchain options is fragile and incomplete. When new 64-bit
targets (such as RISC-V 64 or LoongArch 64) are introduced, hardcoded lists
silently fall back to 32-bit paths or fail at link time.

## Examples

Bad:

```cpp
# Bad: Enumerating individual 64-bit CPU architectures for linker selection
if (is_android) {
  if (current_cpu == "arm64" || current_cpu == "riscv64") {
    ldflags = [ "-Wl,--dynamic-linker,/system/bin/linker64" ]
  } else {
    ldflags = [ "-Wl,--dynamic-linker,/system/bin/linker" ]
  }
}
```

Good:

```cpp
# Good: Conditioning on target bitness or defaulting to 64-bit linker
if (is_android) {
  if (current_cpu == "arm" || current_cpu == "x86" || current_cpu == "mipsel") {
    ldflags = [ "-Wl,--dynamic-linker,/system/bin/linker" ]
  } else {
    # Default to 64-bit linker for all 64-bit CPU architectures
    ldflags = [ "-Wl,--dynamic-linker,/system/bin/linker64" ]
  }
}
```
