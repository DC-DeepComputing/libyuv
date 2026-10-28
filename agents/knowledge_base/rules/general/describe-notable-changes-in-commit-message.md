---
name: libyuv/describe-notable-changes-in-commit-message
description: When writing a commit message description, summarize all notable changes, including public API modifications, format adjustments, and platform implications
rubric: readability
severity: should_fix
---

**Rule.** In the commit message description, explicitly summarize all notable
modifications introduced in the change, especially public API signature updates,
format parameter adjustments, and platform or SIMD optimization details. Do not
omit substantive functional or interface changes from the changelist summary.

**Why.** Commit messages serve as the primary historical record for reviewers,
downstream library consumers, and regression investigations. Omitting notable
API signature updates or format conversion changes hides important behavioral
adjustments and complicates debugging.

## Examples

Bad:

```cpp
Scaling cast to intptr_t for address computations
Row_common reduce to 1 cast per assignment.
```

Good:

```cpp
Scaling cast to intptr_t for address computations
Row_common reduce to 1 cast per assignment.
Update MT2TToP010 to take uint8_t* source pointers and byte strides.
```
