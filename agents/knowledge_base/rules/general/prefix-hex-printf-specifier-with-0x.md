---
name: libyuv/prefix-hex-printf-specifier-with-0x
description: When printing or formatting hexadecimal values, masks, or flags using printf-family format specifiers, explicitly prefix %x or %X with 0x (e.g. 0x%x).
rubric: readability
severity: nit
---

**Rule.** Prefix hexadecimal format specifiers in printf-family functions with
'0x' (e.g., '0x%x', '0x%X', or '0x%08x') when outputting flags, masks, or hex
values. Avoid bare '%x' or '%X' without a prefix to prevent ambiguity between
hexadecimal and decimal representations.

**Why.** A bare '%x' specifier outputs raw hex digits without an indicator,
creating ambiguity over whether numeric values like '30000001' or '1234' are
decimal or hexadecimal. Explicitly prefixing with '0x' makes the number base
immediately clear in log and diagnostic output.

## Examples

Bad:

```cpp
printf("Cpu Flags %x\n", cpu_flags);
printf("Bitmask: %08x\n", mask);
```

Good:

```cpp
printf("Cpu Flags 0x%x\n", cpu_flags);
printf("Bitmask: 0x%08x\n", mask);
```
