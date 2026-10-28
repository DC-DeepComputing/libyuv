---
name: libyuv/avoid-internal-repository-codenames-in-comments
description: When writing comments, build files, or documentation, avoid internal company repository codenames like google3 and refer to internal infrastructure generically instead
rubric: readability
severity: nit
---

**Rule.** In open-source comments, documentation, and build files, avoid
internal Google repository codenames such as `google3`. Refer to internal
infrastructure generically (for example, "Google's internal repository")
instead.

**Why.** Open-source code and build definitions are read by external
contributors and consumers. Using company-internal jargon or specific monorepo
codenames creates confusing, non-standard terminology and leaks internal
implementation details into public code.

## Examples

Bad:

```cpp
# Detect if we are in google3 (where source files are under files/) or
# open source (where source files are at the root).
PREFIX = "files/" if len(native.glob(["files/source/*.cc"])) > 0 else ""
```

Good:

```cpp
# Detect if we are in Google's internal repository (where source files are
# under files/) or open source (where source files are at the root).
PREFIX = "files/" if len(native.glob(["files/source/*.cc"])) > 0 else ""
```
