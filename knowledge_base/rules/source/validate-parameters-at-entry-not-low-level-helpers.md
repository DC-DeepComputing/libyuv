---
name: libyuv/source/validate-parameters-at-entry-not-low-level-helpers
description: Validate parameters and buffer bounds at public API entry points, keeping internal planar and row processing helpers void without redundant checks
rubric: design
severity: should_fix
---

**Rule.** Perform parameter validation, buffer overflow checks, and memory
allocations at public API entry points or outer caller functions. Keep internal,
low-level planar, column, filter, and row processing helpers returning `void`
(or `static void`), assuming pre-validated parameters rather than duplicating
bounds checks or returning error codes.

**Why.** Public API entry points enforce API contracts and return error codes to
callers. Low-level planar and row processing kernels are inner
performance-critical execution paths; returning `void` and assuming
pre-validated inputs eliminates redundant runtime checks, avoids
error-propagation boilerplate, and keeps kernel interfaces clean.

## Examples

Bad:

```cpp
// Bad: Low-level helper duplicates validation and returns an error code.
int ScalePlaneVertical_16To8(int height, int uv_width, int uv_height,
                             int src_stride_u, int dst_stride_u,
                             const uint16_t* src_u, uint8_t* dst_u,
                             int y, int dy, int bpp,
                             int scale, FilterMode filter) {
  int64_t dst_width_bytes64 = (int64_t)uv_width * bpp;
  if (!src_u || !dst_u || uv_width <= 0 || dst_width_bytes64 > INT_MAX) {
    return -1;
  }
  // ... process rows ...
  return 0;
}

int r = ScalePlaneVertical_16To8(height, uv_width, uv_height, src_stride_u,
                                 dst_stride_u, src_u, dst_u, 0, 32768, dy,
                                 /*bpp=*/1, scale, kFilterBilinear);
if (r != 0) {
  return r;
}
```

Good:

```cpp
// Good: Helper returns void and assumes pre-validated inputs.
void ScalePlaneVertical_16To8(int height, int uv_width, int uv_height,
                              int src_stride_u, int dst_stride_u,
                              const uint16_t* src_u, uint8_t* dst_u,
                              int y, int dy, int bpp,
                              int scale, FilterMode filter) {
  int dst_width_bytes = uv_width * bpp;
  // ... process rows ...
}

// Outer caller validates parameters before invoking internal helpers.
if (!src_u || !dst_u || uv_width <= 0 || uv_width > INT_MAX / bpp) {
  return -1;
}
ScalePlaneVertical_16To8(height, uv_width, uv_height, src_stride_u,
                         dst_stride_u, src_u, dst_u, 0, 32768, dy,
                         /*bpp=*/1, scale, kFilterBilinear);
return 0;
```
