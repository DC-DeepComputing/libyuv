---
name: libyuv/no-sanitizer-conditional-storage-or-sync
rubric: safety
severity: should_fix
---

**Rule.** Do not conditionally change variable storage duration (such as using
`__thread`, `thread_local`, or TLS) or modify synchronization primitives under
sanitizer macros like `THREAD_SANITIZER` or `__SANITIZE_THREAD__`. Ensure
sanitizer builds execute and analyze the exact concurrent execution and storage
model shipped in production.

**Why.** Sanitizers like ThreadSanitizer (TSan) detect concurrency bugs and data
races in production workflows. Changing storage duration (such as turning shared
variables into thread-local storage) or altering synchronization solely under
sanitizer builds silences sanitizer warnings by masking real races while leaving
production builds unprotected.

## Examples

Bad:

```cpp
// Bad: Switching from shared to thread-local storage under TSan hides data races.
#ifdef THREAD_SANITIZER
LIBYUV_API extern __thread int cpu_info_;
#else
LIBYUV_API extern int cpu_info_;
#endif
```

Good:

```cpp
// Good: Keep identical storage duration and use proper atomic synchronization.
LIBYUV_API extern int cpu_info_;
```
