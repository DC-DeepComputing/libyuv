---
name: libyuv/no-unmotivated-cfi-or-sanitizer-suppressions
rubric: safety
severity: should_fix
---

**Rule.** Do not add `__attribute__((no_sanitize(...)))` (such as
`no_sanitize("cfi-icall")`), `NO_SANITIZE`, or other sanitizer suppressions to
functions without verified necessity and explicit justification. Ensure all
temporary debug attributes and sanitizer bypasses are removed before landing
changes.

**Why.** Suppressing Control Flow Integrity (CFI) or sanitizer checks removes
critical security mitigations and diagnostics, potentially hiding memory errors
or creating exploitable control-flow attack surfaces in production builds.

## Examples

Bad:

```cpp
// Stray or unmotivated CFI suppression added to a row kernel:
LIBYUV_TARGET_AVX2 __attribute__((no_sanitize("cfi-icall")))
void ARGBToUVMatrixRow_AVX2(const uint8_t* src_argb,
                            int src_stride_argb,
                            uint8_t* dst_u,
                            uint8_t* dst_v,
                            int width) {
  // ...
}
```

Good:

```cpp
// Production kernels remain CFI-compliant without sanitizer suppressions:
LIBYUV_TARGET_AVX2
void ARGBToUVMatrixRow_AVX2(const uint8_t* src_argb,
                            int src_stride_argb,
                            uint8_t* dst_u,
                            uint8_t* dst_v,
                            int width) {
  // ...
}
```
