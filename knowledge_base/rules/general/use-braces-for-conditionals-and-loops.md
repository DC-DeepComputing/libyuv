---
name: libyuv/use-braces-for-conditionals-and-loops
description: When writing if, else, for, or while statements spanning multiple lines, enclose their bodies in curly braces rather than omitting them
rubric: readability
severity: should_fix
---

**Rule.** Enclose conditional (if, else) and loop (for, while, do) bodies in
curly braces ({}) whenever the statement spans multiple lines rather than
omitting braces.

**Why.** Omitting braces on multi-line control statements harms readability and
risks indentation-related logic bugs when modifying or adding statements to the
branch body.

## Examples

Bad:

```cpp
if (!row16)
  return 1;

for (int i = 0; i < width; ++i)
  dst[i] = src[i];
```

Good:

```cpp
if (!row16) {
  return 1;
}

for (int i = 0; i < width; ++i) {
  dst[i] = src[i];
}
```
