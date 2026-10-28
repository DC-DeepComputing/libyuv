---
name: libyuv/source/document-architecture-specific-constants-and-avoid-dummy-mac
description: When defining platform-specific SIMD constants or matrix macros, explain architectural math differences in comments and avoid dummy constants or dead macro parameters
rubric: readability
severity: nit
---

**Rule.** Avoid vague comments like 'We don't care about these' or defining
dummy zero constants for architecture-specific macro parameters. Explain the
architectural or mathematical rationale for omitted constants (such as signed
arithmetic eliminating the need for bias terms), or refactor platform-specific
macro signatures so unused parameters and dummy constants are not defined for
targets that do not use them.

**Why.** Dismissive or vague comments fail to explain why calculation steps or
constants are omitted on certain CPU targets. Providing technical explanations
or refactoring macro signatures prevents confusion about whether omitted terms
were intentional and keeps macro interfaces clean across platforms.

## Examples

Bad:

```cpp
#if defined(__aarch64__) || defined(__arm__)
#define BB (UB * 128 - YB)
#else
// We don't care these.
#define BB 0
#endif
#define MAKEYUVCONSTANTS(name, YG, YB, UB, UG, VG, VR, BB, BG, BR) ...
```

Good:

```cpp
#if defined(__aarch64__) || defined(__arm__)
// Bias values to round, and subtract 128 from U and V.
#define BB (UB * 128 - YB)
#define YUBCONSTANTSBODY(YG, YB, UB, UG, VG, VR) \
  {{UB, VR, UG, VG, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
   {YG, BB, BG, BR, YB, 0, 0, 0}}
#else
// Intel uses signed math, so bias constants (BB, BG, BR) are unneeded.
#define YUBCONSTANTSBODY(YG, YB, UB, UG, VG, VR) ...
#endif
#define MAKEYUVCONSTANTS(name, YG, YB, UB, UG, VG, VR) ...
```
