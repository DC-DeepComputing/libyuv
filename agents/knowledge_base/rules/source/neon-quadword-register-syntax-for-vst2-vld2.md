---
name: libyuv/source/neon-quadword-register-syntax-for-vst2-vld2
rubric: readability
severity: blocking
---

**Rule.** In 32-bit ARM NEON assembly (AArch32), when using structured load or
store instructions like vst2 or vld2 to transfer two 128-bit vector streams,
specify the register operand list using two quadword registers (e.g. {q0, q1})
rather than four doubleword registers (e.g. {d0, d1, d2, d3}). Keep the register
count in the operand list matching the structure count of the instruction.

**Why.** vstN and vldN instructions interleave N structures. Listing four
doubleword registers for a two-structure vst2 or vld2 instruction obscures the
structure count and makes the instruction easily confused with a 4-structure
vst4 or vld4 operation.

## Examples

Bad:

```cpp
"vst2.16     {d0, d1, d2, d3}, [%1]!       \n"  // store
```

Good:

```cpp
"vst2.16     {q0, q1}, [%1]!               \n"  // store
```
