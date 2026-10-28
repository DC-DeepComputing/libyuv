---
name: libyuv/api/document-conversion-matrix-memory-layout
description: When exposing color conversion matrix constants in public headers, document how little-endian register format names map to in-memory byte layouts
rubric: readability
severity: nit
---

**Rule.** In public conversion headers declaring matrix conversion constants
(such as ArgbConstants or YuvConstants), document in a preceding comment how
format names correspond to in-memory byte layouts under little-endian register
naming (e.g., explaining that little-endian ARGB is BGRA in memory and ABGR is
RGBA). Include usage guidance showing API consumers which constant family to
select for their input memory buffers.

**Why.** Libyuv FOURCC names specify channel order within CPU registers on
little-endian architectures, causing memory byte order to be reversed.
Documenting this relationship alongside conversion constants prevents API
consumers from selecting the wrong matrix tables for their memory buffers.

## Examples

Bad:

```cpp
// Conversion matrix for RGB to YUV
LIBYUV_API extern const struct ArgbConstants kArgbI601Constants;
LIBYUV_API extern const struct ArgbConstants kAbgrI601Constants;
```

Good:

```cpp
// The ArgbConstants below can be used with the ARGBTo*Matrix() functions to
// process different RGB formats. E.g., if your input is ARGB little endian
// (bgra in memory) you'll want to use the kArgb* constants. Alternatively, if
// your input is ABGR little endian (rgba in memory) you'd use the kAbgr* ones.
//
// Conversion matrix for xRGB to YUV.
LIBYUV_API extern const struct ArgbConstants kArgbI601Constants;
LIBYUV_API extern const struct ArgbConstants kAbgrI601Constants;
```
