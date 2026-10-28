---
name: libyuv/document-secondary-changes-in-commit-description
description: When authoring a changelist with secondary or drive-by modifications alongside the primary change, explicitly document all distinct changes in the commit description
rubric: readability
severity: should_fix
---

**Rule.** Describe all modifications included in the changelist within the
commit description. When a CL bundles secondary, auxiliary, or drive-by edits
alongside the primary change, explicitly mention them so reviewers and future
maintainers inspecting git history understand why each change occurred.

**Why.** Undocumented secondary changes create confusion during code review and
obscure git history when debugging regressions or bisecting commits.

## Examples

Bad:

```cpp
Make internal CpuId function match type of intrinsic

Instead of casting and lint warnings, use same types as
intrinsics for x86 use for CpuId.
```

Good:

```cpp
Make internal CpuId function match type of intrinsic

Instead of casting and lint warnings, use same types as
intrinsics for x86 use for CpuId.
Also simplify CPU capability string matching in ArmCpuCaps and MipsCpuCaps.
```
