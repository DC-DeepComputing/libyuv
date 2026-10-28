---
name: libyuv/no-redundant-preprocessor-macro-guards
description: When defining platform or compiler compatibility macro aliases under compiler guards, avoid redundant preprocessor checks like && !defined(symbol)
rubric: readability
severity: nit
---

**Rule.** When defining platform or compiler compatibility aliases under
compiler-specific preprocessor guards (such as `#if defined(_MSC_VER)`), do not
add redundant guard conditions (such as `&& !defined(SYMBOL)`) for symbols known
not to be defined in that environment. Keep preprocessor conditionals concise
and omit unnecessary defensive checks.

**Why.** Redundant preprocessor checks add visual noise and boilerplate without
providing additional safety or functionality when defining compatibility shims
for platforms where the standard identifier is absent.

## Examples

Bad:

```cpp
#if defined(_MSC_VER) && !defined(strtok_r)
#define strtok_r strtok_s
#endif
```

Good:

```cpp
#if defined(_MSC_VER)
#define strtok_r strtok_s
#endif
```
