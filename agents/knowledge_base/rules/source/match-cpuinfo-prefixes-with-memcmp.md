---
name: libyuv/source/match-cpuinfo-prefixes-with-memcmp
description: When parsing /proc/cpuinfo lines in cpu_id.cc, match full key-value prefixes directly with memcmp in an else-if ladder instead of nested strstr searches
rubric: readability
severity: nit
---

**Rule.** When parsing line-oriented `/proc/cpuinfo` streams in
`source/cpu_id.cc`, match known key-value prefix strings directly using `memcmp`
within an `else if` ladder rather than nesting a generic key prefix check with
separate `strstr` substring scans. Ensure prefix length arguments match the
exact string literal length including whitespace and delimiters.

**Why.** Directly matching the full key-value prefix via `memcmp` avoids
unnecessary linear substring scans with `strstr` over the buffer, and chaining
checks in an `else if` ladder prevents redundant pattern evaluations after
earlier line headers have already matched.

## Examples

Bad:

```cpp
if (memcmp(cpuinfo_line, "vendor_id", 9) == 0) {
  if (strstr(cpuinfo_line, "GenuineIntel") ||
      strstr(cpuinfo_line, "AuthenticAMD")) {
    flag |= kCpuHasRVV;
  }
}
```

Good:

```cpp
else if ((memcmp(cpuinfo_line, "vendor_id\t: GenuineIntel", 24) == 0) ||
         (memcmp(cpuinfo_line, "vendor_id\t: AuthenticAMD", 24) == 0)) {
  flag |= kCpuHasRVV;
}
```
