---
name: libyuv/api/standard-dst-and-dst-stride-param-naming
rubric: readability
severity: blocking
---

**Rule.** In image conversion, scaling, rotation, row kernels, and processing
function declarations and implementations, name buffer pointer and stride
parameters consistently as `src_<format>` / `src_stride_<format>` and
`dst_<format>` / `dst_stride_<format>` matching the specific FOURCC, channel
byte order, or pixel layout defined by the function (e.g., `src_abgr`,
`dst_abgr`, `dst_stride_abgr`). Do not use non-standard names (e.g.,
`crop_argb`, `argb_stride`) or retain mismatched format names (e.g., `src_argb`
in ABGR/RGBA functions or `src_rgba` in ARGB functions) carried over from
copy-pasted templates or underlying matrix helper calls, and ensure accompanying
doc comments match. Specifically:
- For biplanar interleaved chroma formats, name pointers and strides to reflect actual channel byte order: use `src_vu` / `dst_vu` and `src_stride_vu` / `dst_stride_vu` for NV21 (where V precedes U), and `src_uv` / `dst_uv` and `src_stride_uv` / `dst_stride_uv` for NV12 (where U precedes V).
- For JPEG full-range planar formats (e.g., J420, J422, J400), consistently suffix all plane pointer and stride parameters with `j` (e.g., `dst_yj`, `dst_uj`, `dst_vj`, `dst_stride_yj`, `dst_stride_uj`, `dst_stride_vj`).

**Why.** Image buffer parameters in libyuv are untyped raw byte pointers
(`uint8_t*`, `const uint8_t*`), making parameter names and doc comments the
primary indicators of expected pixel memory layout, channel ordering, and color
range. Inaccurate or mismatched format names (such as ARGB vs. ABGR/RGBA, or
generic `uv` for NV21 where V precedes U) mislead API consumers and maintainers,
increasing the risk of applying incorrect conversion matrices or channel
offsets. Standard `src_<format>` / `dst_<format>` naming ensures uniform,
predictable APIs, distinguishes JPEG full-range planes from standard
limited-range planes, and prevents channel-swapping bugs between NV12 and NV21.

## Examples

Bad:

```cpp
// 1. High-level API: mismatched target format or missing 'j' suffix on full-range chroma
int NV12ToABGR(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_uv,
               int src_stride_uv,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height);

int ARGBToJ420(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_yj,
               int dst_stride_yj,
               uint8_t* dst_u,
               int dst_stride_u,
               uint8_t* dst_v,
               int dst_stride_v,
               int width,
               int height);

// 2. Interleaved chroma: NV21 chroma plane has V before U, but is named src_uv / dst_uv
int NV21ToARGB(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_uv,
               int src_stride_uv,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height);

// 3. Row kernel: mismatched format name copied from helper/template
// Convert 16 ARGB pixels (64 bytes) to 16 YJ values.
void ABGRToYJRow_SSSE3(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_SSSE3(src_argb, dst_y, width, &kAbgrJPEGConstants);
}
```

Good:

```cpp
// 1. High-level API: parameter names match ABGR format and include 'j' suffix for all JPEG planes
int NV12ToABGR(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_uv,
               int src_stride_uv,
               uint8_t* dst_abgr,
               int dst_stride_abgr,
               int width,
               int height);

int ARGBToJ420(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_yj,
               int dst_stride_yj,
               uint8_t* dst_uj,
               int dst_stride_uj,
               uint8_t* dst_vj,
               int dst_stride_vj,
               int width,
               int height);

// 2. Interleaved chroma: NV21 chroma plane is explicitly named src_vu / dst_vu to reflect V before U
int NV21ToARGB(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_vu,
               int src_stride_vu,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height);

// 3. Row kernel: parameter name and comments accurately match ABGR format
// Convert 16 ABGR pixels (64 bytes) to 16 YJ values.
void ABGRToYJRow_SSSE3(const uint8_t* src_abgr, uint8_t* dst_y, int width) {
  ARGBToYMatrixRow_SSSE3(src_abgr, dst_y, width, &kAbgrJPEGConstants);
}
```
