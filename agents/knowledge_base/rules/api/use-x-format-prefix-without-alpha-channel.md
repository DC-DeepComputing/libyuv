---
name: libyuv/api/use-x-format-prefix-without-alpha-channel
description: When naming packed format merge or conversion functions and row kernels lacking an alpha input plane, use the 'X' prefix instead of 'A'
rubric: readability
severity: should_fix
---

**Rule.** Name packed format conversion or plane merge functions and row kernels
using the 'X' prefix (such as MergeXR30Plane, MergeXR30Row_C) rather than 'A'
(such as MergeAR30Plane) when merging or converting RGB planes without an alpha
source channel. Reserve the 'A' prefix for formats and functions that take an
explicit alpha input plane.

**Why.** In libyuv format naming conventions, 'A' indicates an active or
explicitly provided alpha channel (e.g., AR30 or ARGB taking an alpha input
plane), whereas 'X' denotes packed formats where alpha or padding bits are
unused or set to opaque without an alpha input plane.

## Examples

Bad:

```cpp
// Merging R, G, B without an alpha input plane named with 'A' prefix
void MergeAR30Row_C(const uint16_t* src_r,
                    const uint16_t* src_g,
                    const uint16_t* src_b,
                    uint8_t* dst_ar30,
                    int depth,
                    int width);
```

Good:

```cpp
// Merging R, G, B without an alpha input plane named with 'X' prefix
void MergeXR30Row_C(const uint16_t* src_r,
                    const uint16_t* src_g,
                    const uint16_t* src_b,
                    uint8_t* dst_ar30,
                    int depth,
                    int width);
```
