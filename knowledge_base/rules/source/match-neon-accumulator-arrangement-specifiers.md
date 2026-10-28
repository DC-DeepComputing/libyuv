---
name: libyuv/source/match-neon-accumulator-arrangement-specifiers
description: When zero-initializing accumulator registers in AArch64 Neon inline assembly, match vector arrangement specifiers (e.g., .4s, .8h) to subsequent element types.
rubric: readability
severity: nit
---

**Rule.** In AArch64 Neon inline assembly, match the vector register arrangement
specifier when zero-initializing accumulators (e.g. `movi vN.4s, #0` or `movi
vN.8h, #0`) to the logical element size used by subsequent accumulation
instructions (such as `.4s` for 32-bit `udot`/`sdot` or `.8h` for 16-bit
`saddw`/`uaddw`). Do not use byte-level specifiers (`.16b`) when the register is
logically used as a multi-byte element accumulator.

**Why.** Although zeroing a 128-bit SIMD vector with `movi vN.16b, #0` clears
all bytes identically, matching the arrangement specifier to the logical
accumulator element width clarifies the register's logical type to readers,
maintains consistency with subsequent vector operations, and avoids misleading
type mismatches in assembly kernels.

## Examples

Bad:

```cpp
// Bad: .16b specifier used to zero registers accumulated as 32-bit .4s
"movi        v2.16b, #0                    \n"
"movi        v3.16b, #0                    \n"
// ...
"udot        v2.4s, v0.16b, v0.16b         \n"
"udot        v3.4s, v1.16b, v1.16b         \n"
```

Good:

```cpp
// Good: .4s arrangement specifiers match 32-bit element accumulation width
"movi        v2.4s, #0                     \n"
"movi        v3.4s, #0                     \n"
// ...
"udot        v2.4s, v0.16b, v0.16b         \n"
"udot        v3.4s, v1.16b, v1.16b         \n"
```
