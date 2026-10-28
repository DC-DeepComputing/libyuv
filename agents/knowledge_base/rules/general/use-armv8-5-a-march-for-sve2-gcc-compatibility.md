---
name: libyuv/use-armv8-5-a-march-for-sve2-gcc-compatibility
description: When configuring compiler -march flags for AArch64 SVE2 or SME in build files, use -march=armv8.5-a with explicit extension modifiers (+sve2, +sme, +i8mm) instead of -march=armv9-a
rubric: correctness
severity: nit
---

**Rule.** When setting compiler architecture flags (`-march=`) for AArch64 SVE2
or SME source files in CMake, GN, or Bazel build configurations, use
`-march=armv8.5-a` with explicit extension modifiers (e.g.,
`-march=armv8.5-a+i8mm+sve2` or `-march=armv8.5-a+i8mm+sme`) rather than
`-march=armv9-a` or relying on implicit extension enablement. Always explicitly
specify required modifiers such as `+sve2`, `+sme`, `+i8mm`, and `+dotprod`.

**Why.** Older compilers such as GCC 10 and GCC 11 support SVE2 and I8MM
extensions via feature modifiers on Armv8.5-A, but reject `-march=armv9-a`
because Armv9-A architecture string support was only introduced in GCC 12 / LLVM
14. Additionally, SVE2 is an optional extension where compiler toolchains differ
in whether `-march=armv9-a` implicitly enables SVE2 instructions. Base flags
using `armv8.5-a` combined with explicit extension modifiers guarantee broad
toolchain compatibility and deterministic code generation across both older and
newer GCC/Clang compilers.

## Examples

Bad:

```cpp
# CMakeLists.txt / BUILD.bazel
# Bad: Uses armv9-a (fails on GCC 10/11) and/or omits explicit +sve2
target_compile_options(${ly_lib_name}_sve PRIVATE -march=armv9-a+i8mm)
target_compile_options(${ly_lib_name}_sme PRIVATE -march=armv9-a+sme)
```

Good:

```cpp
# CMakeLists.txt / BUILD.bazel
# Good: Targets armv8.5-a with explicit extension modifiers
target_compile_options(${ly_lib_name}_sve PRIVATE -march=armv8.5-a+i8mm+sve2)
target_compile_options(${ly_lib_name}_sme PRIVATE -march=armv8.5-a+i8mm+sme)
```
