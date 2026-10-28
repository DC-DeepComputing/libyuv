---
name: libyuv/api/document-or-revert-disabled-feature-macros
description: When disabling feature macros like HAS_* or commenting out functionality, revert the temporary workaround once fixed, or document the rationale with an explanatory comment/TODO
rubric: readability
severity: should_fix
---

**Rule.** When disabling feature macros or commenting out functionality (such as
HAS_* row definitions in include/libyuv/row.h), revert the temporary workaround
once the underlying fix lands. If the feature must remain disabled, document the
rationale with an explanatory comment or attributed TODO, and describe the
change in the commit message.

**Why.** Commented-out macro definitions or disabled features without
explanation create ambiguity about whether the change was an accidental leftover
from debugging or an intentional workaround. Clear documentation or tracking
TODOs ensure maintainers know why optimizations are disabled and when they can
be safely re-enabled.

## Examples

Bad:

```cpp
// #define HAS_ARGBTOYJROW_NEON
// #define HAS_ARGBTOYROW_NEON
```

Good:

```cpp
// Either revert the workaround once the underlying fix lands:
#define HAS_ARGBTOYJROW_NEON
#define HAS_ARGBTOYROW_NEON

// Or document the rationale and tracking issue:
// TODO(bug/1234): Temporarily disabled due to compiler regression on Clang 18.
// #define HAS_ARGBTOYJROW_NEON
```
