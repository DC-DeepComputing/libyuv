---
name: libyuv/provide-fallback-return-for-enum-switch
rubric: correctness
severity: blocking
---

**Rule.** In non-void functions that switch over an enum, ensure all control
paths return a value by providing a trailing fallback return (such as `return
-1;`) after the switch block or a `default:` case. Compilers treat enums as
capable of holding integer values outside named enumerators and will fail builds
or warn about missing return values (-Wreturn-type / MSVC C4715) if a non-void
function omits a fallback path.

**Why.** In C and C++, enums can technically hold values outside the declared
enumerators, preventing compilers from proving that an exhaustive enum switch
covers every possible runtime execution path. Without a trailing fallback return
or default case, strict builds fail with missing-return warnings/errors, and
unexpected enum values cause undefined behavior.

## Examples

Bad:

```cpp
int I210ToAR30MatrixFilter(const uint16_t* src_y, ..., FilterMode filter) {
  switch (filter) {
    case kFilterNone:
      return I210ToAR30Matrix(...);
    case kFilterBilinear:
    case kFilterBox:
    case kFilterLinear:
      return I210ToAR30MatrixLinear(...);
  }
}
```

Good:

```cpp
int I210ToAR30MatrixFilter(const uint16_t* src_y, ..., FilterMode filter) {
  switch (filter) {
    case kFilterNone:
      return I210ToAR30Matrix(...);
    case kFilterBilinear:
    case kFilterBox:
    case kFilterLinear:
      return I210ToAR30MatrixLinear(...);
  }
  return -1;
}
```
