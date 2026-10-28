---
name: libyuv/synchronize-endif-comment-with-condition
description: 'When branching or closing non-trivial preprocessor blocks, annotate #else and #endif directives with active conditions and keep all trailing/doc comments synchronized'
rubric: readability
severity: should_fix
---

**Rule.** Annotate `#else` and `#endif` preprocessor directives with comments
indicating the active or closing condition on non-trivial conditional
compilation blocks (e.g., annotating `#else` with `// defined(FOO)` following
`#ifndef FOO` or `#if !defined(FOO)`). Additionally, when adding or modifying
preprocessor conditional blocks (`#if`, `#ifdef`, `#ifndef`, `#elif`), keep
accompanying descriptive comments (such as architecture or platform notes) and
trailing comments on `#else` and `#endif` synchronized with the updated
conditional expression.

**Why.** In multi-line conditional compilation blocks, trailing comments on
`#else` and `#endif` clarify which configuration branch is active without
requiring readers to scroll back up to the opening guard. Keeping descriptive
header comments and trailing annotations synchronized with preprocessor
conditionals prevents stale target lists or contradictory macro conditions from
misleading readers.

## Examples

Bad:

```cpp
// Variant 1: Missing active condition annotation on #else
#ifndef USE_EXTRACTALPHA
// ... extensive function implementation ...
#else
int ARGBToI420Alpha(...) { ... }
#endif  // USE_EXTRACTALPHA

// Variant 2: Stale descriptive doc comment and trailing #endif comment
#if defined(__aarch64__) || defined(__arm__) || defined(__riscv)
// This struct is for ARM color conversion.
struct YuvConstants {
  // ...
};
#endif  // defined(__aarch64__) || defined(__arm__)
```

Good:

```cpp
// Variant 1: Annotated #else with inverted/active condition comment
#ifndef USE_EXTRACTALPHA
// ... extensive function implementation ...
#else  // defined(USE_EXTRACTALPHA)
int ARGBToI420Alpha(...) { ... }
#endif  // USE_EXTRACTALPHA

// Variant 2: Synchronized descriptive doc comment and trailing #endif comment
#if defined(__aarch64__) || defined(__arm__) || defined(__riscv)
// This struct is for ARM and RISC-V color conversion.
struct YuvConstants {
  // ...
};
#endif  // defined(__aarch64__) || defined(__arm__) || defined(__riscv)
```
