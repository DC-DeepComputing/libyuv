---
name: libyuv/api/document-public-api-removals-in-commit-message
description: When removing or deprecating public LIBYUV_API functions from public headers, explicitly document and justify the API removals in the commit message
rubric: readability
severity: should_fix
---

**Rule.** When intentionally removing or deprecating public functions or exports
(marked with LIBYUV_API) from public headers under include/libyuv/, explicitly
document and justify the removals in the commit message description. Do not
silently delete public API declarations or merge API deletions without
description context.

**Why.** Deleting public API functions is a breaking change for downstream
consumers and external embedders. Documenting intentional removals in the commit
message prevents accidental deletions during rebases and gives downstream
consumers clear notice and rationale for API changes.

## Examples

Bad:

```cpp
Enable UNLIMITED_DATA for BT.709

Bug: chromium:1313591
(CL also deletes public LIBYUV_API functions like I422ToI210 without mention in the description)
```

Good:

```cpp
Enable UNLIMITED_DATA for BT.709 and remove unused conversion functions

Remove I422ToI210 and related deprecated public conversion APIs.

Bug: chromium:1313591
```
