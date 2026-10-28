---
name: libyuv/gn-restrict-internal-target-visibility
description: When defining internal helper targets in GN build files, explicitly restrict their visibility with `visibility = [ ":*" ]` to prevent external bypass of public interfaces.
rubric: design
severity: should_fix
---

**Rule.** Explicitly restrict the visibility of internal or helper GN build
targets (e.g., `visibility = [ ":*" ]`) unless they are intended to be public
API. Do not leave internal targets with default public visibility, which allows
external targets to bypass public interfaces and couple directly to private
implementation details.

**Why.** GN targets are publicly visible by default unless restricted. Setting
`visibility = [ ":*" ]` on internal helper targets enforces modular
encapsulation and ensures external consumers only depend on designated public
interface targets.

## Examples

Bad:

```cpp
static_library("libyuv_internal") {
  sources = [
    "source/internal_helper.cc",
  ]
}
```

Good:

```cpp
static_library("libyuv_internal") {
  visibility = [ ":*" ]

  sources = [
    "source/internal_helper.cc",
  ]
}
```
