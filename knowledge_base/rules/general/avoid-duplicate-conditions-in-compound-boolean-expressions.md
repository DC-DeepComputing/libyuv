---
name: libyuv/avoid-duplicate-conditions-in-compound-boolean-expressions
description: When writing compound boolean expressions or parameter null checks, ensure each variable is validated exactly once and eliminate duplicate terms
rubric: readability
severity: nit
---

**Rule.** Avoid duplicate condition checks or redundant terms in compound
boolean expressions (such as multi-pointer null validation or chained bounds
checks). Ensure each variable or argument is tested exactly once, and verify
whether a repeated condition was intended to check an omitted parameter.

**Why.** Duplicate conditions in compound boolean expressions are redundant and
typically result from copy-paste errors. They add noise to validation logic and
can mask missing checks for other intended operands.

## Examples

Bad:

```cpp
if (!src_y || !src_u || !src_v || !src_a || !src_a || !dst_argb ||
    width <= 0 || height == 0) {
  return -1;
}
```

Good:

```cpp
if (!src_y || !src_u || !src_v || !src_a || !dst_argb ||
    width <= 0 || height == 0) {
  return -1;
}
```
