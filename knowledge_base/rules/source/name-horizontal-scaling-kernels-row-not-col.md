---
name: libyuv/source/name-horizontal-scaling-kernels-row-not-col
description: When defining horizontal scanline scaling or format conversion kernels, name functions with Row rather than Col to distinguish row operations from column filtering
rubric: readability
severity: should_fix
---

**Rule.** Name horizontal scanline scaling and conversion kernels with 'Row'
(e.g., ScaleRowUp2_*, ScaleRowDown2_*) rather than 'Col' or 'Cols'. Reserve
'Col' and 'Cols' (e.g., ScaleCols_*, ScaleFilterCols_*) strictly for vertical
column-wise operations and multi-pass vertical interpolation.

**Why.** In libyuv multi-pass scaling and conversion routines, horizontal
operations process across scanlines along the width axis while column operations
filter vertically along the height axis. Naming a horizontal scanline kernel
with 'Col' confuses its operational axis and breaks consistency with existing
row kernels.

## Examples

Bad:

```cpp
// Bad: Horizontal scanline scaling kernels misnamed with 'Col'
void ScaleColUp2_Linear_NEON(const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             int dst_width);
void ScaleColDown2_Box_SSSE3(const uint8_t* src_ptr,
                             ptrdiff_t src_stride,
                             uint8_t* dst_ptr,
                             int dst_width);
```

Good:

```cpp
// Good: Horizontal scanline scaling kernels correctly named with 'Row'
void ScaleRowUp2_Linear_NEON(const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             int dst_width);
void ScaleRowDown2_Box_SSSE3(const uint8_t* src_ptr,
                             ptrdiff_t src_stride,
                             uint8_t* dst_ptr,
                             int dst_width);
```
