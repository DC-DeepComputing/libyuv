---
name: libyuv/source/document-simd-kernel-block-dimensions
description: When commenting SIMD row scaling, conversion, or filtering kernels, accurately document the input pixel dimensions read and output dimensions written per iteration
rubric: readability
severity: should_fix
---

**Rule.** When adding or updating comments preceding SIMD row scaling,
conversion, or filtering kernels, accurately describe the exact input block
dimensions read and output dimensions written per loop iteration (including any
extra boundary pixels read for filtering). Do not leave stale or inaccurate
block dimensions copied from adjacent kernels.

**Why.** Inaccurate comments describing kernel block sizes mislead readers and
complicate maintenance, debugging, and porting of SIMD assembly and intrinsic
kernels across architectures.

## Examples

Bad:

```cpp
// Read 5x2 upsample with filtering and write 8x1.
void ScaleRowUp2_16_NEON(const uint16_t* src_ptr,
                         ptrdiff_t src_stride,
                         uint16_t* dst,
                         int dst_width);
```

Good:

```cpp
// Read 8x2 upsample with filtering and write 16x1.
// Actually reads an extra pixel, so 9x2.
void ScaleRowUp2_16_NEON(const uint16_t* src_ptr,
                         ptrdiff_t src_stride,
                         uint16_t* dst,
                         int dst_width);
```
