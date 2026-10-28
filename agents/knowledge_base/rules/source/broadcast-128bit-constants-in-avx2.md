---
name: libyuv/source/broadcast-128bit-constants-in-avx2
description: When defining or loading 128-bit SIMD constants in AVX2, store compact 16-byte entries and broadcast across lanes using vbroadcast or _mm256_broadcastsi128_si256 instead of duplicate 32-byte tables
rubric: readability
severity: should_fix
---

**Rule.** When defining SIMD lookup, shuffle mask, or conversion constant tables
(e.g. `YuvConstants`, `ArgbConstants`, `kLinearShuffleFar`, `kRGBToU`), store
compact 128-bit (16-byte) constant entries (or pack adjacent 16-byte entries)
rather than duplicating 256-bit (32-byte) arrays. In x86 AVX2 inline assembly
and SIMD intrinsics, load 128-bit vector constants by broadcasting across both
lanes using `vbroadcastf128`/`vbroadcasti128` or
`_mm256_broadcastsi128_si256(_mm_loadu_si128(...))`. Do not define duplicate
32-byte constant tables (such as `_AVX2` copies) or perform direct 256-bit loads
(`_mm256_loadu_si256`) from 128-bit structures.

**Why.** Duplicating 32-byte arrays for 128-bit constants in static tables or
structs bloats binary size and memory/cache footprint. Broadcasting 16-byte
constants into 256-bit YMM registers reuses existing SSE/NEON constants and
prevents out-of-bounds reads or uninitialized high lanes caused by direct
256-bit loads from 128-bit structures.

## Examples

Bad:

```cpp
// Bad (Struct Definition): Duplicating 32-byte arrays in constant tables
struct YuvConstants {
  uint8_t kUMaskB[32];
  uint8_t kVMaskR[32];
};

// Bad (Assembly): Defining duplicate 32-byte tables for AVX2
static const uvec8 kLinearShuffleFar_AVX2 = {
    2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13,
    2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13};
asm volatile("vmovdqu %0, %%ymm5" : : "m"(kLinearShuffleFar_AVX2));

// Bad (Intrinsics): Direct 256-bit load from 128-bit matrix constant
__m256i ymm_u = _mm256_loadu_si256((const __m256i*)c->kRGBToU);
```

Good:

```cpp
// Good (Struct Definition): Pack adjacent 16-byte entries or keep 128-bit size
struct YuvConstants {
  uint8_t kUVMaskBR[32];  // 16 bytes for U mask, 16 bytes for V mask
};

// Good (Assembly): Broadcast 128-bit SSE table across both YMM lanes
static const uvec8 kLinearShuffleFar = {2, 3, 0, 1, 6, 7, 4, 5,
                                        10, 11, 8, 9, 14, 15, 12, 13};
asm volatile("vbroadcastf128 %0, %%ymm5" : : "m"(kLinearShuffleFar));

// Good (Intrinsics): Load 128-bit vector and broadcast across both YMM lanes
__m128i kRGBToU = _mm_loadu_si128((const __m128i*)c->kRGBToU);
__m256i ymm_u = _mm256_broadcastsi128_si256(kRGBToU);
```
