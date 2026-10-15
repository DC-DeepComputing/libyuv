// Copyright 2026 The LibYuv Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source tree.

#include <stddef.h>
#include <stdint.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <vector>
#include "libyuv.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size < 32) {
    return 0;
  }
  FuzzedDataProvider provider(data, size);

  int src_w = provider.ConsumeIntegralInRange<int>(1, 64);
  int src_h = provider.ConsumeIntegralInRange<int>(1, 64);
  int dst_w = provider.ConsumeIntegralInRange<int>(1, 64);
  int dst_h = provider.ConsumeIntegralInRange<int>(1, 64);
  int filter = provider.ConsumeIntegralInRange<int>(0, 3);
  int mode = provider.ConsumeIntegralInRange<int>(0, 3);

  size_t src_argb_size = static_cast<size_t>(src_w) * src_h * 4;
  size_t dst_argb_size = static_cast<size_t>(dst_w) * dst_h * 4;

  std::vector<uint8_t> src_argb = provider.ConsumeBytes<uint8_t>(src_argb_size);
  src_argb.resize(src_argb_size, 0);
  std::vector<uint8_t> dst_argb(dst_argb_size, 0);

  int half_src_w = (src_w + 1) / 2;
  int half_src_h = (src_h + 1) / 2;
  std::vector<uint8_t> y_plane(src_w * src_h, 0);
  std::vector<uint8_t> u_plane(half_src_w * half_src_h, 0);
  std::vector<uint8_t> v_plane(half_src_w * half_src_h, 0);

  if (mode == 0) {
    libyuv::ARGBScale(src_argb.data(), src_w * 4, src_w, src_h,
                      dst_argb.data(), dst_w * 4, dst_w, dst_h,
                      static_cast<libyuv::FilterMode>(filter));
  } else if (mode == 1) {
    libyuv::ARGBToI420(src_argb.data(), src_w * 4, y_plane.data(), src_w,
                       u_plane.data(), half_src_w, v_plane.data(), half_src_w,
                       src_w, src_h);
    libyuv::I420ToARGB(y_plane.data(), src_w, u_plane.data(), half_src_w,
                       v_plane.data(), half_src_w, src_argb.data(), src_w * 4,
                       src_w, src_h);
  } else if (mode == 2) {
    int8_t matrix[16];
    for (int i = 0; i < 16; ++i) {
      matrix[i] = provider.ConsumeIntegral<int8_t>();
    }
    libyuv::ARGBColorMatrix(src_argb.data(), src_w * 4, src_argb.data(),
                            src_w * 4, matrix, src_w, src_h);
  } else {
    std::vector<uint8_t> rot_argb(src_argb_size, 0);
    libyuv::ARGBRotate(src_argb.data(), src_w * 4, rot_argb.data(), src_h * 4,
                       src_w, src_h, libyuv::kRotate90);
  }

  return 0;
}
