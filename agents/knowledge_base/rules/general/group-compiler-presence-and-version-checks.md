---
name: libyuv/group-compiler-presence-and-version-checks
description: When writing preprocessor conditionals that test compiler versions, keep compiler presence checks contiguous with their version comparisons instead of splitting them with architecture or platform guards
rubric: readability
severity: nit
---

**Rule.** In preprocessor conditional directives (`#if`, `#elif`), keep compiler
presence checks (such as `defined(__clang__)`, `defined(__GNUC__)`, or
`defined(_MSC_VER)`) contiguous with their corresponding version comparisons
(e.g., `__clang_major__ >= 19`, `_MSC_VER < 1900`). Do not separate tightly
coupled compiler identity and version checks with intermediate architecture, OS,
or feature flag conditions.

**Why.** Grouping compiler identification macros directly with their version
requirements ensures logical clarity, makes compiler prerequisites easy to read
and maintain, and prevents related checks from being scattered across unrelated
platform or build conditions.

## Examples

Bad:

```cpp
// Bad: Compiler presence check and version comparison are separated by architecture and OS conditions.
#if !defined(LIBYUV_DISABLE_SME) && defined(__clang__) && \
    defined(__aarch64__) && defined(__linux__) && (__clang_major__ >= 19)

#if defined(_MSC_VER) && defined(_M_IX86) && (_MSC_VER < 1900)
```

Good:

```cpp
// Good: Compiler presence checks and version comparisons are grouped contiguously.
#if !defined(LIBYUV_DISABLE_SME) && defined(__aarch64__) && \
    defined(__linux__) && defined(__clang__) && (__clang_major__ >= 19)

#if defined(_M_IX86) && defined(_MSC_VER) && (_MSC_VER < 1900)
```
