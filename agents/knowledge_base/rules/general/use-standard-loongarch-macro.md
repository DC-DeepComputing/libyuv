---
name: libyuv/use-standard-loongarch-macro
description: When checking for LoongArch CPU architecture in preprocessor guards, use standard compiler macro __loongarch__ instead of obsolete __loongarch
rubric: correctness
severity: should_fix
---

**Rule.** When checking for LoongArch architecture in preprocessor guards, use
standard compiler predefined macros such as `__loongarch__` (or `__loongarch64`
/ `__loongarch32` and extension macros like `__loongarch_sx` /
`__loongarch_asx`). Avoid obsolete or non-standard macros like `__loongarch`
without trailing underscores, which modern compilers do not define.

**Why.** Modern GCC and Clang compilers define `__loongarch__` to identify the
LoongArch architecture. Using the non-standard `__loongarch` macro causes
preprocessor conditions to fail silently, disabling architecture-specific
features, SIMD optimizations, or test coverage.

## Examples

Bad:

```cpp
#if defined(__loongarch)
  int has_loongarch = TestCpuFlag(kCpuHasLOONGARCH);
  if (has_loongarch) {
    printf("Has LOONGARCH 0x%x\n", has_loongarch);
  }
#endif
```

Good:

```cpp
#if defined(__loongarch__)
  int has_loongarch = TestCpuFlag(kCpuHasLOONGARCH);
  if (has_loongarch) {
    printf("Has LOONGARCH 0x%x\n", has_loongarch);
  }
#endif
```
