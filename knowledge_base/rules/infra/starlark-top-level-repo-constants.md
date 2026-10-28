---
name: libyuv/infra/starlark-top-level-repo-constants
description: When configuring LUCI Starlark files (*.star), define repository and Gerrit URLs as top-level constants instead of duplicating string literals across definitions
rubric: readability
severity: should_fix
---

**Rule.** In LUCI Starlark configuration files (`*.star`), define repository,
Gerrit, and Gitiles URLs as top-level uppercase constants (e.g., `<PROJECT>_GIT
= "..."`) rather than repeating raw URL string literals across
`luci.gitiles_poller`, `luci.console_view`, `luci.cq_group`, and scheduler
definitions.

**Why.** Extracting service and repository URLs into top-level constants
establishes a single source of truth, avoids duplicate string literals, and
prevents subtle configuration inconsistencies such as trailing slash mismatches
across pollers, views, and CQ definitions.

## Examples

Bad:

```cpp
luci.gitiles_poller(
    name = "master-gitiles-trigger",
    bucket = "ci",
    repo = "https://chromium.googlesource.com/libyuv/libyuv",
)

luci.console_view(
    name = "ci",
    repo = "https://chromium.googlesource.com/libyuv/libyuv",
)
```

Good:

```cpp
LIBYUV_GIT = "https://chromium.googlesource.com/libyuv/libyuv"

luci.gitiles_poller(
    name = "master-gitiles-trigger",
    bucket = "ci",
    repo = LIBYUV_GIT,
)

luci.console_view(
    name = "ci",
    repo = LIBYUV_GIT,
)
```
