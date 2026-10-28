---
name: libyuv/match-referenced-symbol-names-in-comments-to-declarations
description: When referencing constants, parameters, or types in comments, ensure symbol names accurately match their declared identifiers rather than stale or misspelled names
rubric: readability
severity: should_fix
---

**Rule.** When referencing constants, parameters, types, or helper symbols in
header or function comments, verify that all referenced identifier names
accurately match their actual declared names in the codebase. Avoid using
outdated, misspelled, or mismatched constant prefixes (such as referring to
`kBgr*` when the declared constants are `kAbgr*`).

**Why.** Inaccurate or stale symbol names in documentation confuse API consumers
about which constants, parameters, or tables to pass, and make searching the
codebase from documentation error-prone.

## Examples

Bad:

```cpp
// Convert ARGB To I444 with matrix.
// For ABGR little endian (rgba in memory) use the kBgr constants.
LIBYUV_API
int ARGBToI444Matrix(const uint8_t* src_argb,
                     int src_stride_argb,
                     uint8_t* dst_y,
                     int dst_stride_y,
                     const struct ArgbConstants* argbconstants,
                     int width,
                     int height);
```

Good:

```cpp
// Convert ARGB To I444 with matrix.
// For ABGR little endian (rgba in memory) use the kAbgr constants.
LIBYUV_API
int ARGBToI444Matrix(const uint8_t* src_argb,
                     int src_stride_argb,
                     uint8_t* dst_y,
                     int dst_stride_y,
                     const struct ArgbConstants* argbconstants,
                     int width,
                     int height);
```
