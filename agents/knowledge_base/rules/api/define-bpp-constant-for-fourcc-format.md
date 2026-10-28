---
name: libyuv/api/define-bpp-constant-for-fourcc-format
description: When adding new video format FourCC identifiers to enum FourCC, define corresponding FOURCC_BPP_* constants in enum FourCCBpp and add test coverage
rubric: correctness
severity: should_fix
---

**Rule.** When adding new video format FourCC identifiers to enum FourCC in
include/libyuv/video_common.h, always define their corresponding bits-per-pixel
constants (FOURCC_BPP_*) in enum FourCCBpp and add test coverage in
unit_test/video_common_test.cc.

**Why.** Each video format defined in enum FourCC requires an associated
bits-per-pixel value in enum FourCCBpp for buffer sizing calculations and format
verification in tests.

## Examples

Bad:

```cpp
// include/libyuv/video_common.h
enum FourCC {
  FOURCC_P010 = FOURCC('P', '0', '1', '0'),
  FOURCC_P210 = FOURCC('P', '2', '1', '0'),
};
// enum FourCCBpp missing corresponding FOURCC_BPP_P010 / FOURCC_BPP_P210 entries
```

Good:

```cpp
// include/libyuv/video_common.h
enum FourCCBpp {
  FOURCC_BPP_P010 = 15,
  FOURCC_BPP_P210 = 20,
};

// unit_test/video_common_test.cc
ASSERT_TRUE(TestValidFourCC(FOURCC_P010, FOURCC_BPP_P010));
ASSERT_TRUE(TestValidFourCC(FOURCC_P210, FOURCC_BPP_P210));
```
