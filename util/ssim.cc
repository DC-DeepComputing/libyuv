/*
 *  Copyright 2013 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "../util/ssim.h"  // NOLINT

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int uint32_t;    // NOLINT
typedef unsigned short uint16_t;  // NOLINT

    (defined(_M_X64) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 2)))
#endif
#include <emmintrin.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

// SSIM
enum { KERNEL = 3, KERNEL_SIZE = 2 * KERNEL + 1 };

// Symmetric Gaussian kernel:  K[i] = ~11 * exp(-0.3 * i * i)
// The maximum value (11 x 11) must be less than 128 to avoid sign
// problems during the calls to _mm_mullo_epi16().
static const int K[KERNEL_SIZE] = {
    1, 3, 7, 11, 7, 3, 1  // ~11 * exp(-0.3 * i * i)
};
static const double kiW[KERNEL + 1 + 1] = {
    1. / 1089.,  // 1 / sum(i:0..6, j..6) K[i]*K[j]
    1. / 1089.,  // 1 / sum(i:0..6, j..6) K[i]*K[j]
    1. / 1056.,  // 1 / sum(i:0..5, j..6) K[i]*K[j]
    1. / 957.,   // 1 / sum(i:0..4, j..6) K[i]*K[j]
    1. / 726.,   // 1 / sum(i:0..3, j..6) K[i]*K[j]
};

// Note: worst case of accumulation is a weight of 33 = 11 + 2 * (7 + 3 + 1)
// with a diff of 255, squared. The maximum error is thus 0x4388241,
// which fits into 32 bits integers.
double GetSSIM(const uint8_t* org,
               const uint8_t* rec,
               int xo,
               int yo,
               int W,
               int H,
               int stride) {
  uint32_t ws = 0, xm = 0, ym = 0, xxm = 0, xym = 0, yym = 0;
  org += (yo - KERNEL) * stride;
  org += (xo - KERNEL);
  rec += (yo - KERNEL) * stride;
  rec += (xo - KERNEL);
  for (int y_ = 0; y_ < KERNEL_SIZE; ++y_, org += stride, rec += stride) {
    if (((yo - KERNEL + y_) < 0) || ((yo - KERNEL + y_) >= H)) {
      continue;
    }
    const int Wy = K[y_];
    for (int x_ = 0; x_ < KERNEL_SIZE; ++x_) {
      const int Wxy = Wy * K[x_];
      if (((xo - KERNEL + x_) >= 0) && ((xo - KERNEL + x_) < W)) {
        const int org_x = org[x_];
        const int rec_x = rec[x_];
        ws += Wxy;
        xm += Wxy * org_x;
        ym += Wxy * rec_x;
        xxm += Wxy * org_x * org_x;
        xym += Wxy * org_x * rec_x;
        yym += Wxy * rec_x * rec_x;
      }
    }
  }
  return FinalizeSSIM(1. / ws, xm, ym, xxm, xym, yym);
}

double GetSSIMFullKernel(const uint8_t* org,
                         const uint8_t* rec,
                         int xo,
                         int yo,
                         int stride,
                         double area_weight) {
  uint32_t xm = 0, ym = 0, xxm = 0, xym = 0, yym = 0;


  org += yo * stride + xo;
  rec += yo * stride + xo;
  for (int y = 1; y <= KERNEL; y++) {
    const int dy1 = y * stride;
    const int dy2 = y * stride;
    const int Wy = K[KERNEL + y];

    for (int x = 1; x <= KERNEL; x++) {
      // Compute the contributions of upper-left (ul), upper-right (ur)
      // lower-left (ll) and lower-right (lr) points (see the diagram below).
      // Symmetric Kernel will have same weight on those points.
      //       -  -  -  -  -  -  -
      //       -  ul -  -  -  ur -
      //       -  -  -  -  -  -  -
      //       -  -  -  0  -  -  -
      //       -  -  -  -  -  -  -
      //       -  ll -  -  -  lr -
      //       -  -  -  -  -  -  -
      const int Wxy = Wy * K[KERNEL + x];
      const int ul1 = org[-dy1 - x];
      const int ur1 = org[-dy1 + x];
      const int ll1 = org[dy1 - x];
      const int lr1 = org[dy1 + x];

      const int ul2 = rec[-dy2 - x];
      const int ur2 = rec[-dy2 + x];
      const int ll2 = rec[dy2 - x];
      const int lr2 = rec[dy2 + x];

      xm += Wxy * (ul1 + ur1 + ll1 + lr1);
      ym += Wxy * (ul2 + ur2 + ll2 + lr2);
      xxm += Wxy * (ul1 * ul1 + ur1 * ur1 + ll1 * ll1 + lr1 * lr1);
      xym += Wxy * (ul1 * ul2 + ur1 * ur2 + ll1 * ll2 + lr1 * lr2);
      yym += Wxy * (ul2 * ul2 + ur2 * ur2 + ll2 * ll2 + lr2 * lr2);
    }

    // Compute the contributions of up (u), down (d), left (l) and right (r)
    // points across the main axes (see the diagram below).
    // Symmetric Kernel will have same weight on those points.
    //       -  -  -  -  -  -  -
    //       -  -  -  u  -  -  -
    //       -  -  -  -  -  -  -
    //       -  l  -  0  -  r  -
    //       -  -  -  -  -  -  -
    //       -  -  -  d  -  -  -
    //       -  -  -  -  -  -  -
    const int Wxy = Wy * K[KERNEL];
    const int u1 = org[-dy1];
    const int d1 = org[dy1];
    const int l1 = org[-y];
    const int r1 = org[y];

    const int u2 = rec[-dy2];
    const int d2 = rec[dy2];
    const int l2 = rec[-y];
    const int r2 = rec[y];

    xm += Wxy * (u1 + d1 + l1 + r1);
    ym += Wxy * (u2 + d2 + l2 + r2);
    xxm += Wxy * (u1 * u1 + d1 * d1 + l1 * l1 + r1 * r1);
    xym += Wxy * (u1 * u2 + d1 * d2 + l1 * l2 + r1 * r2);
    yym += Wxy * (u2 * u2 + d2 * d2 + l2 * l2 + r2 * r2);
  }

  // Lastly the contribution of (x0, y0) point.
  const int Wxy = K[KERNEL] * K[KERNEL];
  const int s1 = org[0];
  const int s2 = rec[0];

  xm += Wxy * s1;
  ym += Wxy * s2;
  xxm += Wxy * s1 * s1;
  xym += Wxy * s1 * s2;
  yym += Wxy * s2 * s2;


  return FinalizeSSIM(area_weight, xm, ym, xxm, xym, yym);
}

static int start_max(int x, int y) {
  return (x > y) ? x : y;
}

double CalcSSIM(const uint8_t* org,
                const uint8_t* rec,
                const int image_width,
                const int image_height) {
  double SSIM = 0.;
  const int KERNEL_Y = (image_height < KERNEL) ? image_height : KERNEL;
  const int KERNEL_X = (image_width < KERNEL) ? image_width : KERNEL;
  const int start_x = start_max(image_width - 8 + KERNEL_X, KERNEL_X);
  const int start_y = start_max(image_height - KERNEL_Y, KERNEL_Y);
  const int stride = image_width;

  for (int j = 0; j < KERNEL_Y; ++j) {
    for (int i = 0; i < image_width; ++i) {
      SSIM += GetSSIM(org, rec, i, j, image_width, image_height, stride);
    }
  }

#ifdef _OPENMP
#pragma omp parallel for reduction(+ : SSIM)
#endif
  for (int j = KERNEL_Y; j < image_height - KERNEL_Y; ++j) {
    for (int i = 0; i < KERNEL_X; ++i) {
      SSIM += GetSSIM(org, rec, i, j, image_width, image_height, stride);
    }
    for (int i = KERNEL_X; i < start_x; ++i) {
      SSIM += GetSSIMFullKernel(org, rec, i, j, stride, kiW[0]);
    }
    if (start_x < image_width) {
      // GetSSIMFullKernel() needs to be able to read 8 pixels (in SSE2). So we
      // copy the 8 rightmost pixels on a cache area, and pad this area with
      // zeros which won't contribute to the overall SSIM value (but we need
      // to pass the correct normalizing constant!). By using this cache, we can
      // still call GetSSIMFullKernel() instead of the slower GetSSIM().
      // NOTE: we could use similar method for the left-most pixels too.
      const int kScratchWidth = 8;
      const int kScratchStride = kScratchWidth + KERNEL + 1;
      uint8_t scratch_org[KERNEL_SIZE * kScratchStride] = {0};
      uint8_t scratch_rec[KERNEL_SIZE * kScratchStride] = {0};

      for (int k = 0; k < KERNEL_SIZE; ++k) {
        const int offset =
            (j - KERNEL + k) * stride + image_width - kScratchWidth;
        memcpy(scratch_org + k * kScratchStride, org + offset, kScratchWidth);
        memcpy(scratch_rec + k * kScratchStride, rec + offset, kScratchWidth);
      }
      for (int k = 0; k <= KERNEL_X + 1; ++k) {
        SSIM += GetSSIMFullKernel(scratch_org, scratch_rec, KERNEL + k, KERNEL,
                                  kScratchStride, kiW[k]);
      }
    }
  }

  for (int j = start_y; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      SSIM += GetSSIM(org, rec, i, j, image_width, image_height, stride);
    }
  }
  return SSIM;
}

double CalcLSSIM(double ssim) {
  return -10.0 * log10(1.0 - ssim);
}

#ifdef __cplusplus
}  // extern "C"
#endif
