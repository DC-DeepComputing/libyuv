/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stdlib.h>
#include <time.h>

#include "../unit_test/unit_test.h"
#include "libyuv/cpu_id.h"
#include "libyuv/scale_uv.h"

namespace libyuv {

#define STRINGIZE(line) #line
#define FILELINESTR(file, line) file ":" STRINGIZE(line)

#if !defined(DISABLE_SLOW_TESTS) || defined(__x86_64__) || defined(__i386__)
// SLOW TESTS are those that are unoptimized C code.
// FULL TESTS are optimized but test many variations of the same code.
#define ENABLE_FULL_TESTS
#endif

// Test scaling with C vs Opt and return maximum pixel difference. 0 = exact.
static int UVTestFilter(int src_width,
                        int src_height,
                        int dst_width,
                        int dst_height,
                        FilterMode f,
                        int benchmark_iterations,
                        int disable_cpu_flags,
                        int benchmark_cpu_info,
                        int src_offset = 0,
                        int dst_offset = 0,
                        int src_stride_offset = 0,
                        int dst_stride_offset = 0) {
  if (!SizeValid(src_width, src_height, dst_width, dst_height)) {
    return 0;
  }

  const int src_row_bytes = Abs(src_width) * 2;
  const int src_stride_uv = src_row_bytes + src_stride_offset;
  const int64_t src_uv_plane_size =
      (int64_t)src_stride_uv * (Abs(src_height) - 1) + src_row_bytes;
  const int dst_row_bytes = dst_width * 2;
  const int dst_stride_uv = dst_row_bytes + dst_stride_offset;
  const int64_t dst_uv_plane_size =
      (int64_t)dst_stride_uv * (dst_height - 1) + dst_row_bytes;

  align_buffer_page_end(src_uv_c, src_uv_plane_size);
  align_buffer_page_end(src_uv_opt, src_uv_plane_size + src_offset);
  align_buffer_page_end(dst_uv_c, dst_uv_plane_size + dst_offset);
  align_buffer_page_end(dst_uv_opt, dst_uv_plane_size + dst_offset);

  if (!src_uv_c || !src_uv_opt || !dst_uv_c || !dst_uv_opt) {
    printf("Skipped.  Alloc failed " FILELINESTR(__FILE__, __LINE__) "\n");
    return 0;
  }
  MemRandomize(src_uv_c, src_uv_plane_size);
  for (int y = 0; y < Abs(src_height); ++y) {
    memcpy(src_uv_opt + src_offset + y * (int64_t)src_stride_uv,
           src_uv_c + y * (int64_t)src_row_bytes, src_row_bytes);
  }
  memset(dst_uv_c, 2, dst_uv_plane_size + dst_offset);
  memset(dst_uv_opt, 123, dst_uv_plane_size + dst_offset);

  MaskCpuFlags(disable_cpu_flags);  // Disable all CPU optimization.
  double c_time = get_time();
  UVScale(src_uv_c, src_row_bytes, src_width, src_height, dst_uv_c,
          dst_row_bytes, dst_width, dst_height, f);
  c_time = (get_time() - c_time);

  MaskCpuFlags(benchmark_cpu_info);  // Enable all CPU optimization.
  double opt_time = get_time();
  for (int i = 0; i < benchmark_iterations; ++i) {
    UVScale(src_uv_opt + src_offset, src_stride_uv, src_width, src_height,
            dst_uv_opt + dst_offset, dst_stride_uv, dst_width, dst_height, f);
  }
  opt_time = (get_time() - opt_time) / benchmark_iterations;

  // Report performance of C vs OPT
  printf("filter %d - %8d us C - %8d us OPT\n", f,
         static_cast<int>(c_time * 1e6), static_cast<int>(opt_time * 1e6));

  int max_diff = 0;
  for (int y = 0; y < dst_height; ++y) {
    for (int x = 0; x < dst_row_bytes; ++x) {
      int abs_diff =
          Abs(dst_uv_c[y * (int64_t)dst_row_bytes + x] -
              dst_uv_opt[dst_offset + y * (int64_t)dst_stride_uv + x]);
      if (abs_diff > max_diff) {
        max_diff = abs_diff;
      }
    }
  }

  free_aligned_buffer_page_end(dst_uv_c);
  free_aligned_buffer_page_end(dst_uv_opt);
  free_aligned_buffer_page_end(src_uv_c);
  free_aligned_buffer_page_end(src_uv_opt);
  return max_diff;
}

TEST_F(LibYUVScaleTest, UVScale_Unaligned) {
  struct TestCase {
    const char* name;
    int src_width;
    int src_height;
    int dst_width;
    int dst_height;
    FilterMode filtering;
  };
  constexpr TestCase kTestCases[] = {
      {"point", 32, 32, 63, 63, kFilterNone},
      {"point_up2", 32, 32, 64, 64, kFilterNone},
      {"point_down_even", 192, 64, 32, 16, kFilterNone},
      {"point_down_even_odd", 198, 64, 33, 16, kFilterNone},
      {"point_down4", 128, 64, 32, 16, kFilterNone},
      {"filter", 63, 47, 37, 29, kFilterLinear},
      {"point_64", 32768, 2, 32767, 2, kFilterNone},
      {"filter_64", 32768, 2, 32767, 2, kFilterLinear},
      {"point_down2", 64, 64, 32, 32, kFilterNone},
      {"linear_down2", 64, 64, 32, 32, kFilterLinear},
      {"linear_up2", 32, 32, 64, 64, kFilterLinear},
      {"bilinear_up2", 32, 32, 64, 64, kFilterBilinear},
      {"box_down2", 64, 64, 32, 32, kFilterBox},
  };

  struct OffsetCase {
    const char* name;
    int src_offset;
    int dst_offset;
  };
  constexpr OffsetCase kOffsetCases[] = {
      {"unaligned_source", 1, 0},
      {"two_byte_aligned_source", 2, 0},
      {"unaligned_destination", 0, 1},
      {"unaligned_source_and_destination", 1, 1},
  };

  for (const auto& offset_case : kOffsetCases) {
    for (const auto& test_case : kTestCases) {
      SCOPED_TRACE(offset_case.name);
      SCOPED_TRACE(test_case.name);
      // Run unaligned pointers through both C and optimized dispatch.
      for (int cpu_info : {disable_cpu_flags_, benchmark_cpu_info_}) {
        SCOPED_TRACE(cpu_info);
        int diff = UVTestFilter(
            test_case.src_width, test_case.src_height, test_case.dst_width,
            test_case.dst_height, test_case.filtering, 1, disable_cpu_flags_,
            cpu_info, offset_case.src_offset, offset_case.dst_offset);
        ASSERT_EQ(0, diff);
      }
    }
  }

  struct StrideCase {
    const char* name;
    int src_width;
    int src_height;
    int dst_width;
    int dst_height;
    FilterMode filtering;
    int src_stride_offset;
    int dst_stride_offset;
  };
  constexpr StrideCase kStrideCases[] = {
      {"point_down2_source_stride", 64, 64, 32, 32, kFilterNone, 1, 0},
      {"point_down2_destination_stride", 64, 64, 32, 32, kFilterNone, 0, 1},
      {"point_down_even_destination_stride", 192, 64, 32, 16, kFilterNone, 0,
       1},
      {"linear_up2_destination_stride", 32, 32, 64, 64, kFilterLinear, 0, 1},
      {"bilinear_up2_destination_stride", 32, 32, 64, 64, kFilterBilinear, 0,
       1},
  };

  for (const auto& stride_case : kStrideCases) {
    SCOPED_TRACE(stride_case.name);
    // Run odd strides through both C and optimized dispatch.
    for (int cpu_info : {disable_cpu_flags_, benchmark_cpu_info_}) {
      SCOPED_TRACE(cpu_info);
      int diff = UVTestFilter(stride_case.src_width, stride_case.src_height,
                              stride_case.dst_width, stride_case.dst_height,
                              stride_case.filtering, 1, disable_cpu_flags_,
                              cpu_info, 0, 0, stride_case.src_stride_offset,
                              stride_case.dst_stride_offset);
      ASSERT_EQ(0, diff);
    }
  }
}

// The following adjustments in dimensions ensure the scale factor will be
// exactly achieved.
#define DX(x, nom, denom) static_cast<int>((Abs(x) / nom) * nom)
#define SX(x, nom, denom) static_cast<int>((x / nom) * denom)

#define TEST_FACTOR1(name, filter, nom, denom)                               \
  TEST_F(LibYUVScaleTest, UVScaleDownBy##name##_##filter) {                  \
    int diff = UVTestFilter(                                                 \
        SX(benchmark_width_, nom, denom), SX(benchmark_height_, nom, denom), \
        DX(benchmark_width_, nom, denom), DX(benchmark_height_, nom, denom), \
        kFilter##filter, benchmark_iterations_, disable_cpu_flags_,          \
        benchmark_cpu_info_);                                                \
    ASSERT_EQ(0, diff);                                                      \
  }

#if defined(ENABLE_FULL_TESTS)
// Test a scale factor with all 4 filters.  Expect exact for SIMD vs C.
#define TEST_FACTOR(name, nom, denom)      \
  TEST_FACTOR1(name, None, nom, denom)     \
  TEST_FACTOR1(name, Linear, nom, denom)   \
  TEST_FACTOR1(name, Bilinear, nom, denom) \
  TEST_FACTOR1(name, Box, nom, denom)
#else
// Test a scale factor with Bilinear.
#define TEST_FACTOR(name, nom, denom) TEST_FACTOR1(name, Bilinear, nom, denom)
#endif

TEST_FACTOR(2, 1, 2)
TEST_FACTOR(4, 1, 4)
// TEST_FACTOR(8, 1, 8)  Disable for benchmark performance.
TEST_FACTOR(3by4, 3, 4)
TEST_FACTOR(3by8, 3, 8)
TEST_FACTOR(3, 1, 3)
#undef TEST_FACTOR1
#undef TEST_FACTOR
#undef SX
#undef DX

#define TEST_SCALETO1(name, width, height, filter, max_diff)                \
  TEST_F(LibYUVScaleTest, name##To##width##x##height##_##filter) {          \
    int diff = UVTestFilter(benchmark_width_, benchmark_height_, width,     \
                            height, kFilter##filter, benchmark_iterations_, \
                            disable_cpu_flags_, benchmark_cpu_info_);       \
    ASSERT_LE(diff, max_diff);                                              \
  }                                                                         \
  TEST_F(LibYUVScaleTest, name##From##width##x##height##_##filter) {        \
    int diff = UVTestFilter(width, height, Abs(benchmark_width_),           \
                            Abs(benchmark_height_), kFilter##filter,        \
                            benchmark_iterations_, disable_cpu_flags_,      \
                            benchmark_cpu_info_);                           \
    ASSERT_LE(diff, max_diff);                                              \
  }

#if defined(ENABLE_FULL_TESTS)
/// Test scale to a specified size with all 4 filters.
#define TEST_SCALETO(name, width, height)       \
  TEST_SCALETO1(name, width, height, None, 0)   \
  TEST_SCALETO1(name, width, height, Linear, 3) \
  TEST_SCALETO1(name, width, height, Bilinear, 3)
#else
#define TEST_SCALETO(name, width, height) \
  TEST_SCALETO1(name, width, height, Bilinear, 3)
#endif

TEST_SCALETO(UVScale, 1, 1)
TEST_SCALETO(UVScale, 569, 480)
TEST_SCALETO(UVScale, 640, 360)
#ifndef DISABLE_SLOW_TESTS
TEST_SCALETO(UVScale, 256, 144) /* 128x72 * 2 */
TEST_SCALETO(UVScale, 320, 240)
TEST_SCALETO(UVScale, 1280, 720)
TEST_SCALETO(UVScale, 1920, 1080)
#endif  // DISABLE_SLOW_TESTS
#undef TEST_SCALETO1
#undef TEST_SCALETO

#define TEST_SCALESWAPXY1(name, filter, max_diff)                              \
  TEST_F(LibYUVScaleTest, name##SwapXY_##filter) {                             \
    int diff =                                                                 \
        UVTestFilter(benchmark_width_, benchmark_height_, benchmark_height_,   \
                     benchmark_width_, kFilter##filter, benchmark_iterations_, \
                     disable_cpu_flags_, benchmark_cpu_info_);                 \
    ASSERT_LE(diff, max_diff);                                                 \
  }

#if defined(ENABLE_FULL_TESTS)
// Test scale with swapped width and height with all 3 filters.
TEST_SCALESWAPXY1(UVScale, None, 0)
TEST_SCALESWAPXY1(UVScale, Linear, 0)
TEST_SCALESWAPXY1(UVScale, Bilinear, 0)
#else
TEST_SCALESWAPXY1(UVScale, Bilinear, 0)
#endif
#undef TEST_SCALESWAPXY1

TEST_F(LibYUVScaleTest, UVTest3x) {
  const int kSrcStride = 480 * 2;
  const int kDstStride = 160 * 2;
  const int kSize = kSrcStride * 3;
  align_buffer_page_end(orig_pixels, kSize);
  for (int i = 0; i < 480 * 3; ++i) {
    orig_pixels[i * 2 + 0] = i;
    orig_pixels[i * 2 + 1] = 255 - i;
  }
  align_buffer_page_end(dest_pixels, kDstStride);

  int iterations160 = (benchmark_width_ * benchmark_height_ + (160 - 1)) / 160 *
                      benchmark_iterations_;
  for (int i = 0; i < iterations160; ++i) {
    UVScale(orig_pixels, kSrcStride, 480, 3, dest_pixels, kDstStride, 160, 1,
            kFilterBilinear);
  }

  ASSERT_EQ(225, dest_pixels[0]);
  ASSERT_EQ(255 - 225, dest_pixels[1]);

  UVScale(orig_pixels, kSrcStride, 480, 3, dest_pixels, kDstStride, 160, 1,
          kFilterNone);

  ASSERT_EQ(225, dest_pixels[0]);
  ASSERT_EQ(255 - 225, dest_pixels[1]);

  free_aligned_buffer_page_end(dest_pixels);
  free_aligned_buffer_page_end(orig_pixels);
}

TEST_F(LibYUVScaleTest, UVTest4x) {
  const int kSrcStride = 640 * 2;
  const int kDstStride = 160 * 2;
  const int kSize = kSrcStride * 4;
  align_buffer_page_end(orig_pixels, kSize);
  for (int i = 0; i < 640 * 4; ++i) {
    orig_pixels[i * 2 + 0] = i;
    orig_pixels[i * 2 + 1] = 255 - i;
  }
  align_buffer_page_end(dest_pixels, kDstStride);

  int iterations160 = (benchmark_width_ * benchmark_height_ + (160 - 1)) / 160 *
                      benchmark_iterations_;
  for (int i = 0; i < iterations160; ++i) {
    UVScale(orig_pixels, kSrcStride, 640, 4, dest_pixels, kDstStride, 160, 1,
            kFilterBilinear);
  }

  ASSERT_EQ(66, dest_pixels[0]);
  ASSERT_EQ(190, dest_pixels[1]);

  UVScale(orig_pixels, kSrcStride, 64, 4, dest_pixels, kDstStride, 16, 1,
          kFilterNone);

  ASSERT_EQ(2, dest_pixels[0]);  // expect the 3rd pixel of the 3rd row
  ASSERT_EQ(255 - 2, dest_pixels[1]);

  free_aligned_buffer_page_end(dest_pixels);
  free_aligned_buffer_page_end(orig_pixels);
}

}  // namespace libyuv
