---
name: libyuv/prefer-positive-preprocessor-condition
description: 'When writing preprocessor conditional blocks with an #else branch, prefer positive #ifdef or #if defined checks and place the shorter block first'
rubric: readability
severity: nit
---

**Rule.** When writing preprocessor conditional blocks that contain an `#else`
branch, prefer positive conditions (`#ifdef MACRO` or `#if defined(MACRO)`) over
negative ones (`#ifndef` or `#if !defined(...)`). Additionally, place the
shorter conditional block first so the initial branch remains compact and
readable.

**Why.** Positive conditions are easier to reason about than negated checks, and
placing the shorter block first keeps the initial conditional section concise so
readers do not have to scroll past a long implementation before seeing the
alternative branch.

## Examples

Bad:

```cpp
#ifndef USE_EXTRACTALPHA
// Long fallback implementation (many lines)...
#else  // USE_EXTRACTALPHA
// Short fast implementation...
#endif  // USE_EXTRACTALPHA
```

Good:

```cpp
#ifdef USE_EXTRACTALPHA
// Short fast implementation...
#else  // USE_EXTRACTALPHA
// Long fallback implementation (many lines)...
#endif  // USE_EXTRACTALPHA
```
