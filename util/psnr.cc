/*
 *  Copyright 2013 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "./psnr.h"  // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

// PSNR formula: psnr = 10 * log10 (Peak Signal^2 * size / sse)
// Returns 128.0 (kMaxPSNR) if sse is 0 (perfect match).
double ComputePSNR(double sse, double size) {
  const double kMINSSE = 255.0 * 255.0 * size / pow(10.0, kMaxPSNR / 10.0);
  if (sse <= kMINSSE) {
    sse = kMINSSE;  // Produces max PSNR of 128
  }
  return 10.0 * log10(255.0 * 255.0 * size / sse);
}

#ifdef __cplusplus
}  // extern "C"
#endif
