---
name: libyuv/api/row-and-scale-kernel-naming
description: When declaring or implementing libyuv row, column, or conversion kernels, use Col/Cols, omit Core_, suffix tail wrappers with _Any_<ARCH>, and capitalize 'To' in PascalCase transitions
rubric: readability
severity: should_fix
---

**Rule.** Follow libyuv low-level kernel and conversion naming conventions: use
Col/Cols (not RowHorizontal) for horizontal column operations, do not prefix or
qualify SIMD/C kernels with Core_, suffix boundary/tail-handling wrappers with
_Any_<ARCH>, and capitalize 'To' in PascalCase format or bit-depth conversion
transitions (e.g., 16To8, 8To16, ARGBToI420) rather than lowercase 'to' (e.g.,
16to8).

**Why.** Uniform naming across row, column, and conversion kernels ensures
consistent function identifiers across public headers, SIMD architecture
implementations, and dispatcher tables, preventing naming discrepancies across
ports.

## Examples

Bad:

```cpp
// Bad: RowHorizontal instead of Col, Core_ qualifier, and lowercase 'to'
void ScaleRowHorizontalUp2_Core_Linear_C(const uint8_t* src_ptr,
                                         uint8_t* dst_ptr,
                                         int dst_width);

void InterpolateRow_16to8_C(uint8_t* dst_ptr,
                            const uint16_t* src_ptr,
                            ptrdiff_t src_stride,
                            int scale,
                            int width,
                            int source_y_fraction);
```

Good:

```cpp
// Good: Col for column operations, no Core_, _Any_ suffix for tail wrappers, and capitalized 'To'
void ScaleColUp2_Linear_C(const uint8_t* src_ptr,
                          uint8_t* dst_ptr,
                          int dst_width);
void ScaleColUp2_Linear_Any_C(const uint8_t* src_ptr,
                              uint8_t* dst_ptr,
                              int dst_width);

void InterpolateRow_16To8_C(uint8_t* dst_ptr,
                            const uint16_t* src_ptr,
                            ptrdiff_t src_stride,
                            int scale,
                            int width,
                            int source_y_fraction);
```
