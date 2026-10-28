---
name: libyuv/api/scale-api-use-filter-mode-internal-dispatch
description: When declaring image operations in public headers, accept mode enums (like FilterMode or RotationMode) and keep single-operation helper functions private in source files
rubric: design
severity: should_fix
---

**Rule.** Do not expose specialized or single-operation helper functions (such
as RotatePlane90_16 or ScaleI420ToI444Bilinear) in public headers. Instead,
declare unified public API functions that accept configuration or mode enums
(e.g., enum RotationMode for RotatePlane_16 or enum FilterMode for ScalePlane),
and keep low-level, mode-specific implementation helpers static in .cc source
files with internal dispatching.

**Why.** Public headers should expose minimal, consistent, and flexible entry
points rather than leaking internal implementation helpers. Parameterizing APIs
by mode enums prevents public API surface bloat and allows callers to
automatically benefit from internal fast-path optimizations without changing
call sites.

## Examples

Bad:

```cpp
// In include/libyuv/rotate.h:
LIBYUV_API
void RotatePlane90_16(const uint16_t* src, int src_stride,
                      uint16_t* dst, int dst_stride,
                      int width, int height);

// In include/libyuv/scale.h:
LIBYUV_API
int ScaleI420ToI444Bilinear(const uint8_t* src_u, int src_stride_u,
                            const uint8_t* src_v, int src_stride_v,
                            int src_width, int src_height,
                            uint8_t* dst_u, int dst_stride_u,
                            uint8_t* dst_v, int dst_stride_v);
```

Good:

```cpp
// In include/libyuv/rotate.h:
LIBYUV_API
int RotatePlane_16(const uint16_t* src, int src_stride,
                   uint16_t* dst, int dst_stride,
                   int width, int height,
                   enum RotationMode mode);

// In source/rotate.cc:
static void RotatePlane90_16(const uint16_t* src, int src_stride,
                             uint16_t* dst, int dst_stride,
                             int width, int height);

// In include/libyuv/scale.h:
LIBYUV_API
int ScalePlane(const uint8_t* src, int src_stride,
               int src_width, int src_height,
               uint8_t* dst, int dst_stride,
               int dst_width, int dst_height,
               enum FilterMode filtering);
```
