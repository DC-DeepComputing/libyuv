---
name: libyuv/source/share-vector-constants-outside-arch-ifdef
description: 'When defining vector shuffle masks, coefficients, or lookup tables used across architectures, place them outside architecture #ifdef blocks rather than duplicating definitions'
rubric: design
severity: should_fix
---

**Rule.** Define common constants, vector shuffle masks, and lookup tables once
at file or namespace scope outside architecture `#ifdef` blocks rather than
duplicating identical definitions across target-specific (e.g., 32-bit vs.
64-bit or inline assembly vs. intrinsics) sections. If a constant is shared
across multiple architecture implementations, place it unconditionally above the
conditional blocks.

**Why.** Duplicating identical constant arrays, masks, or lookup tables inside
separate architecture-specific conditional compilation blocks increases
maintenance burden, causes code duplication, and risks silent divergence when
tables are modified.

## Examples

Bad:

```cpp
// 64-bit intrinsics section
#if defined(_M_X64)
static const lvec8 kShuffleYUY2Y = {0, 0, 2, 2, 4, 4, 6, 6,
                                    8, 8, 10, 10, 12, 12, 14, 14};
#endif

// 32-bit inline assembly section
#if !defined(_M_X64) && defined(_M_IX86)
static const lvec8 kShuffleYUY2Y = {0, 0, 2, 2, 4, 4, 6, 6,
                                    8, 8, 10, 10, 12, 12, 14, 14};
#endif
```

Good:

```cpp
// Shared across 32-bit and 64-bit implementations
static const lvec8 kShuffleYUY2Y = {0, 0, 2, 2, 4, 4, 6, 6,
                                    8, 8, 10, 10, 12, 12, 14, 14};

#if defined(_M_X64)
// 64-bit implementation using kShuffleYUY2Y
#elif defined(_M_IX86)
// 32-bit implementation using kShuffleYUY2Y
#endif
```
