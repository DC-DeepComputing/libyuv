---
name: libyuv/sync-comments-with-code-changes
description: When modifying function signatures, parameters, or implementation logic, update or remove accompanying comments to ensure they accurately reflect revised behavior.
rubric: readability
severity: should_fix
---

**Rule.** When changing a function's parameters, signature, or internal
algorithm, inspect all preceding and inline comments and update or delete any
comments that describe obsolete mechanisms, removed parameters, or previous
behavior. Do not leave stale explanations (such as obsolete scale factors, old
parameter names, or superseded conversion formulas) that contradict the actual
code.

**Why.** Outdated comments mislead maintainers about how a function operates,
what units or parameter representations it expects, and why specific operations
are performed, leading to incorrect assumptions and bug regressions.

## Examples

Bad:

```cpp
// Use scale to convert msb formats to lsb, depending how many bits there are:
// 512 = 9 bits
// 1024 = 10 bits
// 4096 = 12 bits
// 65536 = 16 bits
void SplitUVRow_16_C(const uint16_t* src_uv,
                     uint16_t* dst_u,
                     uint16_t* dst_v,
                     int depth,
                     int width);
```

Good:

```cpp
// Convert msb formats to lsb, depending on sample depth.
void SplitUVRow_16_C(const uint16_t* src_uv,
                     uint16_t* dst_u,
                     uint16_t* dst_v,
                     int depth,
                     int width);
```
