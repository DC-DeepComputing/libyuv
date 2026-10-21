---
name: libyuv/unit_test/guard-large-test-allocations
rubric: correctness
severity: should_fix
---

**Rule.** When writing unit tests that allocate multi-gigabyte memory buffers,
guard against process crashes on low-memory or 32-bit test bots. On ARM64, gate
large allocations by testing for modern CPU feature flags (such as
TestCpuFlag(kCpuHasNeonDotProd)) and allocate using new (std::nothrow). If
modern CPU support is absent or allocation returns nullptr, skip the test with
GTEST_SKIP() rather than failing via ASSERT_NE.

**Why.** On 32-bit and memory-constrained mobile test environments (such as
older Android test bots), allocating multiple gigabytes of memory causes the OS
or allocator to abort the test runner process rather than cleanly returning
nullptr. Checking modern CPU extensions (like DotProd on AArch64) identifies
devices with sufficient RAM to safely execute large stress tests while
gracefully skipping elsewhere.

## Examples

Bad:

```cpp
const size_t kSrcSize =
    static_cast<size_t>(kSrcH - 1) * static_cast<size_t>(kSrcStride) + 256u;
uint8_t* src = new (std::nothrow) uint8_t[kSrcSize];
ASSERT_NE(src, nullptr) << "Failed to allocate " << kSrcSize
                        << " bytes (need ~6.4 GiB free)";
```

Good:

```cpp
#if defined(__aarch64__)
  // Infer malloc can accept a large size for cpu with dot product (a76/a55).
  int has_large_malloc = TestCpuFlag(kCpuHasNeonDotProd);
#else
  int has_large_malloc = 1;
#endif
  if (!has_large_malloc) {
    GTEST_SKIP() << "large allocation may assert for " << src_size << " bytes";
  }

  uint8_t* src = new (std::nothrow) uint8_t[src_size];
  if (!src) {
    GTEST_SKIP() << "could not allocate " << src_size << " bytes";
  }
```
