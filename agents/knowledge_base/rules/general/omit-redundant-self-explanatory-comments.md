---
name: libyuv/omit-redundant-self-explanatory-comments
description: When writing or updating source or build files, omit redundant, tautological, or stale comments that merely restate self-evident conditionals, syntax, or obsolete import/file instructions.
rubric: readability
severity: nit
---

**Rule.** Do not include or retain comments that merely restate self-evident
syntax or conditionals (such as commenting that `if (!is_debug)` applies to
release builds), describe obvious properties of a file (such as stating a build
file is compatible with its build tool), or leave obsolete copy/import
instructions or legacy workaround explanations. Remove obsolete workflow
directions and tautological comments once files are checked in or logic is
simplified.

**Why.** Comments that narrate self-explanatory code or describe obvious file
metadata add visual noise and maintenance overhead without providing actionable
context. Once a file is committed or refactored, obsolete copy instructions and
tautological comments risk becoming stale and distract readers from non-obvious
logic.

## Examples

Bad:

```cpp
// Variant 1: Obsolete file copying or tool compatibility comments in build files
# This Bazel build file is compatible with open-source Bazel and can be
# copied directly into the root of the libyuv git repository.
load("@bazel_skylib//lib:selects.bzl", "selects")

// Variant 2: Self-evident comments restating conditionals or legacy flags
# Always enable optimization for Release.
if (!is_debug) {
  configs += [ "//build/config/compiler:optimize_max" ]
}
```

Good:

```cpp
// Variant 1: Clean build file without redundant preamble comments
load("@bazel_skylib//lib:selects.bzl", "selects")

// Variant 2: Self-explanatory conditional without tautological comments
if (!is_debug) {
  configs += [ "//build/config/compiler:optimize_max" ]
}
```
