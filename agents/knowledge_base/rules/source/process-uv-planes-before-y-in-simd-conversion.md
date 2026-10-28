---
name: libyuv/source/process-uv-planes-before-y-in-simd-conversion
description: In planar YUV SIMD conversion kernels, load and process UV chroma planes before or interleaved with Y rather than completing Y processing first
rubric: safety
severity: nit
---

**Rule.** In planar YUV SIMD conversion kernels (such as I420, I422, I444, I210,
and I212 row readers), order memory loads and conversion operations to load and
process UV chroma planes before or interleaved with Y luma processing rather
than completing all Y operations before loading UV.

**Why.** In planar YUV formats, UV chroma planes have smaller memory footprints
and often require multi-step unpacking, widening, or subsampling interpolation
operations (such as trn1 or shifts). Loading and processing UV chroma first or
interleaving UV unpacks with Y loads hides memory latency, overlaps execution
pipeline stages, and avoids pipeline stalls.

## Examples

Bad:

```cpp
#define READI212_SVE                             \
  "ld1h       {z3.h}, p1/z, [%[src_y]]       \n" \
  "lsl        z0.h, z3.h, #4                 \n" \
  "usra       z0.h, z3.h, #8                 \n" \
  "ld1h       {z1.s}, p1/z, [%[src_u]]       \n" \
  "ld1h       {z2.s}, p1/z, [%[src_v]]       \n"
```

Good:

```cpp
#define READI212_SVE                             \
  "ld1h       {z3.h}, p1/z, [%[src_y]]       \n" \
  "ld1h       {z1.s}, p1/z, [%[src_u]]       \n" \
  "ld1h       {z2.s}, p1/z, [%[src_v]]       \n" \
  "incb       %[src_y]                       \n" \
  "inch       %[src_u]                       \n" \
  "inch       %[src_v]                       \n" \
  "lsl        z0.h, z3.h, #4                 \n" \
  "trn1       z1.h, z1.h, z1.h               \n" \
  "trn1       z2.h, z2.h, z2.h               \n"
```
