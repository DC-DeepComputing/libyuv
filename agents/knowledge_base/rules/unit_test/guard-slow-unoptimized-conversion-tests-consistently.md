---
name: libyuv/unit_test/guard-slow-unoptimized-conversion-tests-consistently
rubric: correctness
severity: should_fix
---

**Rule.** When gating slow, unoptimized unit tests with slow-test macros
(ENABLE_SLOW_TESTS, DISABLE_SLOW_TESTS) or architecture checks (such as
__x86_64__, __i386__) to prevent timeouts on non-SIMD bots, ensure all related
conversion families (planar, biplanar, quad-planar) sharing the same unoptimized
status are consistently guarded. Do not leave sibling conversion suites
unguarded when they rely on the same slow C fallback implementation.

**Why.** High bit-depth and multi-planar conversions lacking SIMD vector
optimizations run orders of magnitude slower in software fallback
implementations, causing test timeouts on non-x86 bots. Inconsistently guarding
only a subset of test suites leaves slow fallback paths running on slow
builders.

## Examples

Bad:

```cpp
// TESTPLANAR16TOB and TESTQPLANAR16TOB guarded, but biplanar tests left unguarded:
TESTBIPLANAR16TOB(P010, 2, 2, ARGB, 4, 4, 1, 10)
TESTBIPLANAR16TOB(P210, 2, 1, ARGB, 4, 4, 1, 10)
```

Good:

```cpp
#if defined(ENABLE_SLOW_TESTS) || defined(__x86_64__) || defined(__i386__)
TESTBIPLANAR16TOB(P010, 2, 2, ARGB, 4, 4, 1, 10)
TESTBIPLANAR16TOB(P210, 2, 1, ARGB, 4, 4, 1, 10)
#endif  // defined(ENABLE_SLOW_TESTS)
```
