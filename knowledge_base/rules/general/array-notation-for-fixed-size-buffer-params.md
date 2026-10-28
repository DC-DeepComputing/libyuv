---
name: libyuv/array-notation-for-fixed-size-buffer-params
description: When declaring C or C++ functions that expect a fixed-size buffer argument, use array notation (type param[N]) instead of a raw pointer
rubric: readability
severity: nit
---

**Rule.** When declaring or defining a C or C++ function that expects a
fixed-size array or buffer, declare the parameter using array notation with its
expected element count (e.g. int version[2] or int cpu_info[4]) rather than a
raw pointer (int* version). This documents the required buffer size in the
function signature for callers and provides size hints to static analysis and
compiler bounds diagnostics.

**Why.** Declaring fixed buffer parameters as raw pointers obscures the required
capacity from readers and callers, making callers guess how many elements are
expected or inspect the implementation. Array notation documents the expected
buffer bounds in the interface and provides size hints to compiler
bounds-checking warnings (e.g. -Warray-bounds) and static analysis.

## Examples

Bad:

```cpp
// Ambiguous buffer size: caller cannot tell how many elements are expected.
static void KernelVersion(int* version) {
  version[0] = 0;
  version[1] = 0;
}
```

Good:

```cpp
// Clear fixed-size expectation; aids readers and compiler bounds checks.
static void KernelVersion(int version[2]) {
  version[0] = 0;
  version[1] = 0;
}
```
