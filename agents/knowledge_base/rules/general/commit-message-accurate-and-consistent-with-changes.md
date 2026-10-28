---
name: libyuv/commit-message-accurate-and-consistent-with-changes
description: When writing or updating a changelist description, ensure the commit message accurately lists all newly introduced functions, modified components, and behavioral changes
rubric: readability
severity: should_fix
---

**Rule.** Ensure the changelist (CL) description and commit message accurately
reflect all functions, modules, and behavioral changes introduced in the change.
If functions, flags, or optimizations are added to the diff that are not part of
the described intent, either update the CL description to document them or
remove the unintended additions to keep the CL focused.

**Why.** An accurate CL description creates an understandable git history for
future triage and regression tracking, helps reviewers verify change scope, and
prevents unintended drive-by additions from slipping into unrelated commits.

## Examples

Bad:

```cpp
CL description:
ARGBToUV444MatrixRow_AVX2 intrinsics for Visual C

(CL diff adds ARGBExtractAlphaRow_SSE2 and ARGBExtractAlphaRow_AVX2 alongside ARGBToUV444MatrixRow_AVX2 without mentioning them)
```

Good:

```cpp
CL description:
ARGBToUV444MatrixRow_AVX2 and ARGBExtractAlphaRow intrinsics for Visual C

Detailed technical explanation of the change:
- Implement ARGBToUV444MatrixRow_AVX2 in row_win.cc.
- Add ARGBExtractAlphaRow_SSE2 and ARGBExtractAlphaRow_AVX2 intrinsics.

Test: libyuv_unittest --gunit_filter=*ARGB*
Bug: libyuv:12345
```
