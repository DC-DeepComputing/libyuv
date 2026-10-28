---
name: libyuv/avoid-ambiguous-placeholders-in-comments
description: When referencing function families or pattern variations in comments, use explicit variant groupings or standard placeholders rather than ambiguous invented abbreviations
rubric: readability
severity: nit
---

**Rule.** When referring to function families, kernel variants, or architecture
patterns in comments, do not use ambiguous, invented placeholder tokens (such as
'OOO'). Use explicit variant groupings (e.g., '(Bi)linear') or standard,
recognizable wildcard placeholders (e.g., 'XXX' or '[FILTER]').

**Why.** Arbitrary or idiosyncratic placeholder tokens are confusing and can be
misinterpreted as domain-specific technical acronyms (such as out-of-order
execution). Explicit groupings and recognizable placeholders clarify which
functions or variants are referenced without creating ambiguity.

## Examples

Bad:

```cpp
// ScaleUVRowUp2_OOO_RVV function is equal to other platforms'
// ScaleUVRowUp2_OOO_Any_XXX. We process entire row in this function.
```

Good:

```cpp
// ScaleUVRowUp2_(Bi)linear_RVV function is equal to other platforms'
// ScaleUVRowUp2_(Bi)linear_Any_XXX. We process entire row in this function.
```
