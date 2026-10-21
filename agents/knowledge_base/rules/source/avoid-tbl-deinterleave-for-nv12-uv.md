---
name: libyuv/source/avoid-tbl-deinterleave-for-nv12-uv
rubric: safety
severity: should_fix
---

**Rule.** In SIMD row conversion kernels for biplanar formats (NV12 and NV21),
avoid de-interleaving UV chroma into separate planar vector registers using
vector table lookup instructions (such as SVE tbl or byte shuffles). Keep UV
chroma pairs interleaved and process them directly with widened multiply or
dot-product instructions (e.g., ARM SVE umullb/umullt, NEON umull/umull2, or x86
vpmaddubsw).

**Why.** Table lookup and shuffle instructions add register pressure for index
tables, require setup overhead, and increase execution latency. Because NV12 and
NV21 formats store UV chroma interleaved in a single memory plane, operating
directly on interleaved pairs avoids unpacking overhead and maximizes SIMD
throughput.

## Examples

Bad:

```cpp
// Separating interleaved NV12 UV chroma into separate registers using table lookup
#define READNV_SVE                               \
  "ld1b       {z1.h}, p2/z, [%[src_uv]]      \n" \
  "tbl        z2.h, {z1.h}, z23.h            \n" \
  "tbl        z1.h, {z1.h}, z22.h            \n"
```

Good:

```cpp
// Keeping UV pairs interleaved and multiplying even/odd lanes directly
#define NVTORGB_SVE_2X(bt_u, bt_v)                        \
  "umull" #bt_u " z4.h, z28.b, z2.b          \n" /* DB */ \
  "umull" #bt_v " z5.h, z29.b, z2.b          \n" /* DR */
```
