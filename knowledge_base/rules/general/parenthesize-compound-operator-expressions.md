---
name: libyuv/parenthesize-compound-operator-expressions
description: When writing compound boolean expressions, conditionals, or assertions that mix arithmetic, comparison, and logical operators, add parentheses around subexpressions to make precedence explicit
rubric: readability
severity: nit
---

**Rule.** In compound boolean expressions, conditionals, and assertions mixing
arithmetic (`+`, `-`, `*`), comparison (`==`, `!=`, `<`, `>`), and logical
operators (`&&`, `||`), parenthesize arithmetic subexpressions and individual
comparisons (e.g., `((a - b) == 0) || ((a - b) == 1)`). Do not leave
multi-operator precedence implicit.

**Why.** While C/C++ operator precedence strictly specifies evaluation order,
unparenthesized compound expressions mixing arithmetic, relational, and logical
operators are difficult to parse visually and prone to precedence bugs. Explicit
parenthesization makes evaluation order clear at a glance.

## Examples

Bad:

```cpp
assert(boxwidth - minboxwidth == 0 || boxwidth - minboxwidth == 1);
if (flags & kMask != 0 && count + offset > limit) {
  return;
}
```

Good:

```cpp
assert(((boxwidth - minboxwidth) == 0) || ((boxwidth - minboxwidth) == 1));
if (((flags & kMask) != 0) && ((count + offset) > limit)) {
  return;
}
```
