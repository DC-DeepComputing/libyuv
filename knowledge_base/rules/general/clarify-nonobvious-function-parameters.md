---
name: libyuv/clarify-nonobvious-function-parameters
description: When declaring function parameters with non-obvious units, scale factors, bit depths, or mode flags, use descriptive names and comments documenting expected values
rubric: readability
severity: should_fix
---

**Rule.** When declaring functions with parameters whose purpose, allowed
values, units, scale factors, bit depths, or mode flags are not self-evident
from type or name alone (such as mode flags, fixed-point multipliers, or scaling
factors), use descriptive parameter names (e.g., is_yuv instead of bare yuv) and
add clarifying comments documenting what each mode, scaling unit, or value
mapping represents.

**Why.** Vague parameter names (like int yuv) and raw numeric multipliers or
scale factors (like int scale in bit-depth conversions) obscure parameter units
and format expectations, leading callers to pass wrong values or compute
incorrect scaling.

## Examples

Bad:

```cpp
// Vague parameter name 'yuv' does not explain whether it is a flag or enum.
int InitYuvConstantsWithMatrix(struct YuvConstants* yuvconstants,
                               const float matrix[3][3],
                               const float bias[3],
                               int yuv);

// Parameter 'scale' has no documentation explaining unit or bit-depth mapping.
void InterpolateRow_16to8_C(uint8_t* dst_ptr,
                            const uint16_t* src_ptr,
                            ptrdiff_t src_stride,
                            int scale,
                            int width,
                            int source_y_fraction);
```

Good:

```cpp
// is_yuv: non-zero for YUV->RGB, 0 for YVU->BGR.
int InitYuvConstantsWithMatrix(struct YuvConstants* yuvconstants,
                               const float matrix[3][3],
                               const float bias[3],
                               int is_yuv);

// scale represents fixed-point scaling for bits per channel:
// 16384 = 10 bits, 4096 = 12 bits, 256 = 16 bits.
void InterpolateRow_16to8_C(uint8_t* dst_ptr,
                            const uint16_t* src_ptr,
                            ptrdiff_t src_stride,
                            int scale,
                            int width,
                            int source_y_fraction);
```
