---
name: libyuv/source/shift-for-high-bit-depth-row-conversions
rubric: safety
severity: should_fix
---

**Rule.** In high bit-depth (10-bit, 12-bit, 16-bit) row split, merge, and
conversion functions, convert between bit depths using bit shifts parameterized
by bit depth (`16 - depth`) rather than multiplication or fixed-point scaling.

**Why.** High bit-depth biplanar and planar formats store values MSB-aligned
with zero-padded lower bits rather than bit replication. Shifting by `(16 -
depth)` matches format specifications and executes faster on SIMD architectures
using variable vector shifts.

## Examples

Bad:

```cpp
void SplitUVRow_16_C(const uint16_t* src_uv,
                     uint16_t* dst_u,
                     uint16_t* dst_v,
                     int scale,
                     int width) {
  for (int x = 0; x < width; ++x) {
    dst_u[x] = (src_uv[0] * scale) >> 16;
    dst_v[x] = (src_uv[1] * scale) >> 16;
    src_uv += 2;
  }
}
```

Good:

```cpp
void SplitUVRow_16_C(const uint16_t* src_uv,
                     uint16_t* dst_u,
                     uint16_t* dst_v,
                     int depth,
                     int width) {
  int shift = 16 - depth;
  for (int x = 0; x < width; ++x) {
    dst_u[x] = src_uv[0] >> shift;
    dst_v[x] = src_uv[1] >> shift;
    src_uv += 2;
  }
}
```
