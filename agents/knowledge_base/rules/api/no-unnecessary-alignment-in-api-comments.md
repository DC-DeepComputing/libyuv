---
name: libyuv/api/no-unnecessary-alignment-in-api-comments
description: When documenting API buffer or table parameters in public headers, do not specify strict alignment requirements unless actually required by underlying SIMD implementations
rubric: readability
severity: should_fix
---

**Rule.** Do not document strict alignment requirements on API buffer, table, or
vector parameters (such as shuffle tables or filter matrices) when underlying
SIMD implementations support unaligned access. If alignment improves performance
but is not strictly mandatory, describe it as recommended rather than required.

**Why.** Modern SIMD architectures (such as x86 SSE/AVX and ARM NEON) handle
unaligned vector loads efficiently with minimal or no penalty. Mandating strict
buffer alignment in API comments imposes unnecessary constraints and boilerplate
alignment logic on callers.

## Examples

Bad:

```cpp
// shuffler is 16 bytes and must be aligned.
LIBYUV_API
int ARGBShuffle(const uint8_t* src_bgra,
                int src_stride_bgra,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const uint8_t* shuffler,
                int width,
                int height);
```

Good:

```cpp
// shuffler is 16 bytes.
LIBYUV_API
int ARGBShuffle(const uint8_t* src_bgra,
                int src_stride_bgra,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const uint8_t* shuffler,
                int width,
                int height);
```
