---
name: libyuv/source/document-shuffle-table-formats-in-register-order
description: When documenting SIMD shuffle tables or masks for format conversions, use comments matching canonical register-order FOURCC format names rather than stale copied descriptions
rubric: readability
severity: should_fix
---

**Rule.** When defining or updating format conversion helper tables (such as
SIMD kShuffleMask* arrays and shuffle tables), document them with comments that
accurately state the source and destination formats using canonical
register-order FOURCC names (e.g., 'AR64 to AB64'). Ensure comments reflect the
actual channel permutation implemented by the mask and avoid leaving stale,
copy-pasted format descriptions from template tables.

**Why.** In libyuv, FOURCC format identifiers specify channel order as stored in
registers rather than memory byte order. Outdated or copied shuffle table
comments mislead readers regarding the actual channel layout and conversion
being performed, making SIMD kernel maintenance and format auditing error-prone.

## Examples

Bad:

```cpp
// Shuffle table for converting BGRA to ARGB.
static const uvec8 kShuffleMaskAR64ToAB64 = {
    4u, 5u, 2u, 3u, 0u, 1u, 6u, 7u, 12u, 13u, 10u, 11u, 8u, 9u, 14u, 15u};
```

Good:

```cpp
// Shuffle table for converting AR64 to AB64.
static const uvec8 kShuffleMaskAR64ToAB64 = {
    4u, 5u, 2u, 3u, 0u, 1u, 6u, 7u, 12u, 13u, 10u, 11u, 8u, 9u, 14u, 15u};
```
