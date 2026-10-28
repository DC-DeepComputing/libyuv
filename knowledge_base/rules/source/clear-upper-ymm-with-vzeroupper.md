---
name: libyuv/source/clear-upper-ymm-with-vzeroupper
description: Clear upper YMM register state with vzeroupper or _mm256_zeroupper() on all exit paths when writing AVX or AVX2 SIMD functions or assembly kernels
rubric: safety
severity: should_fix
---

**Rule.** When exiting any AVX or AVX2 SIMD function or kernel using 256-bit YMM
registers, clear the upper 128-bit YMM register state on all normal and early
return paths by emitting `vzeroupper` (in inline assembly) or invoking
`_mm256_zeroupper()` (in C++ intrinsics or MSVC assembly).

**Why.** Modifying 256-bit YMM registers leaves their upper 128 bits dirty.
Exiting AVX/AVX2 routines without clearing this state forces the CPU into an
AVX-to-SSE transition penalty, causing severe latency and throughput degradation
whenever legacy 128-bit SSE code is subsequently executed.

## Examples

Bad:

```cpp
// Example 1: AVX2 C++ intrinsics missing _mm256_zeroupper()
void YUY2ToARGBRow_AVX2(const uint8_t* src_yuy2,
                        uint8_t* dst_argb,
                        const struct YuvConstants* yuvconstants,
                        int width) {
  while (width > 0) {
    READYUY2_AVX2
    YUVTORGB_AVX2(yuvconstants)
    STOREARGB_AVX2
    width -= 16;
  }
}

// Example 2: AVX/AVX2 inline assembly missing vzeroupper
__asm__ __volatile__(
    "1:                                        \n"
    "vmovdqu     (%0),%%ymm0                   \n"
    "vmovdqu     %%ymm0,(%1)                   \n"
    "sub         $0x20,%2                      \n"
    "jg          1b                            \n"
    : "+r"(src), "+r"(dst), "+r"(width)
    :
    : "memory", "ymm0");
```

Good:

```cpp
// Example 1: AVX2 C++ intrinsics with _mm256_zeroupper()
void YUY2ToARGBRow_AVX2(const uint8_t* src_yuy2,
                        uint8_t* dst_argb,
                        const struct YuvConstants* yuvconstants,
                        int width) {
  while (width > 0) {
    READYUY2_AVX2
    YUVTORGB_AVX2(yuvconstants)
    STOREARGB_AVX2
    width -= 16;
  }
  _mm256_zeroupper();
}

// Example 2: AVX/AVX2 inline assembly with vzeroupper
__asm__ __volatile__(
    "1:                                        \n"
    "vmovdqu     (%0),%%ymm0                   \n"
    "vmovdqu     %%ymm0,(%1)                   \n"
    "sub         $0x20,%2                      \n"
    "jg          1b                            \n"
    "vzeroupper                                \n"
    : "+r"(src), "+r"(dst), "+r"(width)
    :
    : "memory", "ymm0");
```
