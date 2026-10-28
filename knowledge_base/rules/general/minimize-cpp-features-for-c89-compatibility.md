---
name: libyuv/minimize-cpp-features-for-c89-compatibility
description: When writing or modifying libyuv source or test code, avoid modern C++ features such as auto, range-for loops, lambdas, and templates to maintain C89 compatibility
rubric: readability
severity: should_fix
---

**Rule.** Avoid modern C++ language features (such as `auto`, range-based `for`
loops, lambdas, templates, and `constexpr`) in both library and test code. Keep
C++ usage minimal and prefer standard C/C89-compatible idioms (such as explicit
types, index-based loops, and struct pointers) to ensure portability across
constrained compilers.

**Why.** Libyuv is widely ported across constrained platforms and legacy
toolchains requiring strict C89 or minimal C++ compatibility; using modern C++
features breaks builds on those environments.

## Examples

Bad:

```cpp
for (const auto& c : cases) {
  TestColorConversion(c.format, c.width, c.height);
}
```

Good:

```cpp
for (int i = 0; i < kTestCaseCount; ++i) {
  const struct ColorTestCase* c = &kTestCases[i];
  TestColorConversion(c->format, c->width, c->height);
}
```
