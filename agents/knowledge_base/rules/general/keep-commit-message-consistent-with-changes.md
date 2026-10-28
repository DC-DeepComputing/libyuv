---
name: libyuv/keep-commit-message-consistent-with-changes
description: When writing or updating a changelist description, accurately and completely describe all conditions, parameters, edge cases, and behavior changes introduced by the code
rubric: readability
severity: should_fix
---

**Rule.** When authoring a changelist (CL) description, ensure the explanation
accurately reflects the full scope of the code change, including all checked
conditions, modified parameters, and handled edge cases. Do not write a
description that only mentions a subset of the conditions or dimensions handled
by the implementation.

**Why.** Commit messages provide permanent documentation in the project's
revision history. Inaccurate or partial descriptions mislead reviewers and
future maintainers investigating regressions or porting fixes about the true
intent and scope of a change.

## Examples

Bad:

```cpp
The fix in this CL is to return early if height is 0 because AFAICT a 0
height should never have a copy performed.
```

Good:

```cpp
The fix in this CL is to return early if height or width is 0 because
AFAICT a 0 width/height should never have a copy performed.
```
