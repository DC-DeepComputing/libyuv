/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/convert_from.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "libyuv/convert.h"  // For I420Copy
#include "libyuv/cpu_id.h"
#include "libyuv/planar_functions.h"
#include "libyuv/rotate.h"
#include "libyuv/row.h"
#include "libyuv/scale.h"  // For ScalePlane()
#include "libyuv/scale_row.h"
#include "libyuv/video_common.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

#define SUBSAMPLE(v, a, s) (v < 0) ? (-((-v + a) >> s)) : ((v + a) >> s)
static __inline int Abs(int v) {
  return v >= 0 ? v : -v;
}

// I420 To any I4xx YUV format with mirroring.
static int I420ToI4xx(const uint8_t* src_y,
                      int src_stride_y,
                      const uint8_t* src_u,
                      int src_stride_u,
                      const uint8_t* src_v,
                      int src_stride_v,
                      uint8_t* dst_y,
                      int dst_stride_y,
                      uint8_t* dst_u,
                      int dst_stride_u,
                      uint8_t* dst_v,
                      int dst_stride_v,
                      int src_y_width,
                      int src_y_height,
                      int dst_uv_width,
                      int dst_uv_height) {
  const int src_uv_width = SUBSAMPLE(src_y_width, 1, 1);
  const int src_uv_height = SUBSAMPLE(src_y_height, 1, 1);
  int r;
  if ((!src_y && dst_y) || !src_u || !src_v || !dst_u || !dst_v ||
      src_y_width <= 0 || src_y_height == 0 || dst_uv_width <= 0 ||
      dst_uv_height <= 0) {
    return -1;
  }
  if (dst_y) {
    CopyPlane(src_y, src_stride_y, dst_y, dst_stride_y, src_y_width,
              src_y_height);
  }
  r = ScalePlane(src_u, src_stride_u, src_uv_width, src_uv_height, dst_u,
                 dst_stride_u, dst_uv_width, dst_uv_height, kFilterBilinear);
  if (r != 0) {
    return r;
  }
  r = ScalePlane(src_v, src_stride_v, src_uv_width, src_uv_height, dst_v,
                 dst_stride_v, dst_uv_width, dst_uv_height, kFilterBilinear);
  return r;
}

// Convert 8 bit YUV to 10 bit.
LIBYUV_API
int I420ToI010(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint16_t* dst_y,
               int dst_stride_y,
               uint16_t* dst_u,
               int dst_stride_u,
               uint16_t* dst_v,
               int dst_stride_v,
               int width,
               int height) {
  int halfwidth = (width + 1) >> 1;
  int halfheight = (height + 1) >> 1;
  if ((!src_y && dst_y) || !src_u || !src_v || !dst_u || !dst_v || width <= 0 ||
      height == 0 || height == INT_MIN) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    halfheight = (height + 1) >> 1;
    if (src_y) {
      src_y = src_y + (ptrdiff_t)(height - 1) * src_stride_y;
      src_stride_y = -src_stride_y;
    }
    src_u = src_u + (ptrdiff_t)(halfheight - 1) * src_stride_u;
    src_v = src_v + (ptrdiff_t)(halfheight - 1) * src_stride_v;
    src_stride_u = -src_stride_u;
    src_stride_v = -src_stride_v;
  }

  // Convert Y plane.
  if (dst_y) {
    Convert8To16Plane(src_y, src_stride_y, dst_y, dst_stride_y, 1024, width,
                      height);
  }
  // Convert UV planes.
  Convert8To16Plane(src_u, src_stride_u, dst_u, dst_stride_u, 1024, halfwidth,
                    halfheight);
  Convert8To16Plane(src_v, src_stride_v, dst_v, dst_stride_v, 1024, halfwidth,
                    halfheight);
  return 0;
}

// Convert 8 bit YUV to 12 bit.
LIBYUV_API
int I420ToI012(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint16_t* dst_y,
               int dst_stride_y,
               uint16_t* dst_u,
               int dst_stride_u,
               uint16_t* dst_v,
               int dst_stride_v,
               int width,
               int height) {
  int halfwidth = (width + 1) >> 1;
  int halfheight = (height + 1) >> 1;
  if ((!src_y && dst_y) || !src_u || !src_v || !dst_u || !dst_v || width <= 0 ||
      height == 0 || height == INT_MIN) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    halfheight = (height + 1) >> 1;
    if (src_y) {
      src_y = src_y + (ptrdiff_t)(height - 1) * src_stride_y;
      src_stride_y = -src_stride_y;
    }
    src_u = src_u + (ptrdiff_t)(halfheight - 1) * src_stride_u;
    src_v = src_v + (ptrdiff_t)(halfheight - 1) * src_stride_v;
    src_stride_u = -src_stride_u;
    src_stride_v = -src_stride_v;
  }

  // Convert Y plane.
  if (dst_y) {
    Convert8To16Plane(src_y, src_stride_y, dst_y, dst_stride_y, 4096, width,
                      height);
  }
  // Convert UV planes.
  Convert8To16Plane(src_u, src_stride_u, dst_u, dst_stride_u, 4096, halfwidth,
                    halfheight);
  Convert8To16Plane(src_v, src_stride_v, dst_v, dst_stride_v, 4096, halfwidth,
                    halfheight);
  return 0;
}

// 420 chroma is 1/2 width, 1/2 height
// 422 chroma is 1/2 width, 1x height
LIBYUV_API
int I420ToI422(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_y,
               int dst_stride_y,
               uint8_t* dst_u,
               int dst_stride_u,
               uint8_t* dst_v,
               int dst_stride_v,
               int width,
               int height) {
  const int dst_uv_width = (Abs(width) + 1) >> 1;
  const int dst_uv_height = Abs(height);
  return I420ToI4xx(src_y, src_stride_y, src_u, src_stride_u, src_v,
                    src_stride_v, dst_y, dst_stride_y, dst_u, dst_stride_u,
                    dst_v, dst_stride_v, width, height, dst_uv_width,
                    dst_uv_height);
}

// 420 chroma is 1/2 width, 1/2 height
// 444 chroma is 1x width, 1x height
LIBYUV_API
int I420ToI444(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_y,
               int dst_stride_y,
               uint8_t* dst_u,
               int dst_stride_u,
               uint8_t* dst_v,
               int dst_stride_v,
               int width,
               int height) {
  const int dst_uv_width = Abs(width);
  const int dst_uv_height = Abs(height);
  return I420ToI4xx(src_y, src_stride_y, src_u, src_stride_u, src_v,
                    src_stride_v, dst_y, dst_stride_y, dst_u, dst_stride_u,
                    dst_v, dst_stride_v, width, height, dst_uv_width,
                    dst_uv_height);
}

// 420 chroma to 444 chroma, 10/12 bit version
LIBYUV_API
int I010ToI410(const uint16_t* src_y,
               int src_stride_y,
               const uint16_t* src_u,
               int src_stride_u,
               const uint16_t* src_v,
               int src_stride_v,
               uint16_t* dst_y,
               int dst_stride_y,
               uint16_t* dst_u,
               int dst_stride_u,
               uint16_t* dst_v,
               int dst_stride_v,
               int width,
               int height) {
  int r;
  if ((!src_y && dst_y) || !src_u || !src_v || !dst_u || !dst_v || width <= 0 ||
      height == 0 || height == INT_MIN) {
    return -1;
  }

  if (dst_y) {
    CopyPlane_16(src_y, src_stride_y, dst_y, dst_stride_y, width, height);
  }
  r = ScalePlane_12(src_u, src_stride_u, SUBSAMPLE(width, 1, 1),
                    SUBSAMPLE(height, 1, 1), dst_u, dst_stride_u, width,
                    Abs(height), kFilterBilinear);
  if (r != 0) {
    return r;
  }
  r = ScalePlane_12(src_v, src_stride_v, SUBSAMPLE(width, 1, 1),
                    SUBSAMPLE(height, 1, 1), dst_v, dst_stride_v, width,
                    Abs(height), kFilterBilinear);
  return r;
}

// 422 chroma to 444 chroma, 10/12 bit version
LIBYUV_API
int I210ToI410(const uint16_t* src_y,
               int src_stride_y,
               const uint16_t* src_u,
               int src_stride_u,
               const uint16_t* src_v,
               int src_stride_v,
               uint16_t* dst_y,
               int dst_stride_y,
               uint16_t* dst_u,
               int dst_stride_u,
               uint16_t* dst_v,
               int dst_stride_v,
               int width,
               int height) {
  int r;
  if ((!src_y && dst_y) || !src_u || !src_v || !dst_u || !dst_v || width <= 0 ||
      height == 0 || height == INT_MIN) {
    return -1;
  }

  if (dst_y) {
    CopyPlane_16(src_y, src_stride_y, dst_y, dst_stride_y, width, height);
  }
  r = ScalePlane_12(src_u, src_stride_u, SUBSAMPLE(width, 1, 1), height, dst_u,
                    dst_stride_u, width, Abs(height), kFilterBilinear);
  if (r != 0) {
    return r;
  }
  r = ScalePlane_12(src_v, src_stride_v, SUBSAMPLE(width, 1, 1), height, dst_v,
                    dst_stride_v, width, Abs(height), kFilterBilinear);
  return r;
}

// 422 chroma is 1/2 width, 1x height
// 444 chroma is 1x width, 1x height
LIBYUV_API
int I422ToI444(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_y,
               int dst_stride_y,
               uint8_t* dst_u,
               int dst_stride_u,
               uint8_t* dst_v,
               int dst_stride_v,
               int width,
               int height) {
  int r;
  if ((!src_y && dst_y) || !src_u || !src_v || !dst_u || !dst_v || width <= 0 ||
      height == 0 || height == INT_MIN) {
    return -1;
  }

  if (dst_y) {
    CopyPlane(src_y, src_stride_y, dst_y, dst_stride_y, width, height);
  }
  r = ScalePlane(src_u, src_stride_u, SUBSAMPLE(width, 1, 1), height, dst_u,
                 dst_stride_u, width, Abs(height), kFilterBilinear);
  if (r != 0) {
    return r;
  }
  r = ScalePlane(src_v, src_stride_v, SUBSAMPLE(width, 1, 1), height, dst_v,
                 dst_stride_v, width, Abs(height), kFilterBilinear);
  return r;
}

// Copy to I400. Source can be I420,422,444,400,NV12,NV21
LIBYUV_API
int I400Copy(const uint8_t* src_y,
             int src_stride_y,
             uint8_t* dst_y,
             int dst_stride_y,
             int width,
             int height) {
  if (!src_y || !dst_y || width <= 0 || height == 0 || height == INT_MIN) {
    return -1;
  }
  CopyPlane(src_y, src_stride_y, dst_y, dst_stride_y, width, height);
  return 0;
}

LIBYUV_API
int I422ToYUY2(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_yuy2,
               int dst_stride_yuy2,
               int width,
               int height) {
  int y;
  void (*I422ToYUY2Row)(const uint8_t* src_y, const uint8_t* src_u,
                        const uint8_t* src_v, uint8_t* dst_yuy2, int width) =
      I422ToYUY2Row_C;
  if (!src_y || !src_u || !src_v || !dst_yuy2 || width <= 0 || height == 0 ||
      height == INT_MIN) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_yuy2 = dst_yuy2 + (ptrdiff_t)(height - 1) * dst_stride_yuy2;
    dst_stride_yuy2 = -dst_stride_yuy2;
  }
  // Coalesce rows.
  if (src_stride_y == width && src_stride_u * 2 == width &&
      src_stride_v * 2 == width && dst_stride_yuy2 == width * 2 &&
      (ptrdiff_t)width * height <= INT_MAX) {
    width *= height;
    height = 1;
    src_stride_y = src_stride_u = src_stride_v = dst_stride_yuy2 = 0;
  }
#if defined(HAS_I422TOYUY2ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_SSE2;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_SSE2;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      I422ToYUY2Row = I422ToYUY2Row_AVX2;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_NEON;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_LSX;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_LSX;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_LASX;
    if (IS_ALIGNED(width, 32)) {
      I422ToYUY2Row = I422ToYUY2Row_LASX;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_WASMSIMD;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_WASMSIMD;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    I422ToYUY2Row(src_y, src_u, src_v, dst_yuy2, width);
    src_y += src_stride_y;
    src_u += src_stride_u;
    src_v += src_stride_v;
    dst_yuy2 += dst_stride_yuy2;
  }
  return 0;
}

LIBYUV_API
int I420ToYUY2(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_yuy2,
               int dst_stride_yuy2,
               int width,
               int height) {
  int y;
  void (*I422ToYUY2Row)(const uint8_t* src_y, const uint8_t* src_u,
                        const uint8_t* src_v, uint8_t* dst_yuy2, int width) =
      I422ToYUY2Row_C;
  if (!src_y || !src_u || !src_v || !dst_yuy2 || width <= 0 || height == 0 ||
      height == INT_MIN) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_yuy2 = dst_yuy2 + (ptrdiff_t)(height - 1) * dst_stride_yuy2;
    dst_stride_yuy2 = -dst_stride_yuy2;
  }
#if defined(HAS_I422TOYUY2ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_SSE2;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_SSE2;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      I422ToYUY2Row = I422ToYUY2Row_AVX2;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_NEON;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_LSX;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_LSX;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_LASX;
    if (IS_ALIGNED(width, 32)) {
      I422ToYUY2Row = I422ToYUY2Row_LASX;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_WASMSIMD;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_WASMSIMD;
    }
  }
#endif

  for (y = 0; y < height - 1; y += 2) {
    I422ToYUY2Row(src_y, src_u, src_v, dst_yuy2, width);
    I422ToYUY2Row(src_y + src_stride_y, src_u, src_v,
                  dst_yuy2 + dst_stride_yuy2, width);
    src_y += src_stride_y * 2;
    src_u += src_stride_u;
    src_v += src_stride_v;
    dst_yuy2 += dst_stride_yuy2 * 2;
  }
  if (height & 1) {
    I422ToYUY2Row(src_y, src_u, src_v, dst_yuy2, width);
  }
  return 0;
}

LIBYUV_API
int I422ToUYVY(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_uyvy,
               int dst_stride_uyvy,
               int width,
               int height) {
  int y;
  void (*I422ToUYVYRow)(const uint8_t* src_y, const uint8_t* src_u,
                        const uint8_t* src_v, uint8_t* dst_uyvy, int width) =
      I422ToUYVYRow_C;
  if (!src_y || !src_u || !src_v || !dst_uyvy || width <= 0 || height == 0 ||
      height == INT_MIN) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_uyvy = dst_uyvy + (ptrdiff_t)(height - 1) * dst_stride_uyvy;
    dst_stride_uyvy = -dst_stride_uyvy;
  }
  // Coalesce rows.
  if (src_stride_y == width && src_stride_u * 2 == width &&
      src_stride_v * 2 == width && dst_stride_uyvy == width * 2 &&
      (ptrdiff_t)width * height <= INT_MAX) {
    width *= height;
    height = 1;
    src_stride_y = src_stride_u = src_stride_v = dst_stride_uyvy = 0;
  }
#if defined(HAS_I422TOUYVYROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_SSE2;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_SSE2;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      I422ToUYVYRow = I422ToUYVYRow_AVX2;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_NEON;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_LSX;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_LSX;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_LASX;
    if (IS_ALIGNED(width, 32)) {
      I422ToUYVYRow = I422ToUYVYRow_LASX;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_WASMSIMD;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_WASMSIMD;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    I422ToUYVYRow(src_y, src_u, src_v, dst_uyvy, width);
    src_y += src_stride_y;
    src_u += src_stride_u;
    src_v += src_stride_v;
    dst_uyvy += dst_stride_uyvy;
  }
  return 0;
}

LIBYUV_API
int I420ToUYVY(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_uyvy,
               int dst_stride_uyvy,
               int width,
               int height) {
  int y;
  void (*I422ToUYVYRow)(const uint8_t* src_y, const uint8_t* src_u,
                        const uint8_t* src_v, uint8_t* dst_uyvy, int width) =
      I422ToUYVYRow_C;
  if (!src_y || !src_u || !src_v || !dst_uyvy || width <= 0 || height == 0 ||
      height == INT_MIN) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_uyvy = dst_uyvy + (ptrdiff_t)(height - 1) * dst_stride_uyvy;
    dst_stride_uyvy = -dst_stride_uyvy;
  }
#if defined(HAS_I422TOUYVYROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_SSE2;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_SSE2;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      I422ToUYVYRow = I422ToUYVYRow_AVX2;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_NEON;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_LSX;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_LSX;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_LASX;
    if (IS_ALIGNED(width, 32)) {
      I422ToUYVYRow = I422ToUYVYRow_LASX;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_WASMSIMD;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_WASMSIMD;
    }
  }
#endif

  for (y = 0; y < height - 1; y += 2) {
    I422ToUYVYRow(src_y, src_u, src_v, dst_uyvy, width);
    I422ToUYVYRow(src_y + src_stride_y, src_u, src_v,
                  dst_uyvy + dst_stride_uyvy, width);
    src_y += src_stride_y * 2;
    src_u += src_stride_u;
    src_v += src_stride_v;
    dst_uyvy += dst_stride_uyvy * 2;
  }
  if (height & 1) {
    I422ToUYVYRow(src_y, src_u, src_v, dst_uyvy, width);
  }
  return 0;
}

LIBYUV_API
int I420ToNV12(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_y,
               int dst_stride_y,
               uint8_t* dst_uv,
               int dst_stride_uv,
               int width,
               int height) {
  int halfwidth = (width + 1) / 2;
  int halfheight = (height + 1) / 2;
  if ((!src_y && dst_y) || !src_u || !src_v || !dst_uv || width <= 0 ||
      height == 0 || height == INT_MIN) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    halfheight = (height + 1) >> 1;
    if (src_y) {
      src_y = src_y + (ptrdiff_t)(height - 1) * src_stride_y;
      src_stride_y = -src_stride_y;
    }
    src_u = src_u + (ptrdiff_t)(halfheight - 1) * src_stride_u;
    src_v = src_v + (ptrdiff_t)(halfheight - 1) * src_stride_v;
    src_stride_u = -src_stride_u;
    src_stride_v = -src_stride_v;
  }
  if (dst_y) {
    CopyPlane(src_y, src_stride_y, dst_y, dst_stride_y, width, height);
  }
  MergeUVPlane(src_u, src_stride_u, src_v, src_stride_v, dst_uv, dst_stride_uv,
               halfwidth, halfheight);
  return 0;
}

LIBYUV_API
int I420ToNV21(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_y,
               int dst_stride_y,
               uint8_t* dst_vu,
               int dst_stride_vu,
               int width,
               int height) {
  return I420ToNV12(src_y, src_stride_y, src_v, src_stride_v, src_u,
                    src_stride_u, dst_y, dst_stride_y, dst_vu, dst_stride_vu,
                    width, height);
}

static void Convert8To10MSBPlane(const uint8_t* src_y,
                                 int src_stride_y,
                                 uint16_t* dst_y,
                                 int dst_stride_y,
                                 int width,
                                 int height) {
  int y;
  int row_width = width;
  // Coalesce contiguous rows into L1-resident chunks (up to 8192 samples =
  // 16 KB of dst_y) to minimize per-row tail/call overhead while avoiding a
  // second full-frame memory pass.
  if (src_stride_y == width && dst_stride_y == width &&
      (ptrdiff_t)width * height <= INT_MAX) {
    int total = width * height;
    if (total <= 8192) {
      width = total;
      height = 1;
      src_stride_y = dst_stride_y = 0;
      row_width = width;
    } else if (width < 4096) {
      int chunk_rows = 8192 / width;
      row_width = width * chunk_rows;
    }
  }
  void (*Convert8To16Row)(const uint8_t* src_y, uint16_t* dst_y, int bits,
                          int width) = Convert8To16Row_C;
  void (*MultiplyRow_16)(const uint16_t* src_y, uint16_t* dst_y, int scale,
                         int width) = MultiplyRow_16_C;
#if defined(HAS_CONVERT8TO16ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    Convert8To16Row = Convert8To16Row_Any_SSE2;
    if (IS_ALIGNED(row_width, 16) && IS_ALIGNED(width, 16)) {
      Convert8To16Row = Convert8To16Row_SSE2;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    Convert8To16Row = Convert8To16Row_Any_AVX2;
    if (IS_ALIGNED(row_width, 32) && IS_ALIGNED(width, 32)) {
      Convert8To16Row = Convert8To16Row_AVX2;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_AVX512BW)
  if (TestCpuFlag(kCpuHasAVX512BW)) {
    Convert8To16Row = Convert8To16Row_AVX512BW;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    Convert8To16Row = Convert8To16Row_Any_NEON;
    if (IS_ALIGNED(row_width, 16) && IS_ALIGNED(width, 16)) {
      Convert8To16Row = Convert8To16Row_NEON;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_SME)
  if (TestCpuFlag(kCpuHasSME)) {
    Convert8To16Row = Convert8To16Row_SME;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_RVV)
  if (TestCpuFlag(kCpuHasRVV)) {
    Convert8To16Row = Convert8To16Row_RVV;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_HVX)
  if (TestCpuFlag(kCpuHasHVX)) {
    Convert8To16Row = Convert8To16Row_HVX;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    Convert8To16Row = Convert8To16Row_Any_WASMSIMD;
    if (IS_ALIGNED(row_width, 16) && IS_ALIGNED(width, 16)) {
      Convert8To16Row = Convert8To16Row_WASMSIMD;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    Convert8To16Row = Convert8To16Row_Any_LSX;
    if (IS_ALIGNED(row_width, 16) && IS_ALIGNED(width, 16)) {
      Convert8To16Row = Convert8To16Row_LSX;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    Convert8To16Row = Convert8To16Row_Any_LASX;
    if (IS_ALIGNED(row_width, 32) && IS_ALIGNED(width, 32)) {
      Convert8To16Row = Convert8To16Row_LASX;
    }
  }
#endif
#if defined(HAS_MULTIPLYROW_16_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    MultiplyRow_16 = MultiplyRow_16_Any_AVX2;
    if (IS_ALIGNED(row_width, 32) && IS_ALIGNED(width, 32)) {
      MultiplyRow_16 = MultiplyRow_16_AVX2;
    }
  }
#endif
#if defined(HAS_MULTIPLYROW_16_AVX512BW)
  if (TestCpuFlag(kCpuHasAVX512BW)) {
    MultiplyRow_16 = MultiplyRow_16_AVX512BW;
  }
#endif
#if defined(HAS_MULTIPLYROW_16_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    MultiplyRow_16 = MultiplyRow_16_Any_NEON;
    if (IS_ALIGNED(row_width, 16) && IS_ALIGNED(width, 16)) {
      MultiplyRow_16 = MultiplyRow_16_NEON;
    }
  }
#endif
#if defined(HAS_MULTIPLYROW_16_SME)
  if (TestCpuFlag(kCpuHasSME)) {
    MultiplyRow_16 = MultiplyRow_16_SME;
  }
#endif
#if defined(HAS_MULTIPLYROW_16_RVV)
  if (TestCpuFlag(kCpuHasRVV)) {
    MultiplyRow_16 = MultiplyRow_16_RVV;
  }
#endif
#if defined(HAS_MULTIPLYROW_16_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    MultiplyRow_16 = MultiplyRow_16_Any_WASMSIMD;
    if (IS_ALIGNED(row_width, 16) && IS_ALIGNED(width, 16)) {
      MultiplyRow_16 = MultiplyRow_16_WASMSIMD;
    }
  }
#endif
#if defined(HAS_MULTIPLYROW_16_HVX)
  if (TestCpuFlag(kCpuHasHVX)) {
    MultiplyRow_16 = MultiplyRow_16_HVX;
  }
#endif
#if defined(HAS_MULTIPLYROW_16_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    MultiplyRow_16 = MultiplyRow_16_Any_LSX;
    if (IS_ALIGNED(row_width, 16) && IS_ALIGNED(width, 16)) {
      MultiplyRow_16 = MultiplyRow_16_LSX;
    }
  }
#endif
#if defined(HAS_MULTIPLYROW_16_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    MultiplyRow_16 = MultiplyRow_16_Any_LASX;
    if (IS_ALIGNED(row_width, 32) && IS_ALIGNED(width, 32)) {
      MultiplyRow_16 = MultiplyRow_16_LASX;
    }
  }
#endif

  if (row_width > width) {
    int chunk_rows = row_width / width;
    for (y = 0; y <= height - chunk_rows; y += chunk_rows) {
      Convert8To16Row(src_y, dst_y, 10, row_width);
      MultiplyRow_16(dst_y, dst_y, 64, row_width);
      src_y += row_width;
      dst_y += row_width;
    }
    int rem = (height - y) * width;
    if (rem > 0) {
      Convert8To16Row(src_y, dst_y, 10, rem);
      MultiplyRow_16(dst_y, dst_y, 64, rem);
    }
    return;
  }

  for (y = 0; y < height; ++y) {
    Convert8To16Row(src_y, dst_y, 10, width);
    MultiplyRow_16(dst_y, dst_y, 64, width);
    src_y += src_stride_y;
    dst_y += dst_stride_y;
  }
}

static int I4xxToPxxx(const uint8_t* src_y,
                      int src_stride_y,
                      const uint8_t* src_u,
                      int src_stride_u,
                      const uint8_t* src_v,
                      int src_stride_v,
                      uint16_t* dst_y,
                      int dst_stride_y,
                      uint16_t* dst_uv,
                      int dst_stride_uv,
                      int width,
                      int height,
                      int subsample_x,
                      int subsample_y) {
  int y;
  int uv_width;
  int uv_height;
  int row_stride;
  void (*Convert8To16Row)(const uint8_t* src_y, uint16_t* dst_y, int bits,
                          int width) = Convert8To16Row_C;
  void (*MergeUVRow_16)(const uint16_t* src_u, const uint16_t* src_v,
                        uint16_t* dst_uv, int depth, int width) =
      MergeUVRow_16_C;
  if ((!src_y && dst_y) || !src_u || !src_v || !dst_y || !dst_uv ||
      width <= 0 || height == 0 || height == INT_MIN) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    uv_height = SUBSAMPLE(height, subsample_y, subsample_y);
    src_y = src_y + (ptrdiff_t)(height - 1) * src_stride_y;
    src_u = src_u + (ptrdiff_t)(uv_height - 1) * src_stride_u;
    src_v = src_v + (ptrdiff_t)(uv_height - 1) * src_stride_v;
    src_stride_y = -src_stride_y;
    src_stride_u = -src_stride_u;
    src_stride_v = -src_stride_v;
  } else {
    uv_height = SUBSAMPLE(height, subsample_y, subsample_y);
  }
  uv_width = SUBSAMPLE(width, subsample_x, subsample_x);

  Convert8To10MSBPlane(src_y, src_stride_y, dst_y, dst_stride_y, width, height);

#if defined(HAS_CONVERT8TO16ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    Convert8To16Row = Convert8To16Row_Any_SSE2;
    if (IS_ALIGNED(uv_width, 16)) {
      Convert8To16Row = Convert8To16Row_SSE2;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    Convert8To16Row = Convert8To16Row_Any_AVX2;
    if (IS_ALIGNED(uv_width, 32)) {
      Convert8To16Row = Convert8To16Row_AVX2;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_AVX512BW)
  if (TestCpuFlag(kCpuHasAVX512BW)) {
    Convert8To16Row = Convert8To16Row_AVX512BW;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    Convert8To16Row = Convert8To16Row_Any_NEON;
    if (IS_ALIGNED(uv_width, 16)) {
      Convert8To16Row = Convert8To16Row_NEON;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_SME)
  if (TestCpuFlag(kCpuHasSME)) {
    Convert8To16Row = Convert8To16Row_SME;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_RVV)
  if (TestCpuFlag(kCpuHasRVV)) {
    Convert8To16Row = Convert8To16Row_RVV;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_HVX)
  if (TestCpuFlag(kCpuHasHVX)) {
    Convert8To16Row = Convert8To16Row_HVX;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    Convert8To16Row = Convert8To16Row_Any_WASMSIMD;
    if (IS_ALIGNED(uv_width, 16)) {
      Convert8To16Row = Convert8To16Row_WASMSIMD;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    Convert8To16Row = Convert8To16Row_Any_LSX;
    if (IS_ALIGNED(uv_width, 16)) {
      Convert8To16Row = Convert8To16Row_LSX;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    Convert8To16Row = Convert8To16Row_Any_LASX;
    if (IS_ALIGNED(uv_width, 32)) {
      Convert8To16Row = Convert8To16Row_LASX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    MergeUVRow_16 = MergeUVRow_16_Any_AVX2;
    if (IS_ALIGNED(uv_width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_AVX2;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_AVX512BW)
  if (TestCpuFlag(kCpuHasAVX512BW)) {
    MergeUVRow_16 = MergeUVRow_16_AVX512BW;
  }
#endif
#if defined(HAS_MERGEUVROW_16_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    MergeUVRow_16 = MergeUVRow_16_Any_NEON;
    if (IS_ALIGNED(uv_width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_NEON;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_SME)
  if (TestCpuFlag(kCpuHasSME)) {
    MergeUVRow_16 = MergeUVRow_16_SME;
  }
#endif
#if defined(HAS_MERGEUVROW_16_HVX)
  if (TestCpuFlag(kCpuHasHVX)) {
    MergeUVRow_16 = MergeUVRow_16_HVX;
  }
#endif
#if defined(HAS_MERGEUVROW_16_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    MergeUVRow_16 = MergeUVRow_16_Any_LSX;
    if (IS_ALIGNED(uv_width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_LSX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    MergeUVRow_16 = MergeUVRow_16_Any_LASX;
    if (IS_ALIGNED(uv_width, 16)) {
      MergeUVRow_16 = MergeUVRow_16_LASX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    MergeUVRow_16 = MergeUVRow_16_Any_WASMSIMD;
    if (IS_ALIGNED(uv_width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_WASMSIMD;
    }
  }
#endif

  row_stride = (uv_width + 31) & ~31;
  align_buffer_64(row_u, (size_t)row_stride * sizeof(uint16_t) * 2u);
  if (!row_u) {
    return 1;
  }
  {
    uint16_t* row_u16 = (uint16_t*)row_u;
    uint16_t* row_v16 = row_u16 + row_stride;
    for (y = 0; y < uv_height; ++y) {
      Convert8To16Row(src_u, row_u16, 10, uv_width);
      Convert8To16Row(src_v, row_v16, 10, uv_width);
      MergeUVRow_16(row_u16, row_v16, dst_uv, 10, uv_width);
      src_u += src_stride_u;
      src_v += src_stride_v;
      dst_uv += dst_stride_uv;
    }
  }
  free_aligned_buffer_64(row_u);
  return 0;
}

LIBYUV_API
int I420ToP010(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint16_t* dst_y,
               int dst_stride_y,
               uint16_t* dst_uv,
               int dst_stride_uv,
               int width,
               int height) {
  return I4xxToPxxx(src_y, src_stride_y, src_u, src_stride_u, src_v,
                    src_stride_v, dst_y, dst_stride_y, dst_uv, dst_stride_uv,
                    width, height, 1, 1);
}

LIBYUV_API
int I422ToP210(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint16_t* dst_y,
               int dst_stride_y,
               uint16_t* dst_uv,
               int dst_stride_uv,
               int width,
               int height) {
  return I4xxToPxxx(src_y, src_stride_y, src_u, src_stride_u, src_v,
                    src_stride_v, dst_y, dst_stride_y, dst_uv, dst_stride_uv,
                    width, height, 1, 0);
}

LIBYUV_API
int I444ToP410(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint16_t* dst_y,
               int dst_stride_y,
               uint16_t* dst_uv,
               int dst_stride_uv,
               int width,
               int height) {
  return I4xxToPxxx(src_y, src_stride_y, src_u, src_stride_u, src_v,
                    src_stride_v, dst_y, dst_stride_y, dst_uv, dst_stride_uv,
                    width, height, 0, 0);
}

LIBYUV_API
int I420ToP210(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint16_t* dst_y,
               int dst_stride_y,
               uint16_t* dst_uv,
               int dst_stride_uv,
               int width,
               int height) {
  int abs_height;
  int uv_width;
  int src_uv_height;
  size_t row_stride;
  size_t row8_stride;
  void (*InterpolateRow)(uint8_t* dst_ptr, const uint8_t* src_ptr,
                         ptrdiff_t src_stride, int dst_width,
                         int source_y_fraction) = InterpolateRow_C;
  void (*Convert8To16Row)(const uint8_t* src_y, uint16_t* dst_y, int bits,
                          int width) = Convert8To16Row_C;
  void (*MergeUVRow_16)(const uint16_t* src_u, const uint16_t* src_v,
                        uint16_t* dst_uv, int depth, int width) =
      MergeUVRow_16_C;
  if (!src_y || !src_u || !src_v || !dst_y || !dst_uv || width <= 0 ||
      height == 0 || height == INT_MIN) {
    return -1;
  }
  abs_height = Abs(height);
  uv_width = SUBSAMPLE(width, 1, 1);
  src_uv_height = SUBSAMPLE(abs_height, 1, 1);
  if (uv_width > 32768 || src_uv_height > 32768) {
    return -1;
  }
  if (height < 0) {
    src_y = src_y + (ptrdiff_t)(abs_height - 1) * src_stride_y;
    src_u = src_u + (ptrdiff_t)(src_uv_height - 1) * src_stride_u;
    src_v = src_v + (ptrdiff_t)(src_uv_height - 1) * src_stride_v;
    src_stride_y = -src_stride_y;
    src_stride_u = -src_stride_u;
    src_stride_v = -src_stride_v;
  }

  Convert8To10MSBPlane(src_y, src_stride_y, dst_y, dst_stride_y, width,
                       abs_height);

#if defined(HAS_INTERPOLATEROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    InterpolateRow = InterpolateRow_Any_AVX2;
    if (IS_ALIGNED(uv_width, 32)) {
      InterpolateRow = InterpolateRow_AVX2;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_AVX512BW)
  if (TestCpuFlag(kCpuHasAVX512BW)) {
    InterpolateRow = InterpolateRow_AVX512BW;
  }
#endif
#if defined(HAS_INTERPOLATEROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    InterpolateRow = InterpolateRow_Any_NEON;
    if (IS_ALIGNED(uv_width, 16)) {
      InterpolateRow = InterpolateRow_NEON;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_SVE2)
  if (TestCpuFlag(kCpuHasSVE2)) {
    InterpolateRow = InterpolateRow_SVE2;
  }
#endif
#if defined(HAS_INTERPOLATEROW_SME)
  if (TestCpuFlag(kCpuHasSME)) {
    InterpolateRow = InterpolateRow_SME;
  }
#endif
#if defined(HAS_INTERPOLATEROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    InterpolateRow = InterpolateRow_Any_LSX;
    if (IS_ALIGNED(uv_width, 16)) {
      InterpolateRow = InterpolateRow_LSX;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    InterpolateRow = InterpolateRow_Any_LASX;
    if (IS_ALIGNED(uv_width, 32)) {
      InterpolateRow = InterpolateRow_LASX;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_RVV)
  if (TestCpuFlag(kCpuHasRVV)) {
    InterpolateRow = InterpolateRow_RVV;
  }
#endif
#if defined(HAS_INTERPOLATEROW_HVX)
  if (TestCpuFlag(kCpuHasHVX)) {
    InterpolateRow = InterpolateRow_HVX;
  }
#endif
#if defined(HAS_INTERPOLATEROW_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    InterpolateRow = InterpolateRow_Any_WASMSIMD;
    if (IS_ALIGNED(uv_width, 16)) {
      InterpolateRow = InterpolateRow_WASMSIMD;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    Convert8To16Row = Convert8To16Row_Any_SSE2;
    if (IS_ALIGNED(uv_width, 16)) {
      Convert8To16Row = Convert8To16Row_SSE2;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    Convert8To16Row = Convert8To16Row_Any_AVX2;
    if (IS_ALIGNED(uv_width, 32)) {
      Convert8To16Row = Convert8To16Row_AVX2;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_AVX512BW)
  if (TestCpuFlag(kCpuHasAVX512BW)) {
    Convert8To16Row = Convert8To16Row_AVX512BW;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    Convert8To16Row = Convert8To16Row_Any_NEON;
    if (IS_ALIGNED(uv_width, 16)) {
      Convert8To16Row = Convert8To16Row_NEON;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_SME)
  if (TestCpuFlag(kCpuHasSME)) {
    Convert8To16Row = Convert8To16Row_SME;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_RVV)
  if (TestCpuFlag(kCpuHasRVV)) {
    Convert8To16Row = Convert8To16Row_RVV;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_HVX)
  if (TestCpuFlag(kCpuHasHVX)) {
    Convert8To16Row = Convert8To16Row_HVX;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    Convert8To16Row = Convert8To16Row_Any_WASMSIMD;
    if (IS_ALIGNED(uv_width, 16)) {
      Convert8To16Row = Convert8To16Row_WASMSIMD;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    Convert8To16Row = Convert8To16Row_Any_LSX;
    if (IS_ALIGNED(uv_width, 16)) {
      Convert8To16Row = Convert8To16Row_LSX;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    Convert8To16Row = Convert8To16Row_Any_LASX;
    if (IS_ALIGNED(uv_width, 32)) {
      Convert8To16Row = Convert8To16Row_LASX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    MergeUVRow_16 = MergeUVRow_16_Any_AVX2;
    if (IS_ALIGNED(uv_width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_AVX2;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_AVX512BW)
  if (TestCpuFlag(kCpuHasAVX512BW)) {
    MergeUVRow_16 = MergeUVRow_16_AVX512BW;
  }
#endif
#if defined(HAS_MERGEUVROW_16_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    MergeUVRow_16 = MergeUVRow_16_Any_NEON;
    if (IS_ALIGNED(uv_width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_NEON;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_SME)
  if (TestCpuFlag(kCpuHasSME)) {
    MergeUVRow_16 = MergeUVRow_16_SME;
  }
#endif
#if defined(HAS_MERGEUVROW_16_HVX)
  if (TestCpuFlag(kCpuHasHVX)) {
    MergeUVRow_16 = MergeUVRow_16_HVX;
  }
#endif
#if defined(HAS_MERGEUVROW_16_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    MergeUVRow_16 = MergeUVRow_16_Any_LSX;
    if (IS_ALIGNED(uv_width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_LSX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    MergeUVRow_16 = MergeUVRow_16_Any_LASX;
    if (IS_ALIGNED(uv_width, 16)) {
      MergeUVRow_16 = MergeUVRow_16_LASX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    MergeUVRow_16 = MergeUVRow_16_Any_WASMSIMD;
    if (IS_ALIGNED(uv_width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_WASMSIMD;
    }
  }
#endif

  row_stride = ((size_t)uv_width + 31u) & ~31u;
  row8_stride = ((size_t)uv_width + 63u) & ~63u;
  align_buffer_64(row_buf,
                  row_stride * sizeof(uint16_t) * 2u + row8_stride * 2u);
  if (!row_buf) {
    return 1;
  }
  {
    uint16_t* row_u16 = (uint16_t*)row_buf;
    uint16_t* row_v16 = row_u16 + row_stride;
    uint8_t* row_u8 = row_buf + row_stride * sizeof(uint16_t) * 2u;
    uint8_t* row_v8 = row_u8 + row8_stride;
    const bool interpolate = (uv_width > 1 && src_uv_height > 1);
    int dy = 0;
    int64_t y64 = 0;
    int64_t max_y = 0;
    if (src_uv_height > 1) {
      max_y = ((int64_t)src_uv_height - 1) << 16;
      if (interpolate) {
        --max_y;
        dy = FixedDiv1(src_uv_height, abs_height);
      } else {
        dy = FixedDiv(src_uv_height, abs_height);
        y64 = dy >> 1;
      }
    }
    for (int j = 0; j < abs_height; ++j) {
      int yi;
      int yf;
      if (y64 > max_y) {
        y64 = max_y;
      }
      yi = (int)(y64 >> 16);
      yf = interpolate ? (int)((y64 >> 8) & 255) : 0;
      if (yf == 0) {
        Convert8To16Row(src_u + yi * (ptrdiff_t)src_stride_u, row_u16, 10,
                        uv_width);
        Convert8To16Row(src_v + yi * (ptrdiff_t)src_stride_v, row_v16, 10,
                        uv_width);
      } else {
        InterpolateRow(row_u8, src_u + yi * (ptrdiff_t)src_stride_u,
                       src_stride_u, uv_width, yf);
        InterpolateRow(row_v8, src_v + yi * (ptrdiff_t)src_stride_v,
                       src_stride_v, uv_width, yf);
        Convert8To16Row(row_u8, row_u16, 10, uv_width);
        Convert8To16Row(row_v8, row_v16, 10, uv_width);
      }
      MergeUVRow_16(row_u16, row_v16, dst_uv, 10, uv_width);
      dst_uv += dst_stride_uv;
      y64 += dy;
    }
  }
  free_aligned_buffer_64(row_buf);
  return 0;
}

LIBYUV_API
int I420ToP410(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint16_t* dst_y,
               int dst_stride_y,
               uint16_t* dst_uv,
               int dst_stride_uv,
               int width,
               int height) {
  int abs_height;
  int src_uv_width;
  int src_uv_height;
  size_t row_stride;
  size_t row8_stride;
  void (*Scale2RowUp)(const uint8_t* src_ptr, ptrdiff_t src_stride,
                      uint8_t* dst_ptr, ptrdiff_t dst_stride, int dst_width) =
      ScaleRowUp2_Bilinear_Any_C;
  void (*Convert8To16Row)(const uint8_t* src_y, uint16_t* dst_y, int bits,
                          int width) = Convert8To16Row_C;
  void (*MergeUVRow_16)(const uint16_t* src_u, const uint16_t* src_v,
                        uint16_t* dst_uv, int depth, int width) =
      MergeUVRow_16_C;
  if (!src_y || !src_u || !src_v || !dst_y || !dst_uv || width <= 0 ||
      height == 0 || height == INT_MIN) {
    return -1;
  }
  abs_height = Abs(height);
  src_uv_width = SUBSAMPLE(width, 1, 1);
  src_uv_height = SUBSAMPLE(abs_height, 1, 1);
  if (src_uv_width > 32768 || src_uv_height > 32768) {
    return -1;
  }
  if (height < 0) {
    src_y = src_y + (ptrdiff_t)(abs_height - 1) * src_stride_y;
    src_u = src_u + (ptrdiff_t)(src_uv_height - 1) * src_stride_u;
    src_v = src_v + (ptrdiff_t)(src_uv_height - 1) * src_stride_v;
    src_stride_y = -src_stride_y;
    src_stride_u = -src_stride_u;
    src_stride_v = -src_stride_v;
  }

  Convert8To10MSBPlane(src_y, src_stride_y, dst_y, dst_stride_y, width,
                       abs_height);

  // Match ScaleFilterReduce: 1-column chroma reduces to point sampling.
  if (src_uv_width == 1) {
    int dy = (abs_height == 1) ? 0 : FixedDiv(src_uv_height, abs_height);
    uint32_t uy = (uint32_t)(dy >> 1);
    for (int j = 0; j < abs_height; ++j) {
      int yi = (int)(uy >> 16);
      uint16_t u16 =
          (uint16_t)(((src_u[yi * (ptrdiff_t)src_stride_u] * 0x0101) >> 6)
                     << 6);
      uint16_t v16 =
          (uint16_t)(((src_v[yi * (ptrdiff_t)src_stride_v] * 0x0101) >> 6)
                     << 6);
      dst_uv[0] = u16;
      dst_uv[1] = v16;
      if (width == 2) {
        dst_uv[2] = u16;
        dst_uv[3] = v16;
      }
      dst_uv += dst_stride_uv;
      uy += (uint32_t)dy;
    }
    return 0;
  }

#ifdef HAS_SCALEROWUP2_BILINEAR_SSE2
  if (TestCpuFlag(kCpuHasSSE2)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_Any_SSE2;
  }
#endif
#ifdef HAS_SCALEROWUP2_BILINEAR_SSSE3
  if (TestCpuFlag(kCpuHasSSSE3)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_Any_SSSE3;
  }
#endif
#ifdef HAS_SCALEROWUP2_BILINEAR_AVX2
  if (TestCpuFlag(kCpuHasAVX2)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_Any_AVX2;
  }
#endif
#ifdef HAS_SCALEROWUP2_BILINEAR_NEON
  if (TestCpuFlag(kCpuHasNEON)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_Any_NEON;
  }
#endif
#ifdef HAS_SCALEROWUP2_BILINEAR_RVV
  if (TestCpuFlag(kCpuHasRVV)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_RVV;
  }
#endif
#ifdef HAS_SCALEROWUP2_BILINEAR_WASMSIMD
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_Any_WASMSIMD;
  }
#endif
#ifdef HAS_SCALEROWUP2_BILINEAR_RELAXEDSIMD
  if (TestCpuFlag(kCpuHasRELAXEDSIMD)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_Any_RELAXEDSIMD;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    Convert8To16Row = Convert8To16Row_Any_SSE2;
    if (IS_ALIGNED(width, 16)) {
      Convert8To16Row = Convert8To16Row_SSE2;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    Convert8To16Row = Convert8To16Row_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      Convert8To16Row = Convert8To16Row_AVX2;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_AVX512BW)
  if (TestCpuFlag(kCpuHasAVX512BW)) {
    Convert8To16Row = Convert8To16Row_AVX512BW;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    Convert8To16Row = Convert8To16Row_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      Convert8To16Row = Convert8To16Row_NEON;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_RVV)
  if (TestCpuFlag(kCpuHasRVV)) {
    Convert8To16Row = Convert8To16Row_RVV;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_HVX)
  if (TestCpuFlag(kCpuHasHVX)) {
    Convert8To16Row = Convert8To16Row_HVX;
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    Convert8To16Row = Convert8To16Row_Any_WASMSIMD;
    if (IS_ALIGNED(width, 16)) {
      Convert8To16Row = Convert8To16Row_WASMSIMD;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    Convert8To16Row = Convert8To16Row_Any_LSX;
    if (IS_ALIGNED(width, 16)) {
      Convert8To16Row = Convert8To16Row_LSX;
    }
  }
#endif
#if defined(HAS_CONVERT8TO16ROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    Convert8To16Row = Convert8To16Row_Any_LASX;
    if (IS_ALIGNED(width, 32)) {
      Convert8To16Row = Convert8To16Row_LASX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    MergeUVRow_16 = MergeUVRow_16_Any_AVX2;
    if (IS_ALIGNED(width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_AVX2;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_AVX512BW)
  if (TestCpuFlag(kCpuHasAVX512BW)) {
    MergeUVRow_16 = MergeUVRow_16_AVX512BW;
  }
#endif
#if defined(HAS_MERGEUVROW_16_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    MergeUVRow_16 = MergeUVRow_16_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_NEON;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_HVX)
  if (TestCpuFlag(kCpuHasHVX)) {
    MergeUVRow_16 = MergeUVRow_16_HVX;
  }
#endif
#if defined(HAS_MERGEUVROW_16_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    MergeUVRow_16 = MergeUVRow_16_Any_LSX;
    if (IS_ALIGNED(width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_LSX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    MergeUVRow_16 = MergeUVRow_16_Any_LASX;
    if (IS_ALIGNED(width, 16)) {
      MergeUVRow_16 = MergeUVRow_16_LASX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_16_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    MergeUVRow_16 = MergeUVRow_16_Any_WASMSIMD;
    if (IS_ALIGNED(width, 8)) {
      MergeUVRow_16 = MergeUVRow_16_WASMSIMD;
    }
  }
#endif

  row_stride = ((size_t)width * 2u + 31u) & ~31u;
  row8_stride = ((size_t)width * 2u + 63u) & ~63u;
  align_buffer_64(row_buf,
                  row_stride * sizeof(uint16_t) * 2u + row8_stride * 2u);
  if (!row_buf) {
    return 1;
  }
  {
    uint16_t* row_u16 = (uint16_t*)row_buf;
    uint16_t* row_v16 = row_u16 + row_stride;
    uint8_t* row_u0 = row_buf + row_stride * sizeof(uint16_t) * 2u;
    uint8_t* row_v0 = row_u0 + row8_stride;
    const bool contig_dst_uv = (dst_stride_uv == width * 2);

    Scale2RowUp(src_u, 0, row_u0, 0, width);
    Scale2RowUp(src_v, 0, row_v0, 0, width);
    Convert8To16Row(row_u0, row_u16, 10, width);
    Convert8To16Row(row_v0, row_v16, 10, width);
    MergeUVRow_16(row_u16, row_v16, dst_uv, 10, width);
    dst_uv += dst_stride_uv;

    for (int x = 0; x < src_uv_height - 1; ++x) {
      Scale2RowUp(src_u, src_stride_u, row_u0, width, width);
      Scale2RowUp(src_v, src_stride_v, row_v0, width, width);
      Convert8To16Row(row_u0, row_u16, 10, width * 2);
      Convert8To16Row(row_v0, row_v16, 10, width * 2);
      if (contig_dst_uv) {
        MergeUVRow_16(row_u16, row_v16, dst_uv, 10, width * 2);
        dst_uv += (ptrdiff_t)dst_stride_uv * 2;
      } else {
        MergeUVRow_16(row_u16, row_v16, dst_uv, 10, width);
        dst_uv += dst_stride_uv;
        MergeUVRow_16(row_u16 + width, row_v16 + width, dst_uv, 10, width);
        dst_uv += dst_stride_uv;
      }
      src_u += src_stride_u;
      src_v += src_stride_v;
    }

    if (!(abs_height & 1)) {
      if (src_uv_height == 1) {
        memcpy(dst_uv, dst_uv - dst_stride_uv,
               (size_t)width * 2u * sizeof(uint16_t));
      } else {
        Scale2RowUp(src_u, 0, row_u0, 0, width);
        Scale2RowUp(src_v, 0, row_v0, 0, width);
        Convert8To16Row(row_u0, row_u16, 10, width);
        Convert8To16Row(row_v0, row_v16, 10, width);
        MergeUVRow_16(row_u16, row_v16, dst_uv, 10, width);
      }
    }
  }
  free_aligned_buffer_64(row_buf);
  return 0;
}

static int I420ToNV24(const uint8_t* src_y,
                      int src_stride_y,
                      const uint8_t* src_u,
                      int src_stride_u,
                      const uint8_t* src_v,
                      int src_stride_v,
                      uint8_t* dst_y,
                      int dst_stride_y,
                      uint8_t* dst_uv,
                      int dst_stride_uv,
                      int width,
                      int height) {
  const int abs_height = Abs(height);
  const int src_uv_width = SUBSAMPLE(width, 1, 1);
  const int src_uv_height = SUBSAMPLE(abs_height, 1, 1);
  void (*MergeUVRow)(const uint8_t* src_u, const uint8_t* src_v,
                     uint8_t* dst_uv, int width) = MergeUVRow_C;
  void (*Scale2RowUp)(const uint8_t* src_ptr, ptrdiff_t src_stride,
                      uint8_t* dst_ptr, ptrdiff_t dst_stride, int dst_width) =
      ScaleUVRowUp2_Bilinear_Any_C;
  if (!src_y || !src_u || !src_v || !dst_y || !dst_uv || width <= 0 ||
      height == 0 || height == INT_MIN || src_uv_width > 32768 ||
      src_uv_height > 32768) {
    return -1;
  }
  if (height < 0) {
    src_y = src_y + (ptrdiff_t)(abs_height - 1) * src_stride_y;
    src_u = src_u + (ptrdiff_t)(src_uv_height - 1) * src_stride_u;
    src_v = src_v + (ptrdiff_t)(src_uv_height - 1) * src_stride_v;
    src_stride_y = -src_stride_y;
    src_stride_u = -src_stride_u;
    src_stride_v = -src_stride_v;
  }

  CopyPlane(src_y, src_stride_y, dst_y, dst_stride_y, width, abs_height);

  // Match ScaleFilterReduce: 1-column chroma reduces to point sampling.
  if (src_uv_width == 1) {
    int dy = (abs_height == 1) ? 0 : FixedDiv(src_uv_height, abs_height);
    uint32_t uy = (uint32_t)(dy >> 1);
    for (int j = 0; j < abs_height; ++j) {
      int yi = (int)(uy >> 16);
      uint8_t u_val = src_u[yi * (ptrdiff_t)src_stride_u];
      uint8_t v_val = src_v[yi * (ptrdiff_t)src_stride_v];
      dst_uv[0] = u_val;
      dst_uv[1] = v_val;
      if (width == 2) {
        dst_uv[2] = u_val;
        dst_uv[3] = v_val;
      }
      dst_uv += dst_stride_uv;
      uy += (uint32_t)dy;
    }
    return 0;
  }

#if defined(HAS_MERGEUVROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    MergeUVRow = MergeUVRow_Any_SSE2;
    if (IS_ALIGNED(src_uv_width, 16)) {
      MergeUVRow = MergeUVRow_SSE2;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    MergeUVRow = MergeUVRow_Any_AVX2;
    if (IS_ALIGNED(src_uv_width, 32)) {
      MergeUVRow = MergeUVRow_AVX2;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_AVX512BW)
  if (TestCpuFlag(kCpuHasAVX512BW)) {
    MergeUVRow = MergeUVRow_AVX512BW;
  }
#endif
#if defined(HAS_MERGEUVROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    MergeUVRow = MergeUVRow_Any_NEON;
    if (IS_ALIGNED(src_uv_width, 16)) {
      MergeUVRow = MergeUVRow_NEON;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_SVE2)
  if (TestCpuFlag(kCpuHasSVE2)) {
    MergeUVRow = MergeUVRow_SVE2;
  }
#endif
#if defined(HAS_MERGEUVROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    MergeUVRow = MergeUVRow_Any_LSX;
    if (IS_ALIGNED(src_uv_width, 16)) {
      MergeUVRow = MergeUVRow_LSX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    MergeUVRow = MergeUVRow_Any_LASX;
    if (IS_ALIGNED(src_uv_width, 32)) {
      MergeUVRow = MergeUVRow_LASX;
    }
  }
#endif
#if defined(HAS_MERGEUVROW_RVV)
  if (TestCpuFlag(kCpuHasRVV)) {
    MergeUVRow = MergeUVRow_RVV;
  }
#endif
#if defined(HAS_MERGEUVROW_HVX)
  if (TestCpuFlag(kCpuHasHVX)) {
    MergeUVRow = MergeUVRow_HVX;
  }
#endif
#if defined(HAS_MERGEUVROW_WASMSIMD)
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    MergeUVRow = MergeUVRow_Any_WASMSIMD;
    if (IS_ALIGNED(src_uv_width, 16)) {
      MergeUVRow = MergeUVRow_WASMSIMD;
    }
  }
#endif
#ifdef HAS_SCALEUVROWUP2_BILINEAR_SSSE3
  if (TestCpuFlag(kCpuHasSSSE3)) {
    Scale2RowUp = ScaleUVRowUp2_Bilinear_Any_SSSE3;
  }
#endif
#ifdef HAS_SCALEUVROWUP2_BILINEAR_AVX2
  if (TestCpuFlag(kCpuHasAVX2)) {
    Scale2RowUp = ScaleUVRowUp2_Bilinear_Any_AVX2;
  }
#endif
#ifdef HAS_SCALEUVROWUP2_BILINEAR_NEON
  if (TestCpuFlag(kCpuHasNEON)) {
    Scale2RowUp = ScaleUVRowUp2_Bilinear_Any_NEON;
  }
#endif
#ifdef HAS_SCALEUVROWUP2_BILINEAR_RVV
  if (TestCpuFlag(kCpuHasRVV)) {
    Scale2RowUp = ScaleUVRowUp2_Bilinear_RVV;
  }
#endif
#ifdef HAS_SCALEUVROWUP2_BILINEAR_WASMSIMD
  if (TestCpuFlag(kCpuHasWASMSIMD)) {
    Scale2RowUp = ScaleUVRowUp2_Bilinear_Any_WASMSIMD;
  }
#endif
#ifdef HAS_SCALEUVROWUP2_BILINEAR_RELAXEDSIMD
  if (TestCpuFlag(kCpuHasRELAXEDSIMD)) {
    Scale2RowUp = ScaleUVRowUp2_Bilinear_Any_RELAXEDSIMD;
  }
#endif

  const size_t uv_row_bytes = ((size_t)src_uv_width * 2u + 63u) & ~63u;
  align_buffer_64(row_uv, uv_row_bytes * 2u);
  if (!row_uv) {
    return 1;
  }
  {
    uint8_t* row_uv0 = row_uv;
    uint8_t* row_uv1 = row_uv + uv_row_bytes;
    MergeUVRow(src_u, src_v, row_uv0, src_uv_width);
    src_u += src_stride_u;
    src_v += src_stride_v;
    Scale2RowUp(row_uv0, 0, dst_uv, 0, width);
    dst_uv += dst_stride_uv;
    for (int x = 0; x < src_uv_height - 1; ++x) {
      MergeUVRow(src_u, src_v, row_uv1, src_uv_width);
      src_u += src_stride_u;
      src_v += src_stride_v;
      Scale2RowUp(row_uv0, row_uv1 - row_uv0, dst_uv, dst_stride_uv, width);
      dst_uv += (ptrdiff_t)dst_stride_uv * 2;
      uint8_t* tmp = row_uv0;
      row_uv0 = row_uv1;
      row_uv1 = tmp;
    }
    if (!(abs_height & 1)) {
      Scale2RowUp(row_uv0, 0, dst_uv, 0, width);
    }
  }
  free_aligned_buffer_64(row_uv);
  return 0;
}

// Convert I420 to specified format
LIBYUV_API
int ConvertFromI420(const uint8_t* y,
                    int y_stride,
                    const uint8_t* u,
                    int u_stride,
                    const uint8_t* v,
                    int v_stride,
                    uint8_t* dst_sample,
                    int dst_sample_stride,
                    int width,
                    int height,
                    uint32_t fourcc) {
  uint32_t format = CanonicalFourCC(fourcc);
  int r = 0;
  if (!y || !u || !v || !dst_sample || width <= 0 || height == 0 ||
      height == INT_MIN) {
    return -1;
  }
  const int abs_height = Abs(height);
  switch (format) {
    // Single plane formats
    case FOURCC_YUY2:
      r = I420ToYUY2(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_sample_stride ? dst_sample_stride : width * 2, width,
                     height);
      break;
    case FOURCC_UYVY:
      r = I420ToUYVY(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_sample_stride ? dst_sample_stride : width * 2, width,
                     height);
      break;
    case FOURCC_RGBP:
      r = I420ToRGB565(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                       dst_sample_stride ? dst_sample_stride : width * 2, width,
                       height);
      break;
    case FOURCC_RGBO:
      r = I420ToARGB1555(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                         dst_sample_stride ? dst_sample_stride : width * 2,
                         width, height);
      break;
    case FOURCC_R444:
      r = I420ToARGB4444(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                         dst_sample_stride ? dst_sample_stride : width * 2,
                         width, height);
      break;
    case FOURCC_24BG:
      r = I420ToRGB24(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                      dst_sample_stride ? dst_sample_stride : width * 3, width,
                      height);
      break;
    case FOURCC_RAW:
      r = I420ToRAW(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                    dst_sample_stride ? dst_sample_stride : width * 3, width,
                    height);
      break;
    case FOURCC_ARGB:
      r = I420ToARGB(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_sample_stride ? dst_sample_stride : width * 4, width,
                     height);
      break;
    case FOURCC_BGRA:
      r = I420ToBGRA(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_sample_stride ? dst_sample_stride : width * 4, width,
                     height);
      break;
    case FOURCC_ABGR:
      r = I420ToABGR(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_sample_stride ? dst_sample_stride : width * 4, width,
                     height);
      break;
    case FOURCC_RGBA:
      r = I420ToRGBA(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_sample_stride ? dst_sample_stride : width * 4, width,
                     height);
      break;
    case FOURCC_AR30:
      r = I420ToAR30(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_sample_stride ? dst_sample_stride : width * 4, width,
                     height);
      break;
    case FOURCC_I400:
      r = I400Copy(y, y_stride, dst_sample,
                   dst_sample_stride ? dst_sample_stride : width, width,
                   height);
      break;
    case FOURCC_NV12: {
      int dst_y_stride = dst_sample_stride ? dst_sample_stride : width;
      int dst_uv_stride =
          dst_sample_stride ? dst_sample_stride : ((width + 1) / 2) * 2;
      uint8_t* dst_uv = dst_sample + (ptrdiff_t)dst_y_stride * abs_height;
      r = I420ToNV12(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_y_stride, dst_uv, dst_uv_stride, width, height);
      break;
    }
    case FOURCC_NV21: {
      int dst_y_stride = dst_sample_stride ? dst_sample_stride : width;
      int dst_vu_stride =
          dst_sample_stride ? dst_sample_stride : ((width + 1) / 2) * 2;
      uint8_t* dst_vu = dst_sample + (ptrdiff_t)dst_y_stride * abs_height;
      r = I420ToNV21(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_y_stride, dst_vu, dst_vu_stride, width, height);
      break;
    }
    case FOURCC_NV16: {
      int dst_y_stride = dst_sample_stride ? dst_sample_stride : width;
      int dst_uv_stride =
          dst_sample_stride ? dst_sample_stride : ((width + 1) / 2) * 2;
      uint8_t* dst_uv = dst_sample + (ptrdiff_t)dst_y_stride * abs_height;
      int halfwidth = (width + 1) / 2;
      int halfheight = SUBSAMPLE(height, 1, 1);
      CopyPlane(y, y_stride, dst_sample, dst_y_stride, width, height);
      MergeUVPlane(u, u_stride, v, v_stride, dst_uv, dst_uv_stride * 2,
                   halfwidth, halfheight);
      if (abs_height > 1) {
        MergeUVPlane(u, u_stride, v, v_stride, dst_uv + dst_uv_stride,
                     dst_uv_stride * 2, halfwidth, height / 2);
      }
      r = 0;
      break;
    }
    case FOURCC_NV24: {
      int dst_y_stride = dst_sample_stride ? dst_sample_stride : width;
      int dst_uv_stride = dst_sample_stride ? dst_sample_stride * 2 : width * 2;
      uint8_t* dst_uv = dst_sample + (ptrdiff_t)dst_y_stride * abs_height;
      r = I420ToNV24(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_y_stride, dst_uv, dst_uv_stride, width, height);
      break;
    }
    case FOURCC_P010:
    case FOURCC_P210: {
      const int uv_min = ((width + 1) / 2) * 2;
      int dst_stride_y;
      int dst_stride_uv;
      uint16_t* dst_y16;
      uint16_t* dst_uv16;
      if (dst_sample_stride == 0) {
        dst_stride_y = width;
        dst_stride_uv = uv_min;
      } else {
        if (dst_sample_stride & 1) {
          return -1;
        }
        dst_stride_y = dst_sample_stride / 2;
        dst_stride_uv = dst_sample_stride / 2;
        if (Abs(dst_stride_uv) < uv_min) {
          return -1;
        }
      }
      dst_y16 = (uint16_t*)dst_sample;
      dst_uv16 = dst_y16 + (ptrdiff_t)dst_stride_y * abs_height;
      if (format == FOURCC_P010) {
        r = I420ToP010(y, y_stride, u, u_stride, v, v_stride, dst_y16,
                       dst_stride_y, dst_uv16, dst_stride_uv, width, height);
      } else {
        r = I420ToP210(y, y_stride, u, u_stride, v, v_stride, dst_y16,
                       dst_stride_y, dst_uv16, dst_stride_uv, width, height);
      }
      break;
    }
    case FOURCC_P410: {
      const int uv_min = width * 2;
      int dst_stride_y;
      int dst_stride_uv;
      uint16_t* dst_y16;
      uint16_t* dst_uv16;
      if (dst_sample_stride == 0) {
        dst_stride_y = width;
        dst_stride_uv = uv_min;
      } else {
        if (dst_sample_stride & 1) {
          return -1;
        }
        dst_stride_y = dst_sample_stride / 2;
        dst_stride_uv = dst_sample_stride;
        if (Abs(dst_stride_uv) < uv_min) {
          return -1;
        }
      }
      dst_y16 = (uint16_t*)dst_sample;
      dst_uv16 = dst_y16 + (ptrdiff_t)dst_stride_y * abs_height;
      r = I420ToP410(y, y_stride, u, u_stride, v, v_stride, dst_y16,
                     dst_stride_y, dst_uv16, dst_stride_uv, width, height);
      break;
    }
    // Triplanar formats
    case FOURCC_I420:
    case FOURCC_YV12: {
      dst_sample_stride = dst_sample_stride ? dst_sample_stride : width;
      int halfstride = (dst_sample_stride + 1) / 2;
      int halfheight = (abs_height + 1) / 2;
      uint8_t* dst_u;
      uint8_t* dst_v;
      if (format == FOURCC_YV12) {
        dst_v = dst_sample + (ptrdiff_t)dst_sample_stride * abs_height;
        dst_u = dst_v + (ptrdiff_t)halfstride * halfheight;
      } else {
        dst_u = dst_sample + (ptrdiff_t)dst_sample_stride * abs_height;
        dst_v = dst_u + (ptrdiff_t)halfstride * halfheight;
      }
      r = I420Copy(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                   dst_sample_stride, dst_u, halfstride, dst_v, halfstride,
                   width, height);
      break;
    }
    case FOURCC_I422:
    case FOURCC_YV16: {
      dst_sample_stride = dst_sample_stride ? dst_sample_stride : width;
      int halfstride = (dst_sample_stride + 1) / 2;
      uint8_t* dst_u;
      uint8_t* dst_v;
      if (format == FOURCC_YV16) {
        dst_v = dst_sample + (ptrdiff_t)dst_sample_stride * abs_height;
        dst_u = dst_v + (ptrdiff_t)halfstride * abs_height;
      } else {
        dst_u = dst_sample + (ptrdiff_t)dst_sample_stride * abs_height;
        dst_v = dst_u + (ptrdiff_t)halfstride * abs_height;
      }
      r = I420ToI422(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_sample_stride, dst_u, halfstride, dst_v, halfstride,
                     width, height);
      break;
    }
    case FOURCC_I444:
    case FOURCC_YV24: {
      dst_sample_stride = dst_sample_stride ? dst_sample_stride : width;
      uint8_t* dst_u;
      uint8_t* dst_v;
      if (format == FOURCC_YV24) {
        dst_v = dst_sample + (ptrdiff_t)dst_sample_stride * abs_height;
        dst_u = dst_v + (ptrdiff_t)dst_sample_stride * abs_height;
      } else {
        dst_u = dst_sample + (ptrdiff_t)dst_sample_stride * abs_height;
        dst_v = dst_u + (ptrdiff_t)dst_sample_stride * abs_height;
      }
      r = I420ToI444(y, y_stride, u, u_stride, v, v_stride, dst_sample,
                     dst_sample_stride, dst_u, dst_sample_stride, dst_v,
                     dst_sample_stride, width, height);
      break;
    }
    // Formats not supported - MJPG, biplanar, some rgb formats.
    default:
      return -1;  // unknown fourcc - return failure code.
  }
  return r;
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
