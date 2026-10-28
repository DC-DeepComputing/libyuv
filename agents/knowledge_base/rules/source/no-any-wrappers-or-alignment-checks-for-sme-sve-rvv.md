---
name: libyuv/source/no-any-wrappers-or-alignment-checks-for-sme-sve-rvv
description: When implementing or dispatching row kernels for SME, SVE2, or RVV, do not create _Any wrappers or add IS_ALIGNED width dispatch checks
rubric: design
severity: should_fix
---

**Rule.** When implementing or dispatching row functions for
vector-length-agnostic architectures (SME, SVE/SVE2, and RVV), do not create
`_Any` wrapper kernels or add `IS_ALIGNED` destination width checks at dispatch
sites. Assign the architecture-specific kernel directly because hardware
predication and masking handle arbitrary remainder widths natively.

**Why.** Instruction sets like SME, SVE2, and RVV support hardware-level masking
and predicated loop tails (such as `whilelt` in SVE or vector length
configuration in RVV) to process unaligned or remainder elements without scalar
fallbacks. Generating `_Any` wrappers or adding `IS_ALIGNED` dispatch branches
adds redundant code bloat and unnecessary runtime branching.

## Examples

Bad:

```cpp
#if defined(HAS_SCALEROWDOWN2_SME)
  if (TestCpuFlag(kCpuHasSME)) {
    if (filtering == kFilterNone) {
      ScaleRowDown2 = ScaleRowDown2_Any_SME;
      if (IS_ALIGNED(dst_width, 2)) {
        ScaleRowDown2 = ScaleRowDown2_SME;
      }
    }
  }
#endif
```

Good:

```cpp
#if defined(HAS_SCALEROWDOWN2_SME)
  if (TestCpuFlag(kCpuHasSME)) {
    if (filtering == kFilterNone) {
      ScaleRowDown2 = ScaleRowDown2_SME;
    }
  }
#endif
```
