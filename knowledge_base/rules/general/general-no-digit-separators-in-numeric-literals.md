---
name: libyuv/general-no-digit-separators-in-numeric-literals
description: When writing integer or floating-point numeric literals, avoid single-quote digit separators like 0x0200'0200U; write plain unseparated literals for compiler portability
rubric: readability
severity: should_fix
---

**Rule.** Do not use single-quote digit separators (such as `0x0200'0200U` or
`1'000'000`) in numeric literals. Write plain unseparated literals (such as
`0x02000200U` or `1000000`) to ensure compatibility across all supported C and
C++ compiler toolchains and consistency with project style.

**Why.** Digit separators (introduced in C++14) are not supported in plain C or
by older and restrictive compilers targeted by libyuv, leading to build
breakages. Writing unseparated literals guarantees cross-compiler portability
and adheres to codebase conventions.

## Examples

Bad:

```cpp
// Avoid C++14 digit separators in hex and decimal literals
uint32_t nv_uv_start = 0x0200'0200U;
uint32_t nv_uv_step = 0x0404'0404U;
int64_t sample_rate = 1'000'000;
```

Good:

```cpp
// Write plain unseparated numeric literals for portability
uint32_t nv_uv_start = 0x02000200U;
uint32_t nv_uv_step = 0x04040404U;
int64_t sample_rate = 1000000;
```
