---
name: libyuv/source/scale-plane-function-naming
description: When defining 2D plane scaling functions, follow the ScalePlane[Up|Down]<Factor>[_<BitDepth>]_<Filter> naming convention without embedding 1D qualifiers like Col or Row
rubric: readability
severity: should_fix
---

**Rule.** When defining or declaring 2D plane-level scaling helper functions,
follow the established ScalePlane[Up|Down]<Factor>[_<BitDepth>]_<Filter> naming
convention (e.g., ScalePlaneUp2_Linear, ScalePlaneUp2_16_Linear,
ScalePlaneDown2_Box). Do not embed 1D axis qualifiers like 'Col' or 'Row' into
plane-level function identifiers, reserving 'Row' and 'Col' strictly for 1D
scanline kernels.

**Why.** In libyuv, plane-level scaling functions operate across full 2D image
planes rather than individual 1D scanlines or columns. Inserting axis qualifiers
like 'Col' or 'Row' into plane-level functions misrepresents their
multi-dimensional scope, confuses them with 1D scanline kernel helpers, and
breaks naming consistency across bit depths and filter modes.

## Examples

Bad:

```cpp
// Bad: Misleading 'Col' axis qualifier embedded in 2D plane scaling helpers
static void ScalePlaneColUp2_Linear(int src_width,
                                    int src_height,
                                    int dst_width,
                                    int dst_height,
                                    int src_stride,
                                    int dst_stride,
                                    const uint8_t* src,
                                    uint8_t* dst);

void ScalePlaneColUp2_16_Linear(int src_width,
                                int src_height,
                                int dst_width,
                                int dst_height,
                                int src_stride,
                                int dst_stride,
                                const uint16_t* src_ptr,
                                uint16_t* dst_ptr);
```

Good:

```cpp
// Good: Standard 2D plane scaling helper naming without axis qualifiers
static void ScalePlaneUp2_Linear(int src_width,
                                 int src_height,
                                 int dst_width,
                                 int dst_height,
                                 int src_stride,
                                 int dst_stride,
                                 const uint8_t* src,
                                 uint8_t* dst);

void ScalePlaneUp2_16_Linear(int src_width,
                             int src_height,
                             int dst_width,
                             int dst_height,
                             int src_stride,
                             int dst_stride,
                             const uint16_t* src_ptr,
                             uint16_t* dst_ptr);
```
