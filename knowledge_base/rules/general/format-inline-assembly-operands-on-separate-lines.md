---
name: libyuv/format-inline-assembly-operands-on-separate-lines
description: When writing inline assembly statements, place output, input, and clobber sections on separate lines, and order "memory" and "cc" first in clobber lists
rubric: readability
severity: nit
---

**Rule.** In multiline inline assembly (`asm` or `__asm__`) statements, place
output operands, input operands, and clobber lists on separate lines, with each
colon (`:`) separator on its own line. Additionally, in clobber lists, place
general compiler side-effect constraints (`"memory"` and `"cc"`) at the
beginning before architecture-specific register names.

**Why.** Placing colon-separated assembly sections on dedicated lines makes
register constraints and modified registers easy to inspect during review.
Ordering `"memory"` and `"cc"` first ensures critical compiler side-effect
constraints are immediately visible and maintains consistency across all libyuv
assembly kernels.

## Examples

Bad:

```cpp
// Bad: Condensing multiple colon sections onto a single line.
asm(".inst 0x04bf5020    \n"
    "mov %w[sve_vl], w0  \n"
    : [sve_vl] "=r"(sve_vl)::"x0");

// Bad: Register clobbers listed before "memory" and "cc".
asm volatile(
    "..."
    :
    :
    : "v0", "v1", "memory", "cc");
```

Good:

```cpp
// Good: Placing output, input, and clobber sections on separate lines.
asm(".inst 0x04bf5020    \n"
    "mov %w[sve_vl], w0  \n"
    : [sve_vl] "=r"(sve_vl)
    :
    : "x0");

// Good: "memory" and "cc" ordered first in the clobber list.
asm volatile(
    "..."
    :
    :
    : "memory", "cc", "v0", "v1");
```
