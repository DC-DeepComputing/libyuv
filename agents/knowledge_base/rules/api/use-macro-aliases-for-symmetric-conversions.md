---
name: libyuv/api/use-macro-aliases-for-symmetric-conversions
description: 'When defining symmetric or identical format conversions, use a #define macro alias in public headers instead of implementing duplicate C functions'
rubric: readability
severity: nit
---

**Rule.** When a format conversion is symmetric (such as bidirectional channel
swaps like AR64 to AB64) or identical to an existing conversion (such as
converting between equivalent formats or identity copies), define a #define
macro alias in the public header (e.g., #define AB64ToAR64 AR64ToAB64) rather
than declaring and implementing duplicate C functions.

**Why.** Exposing symmetric or identical format conversions as preprocessor
aliases in public headers reuses the canonical implementation, eliminating
redundant function definitions and avoiding binary bloat.

## Examples

Bad:

```cpp
// In source/convert_argb.cc
LIBYUV_API
int AB64ToAR64(const uint16_t* src_ab64,
               int src_stride_ab64,
               uint16_t* dst_ar64,
               int dst_stride_ar64,
               int width,
               int height) {
  return AR64Shuffle(src_ab64, src_stride_ab64, dst_ar64, dst_stride_ar64,
                     (const uint8_t*)&kShuffleMaskAR64ToAB64, width, height);
}
```

Good:

```cpp
// In include/libyuv/convert_argb.h
// Convert AB64 To AR64.
#define AB64ToAR64 AR64ToAB64
```
