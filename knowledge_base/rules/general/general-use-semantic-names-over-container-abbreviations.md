---
name: libyuv/general-use-semantic-names-over-container-abbreviations
description: When declaring constants, lookup tables, or array variables, use descriptive semantic names (e.g., Coefficients, Table, Indices) instead of container-type suffixes like Arr
rubric: readability
severity: should_fix
---

**Rule.** Do not use truncated container-type abbreviations or suffixes like
'Arr', 'Array', or 'Vec' in identifier names. Instead, name constants, lookup
tables, and variables after the semantic meaning of the data (e.g.,
'Coefficients', 'Indices', 'Constants', 'Table', or 'Weights').

**Why.** Truncated container suffixes like 'Arr' encode container data types
rather than purpose and provide little descriptive value. Naming identifiers
after what the values represent semantically clarifies their mathematical or
algorithmic role and makes code self-documenting.

## Examples

Bad:

```cpp
static const int16_t kAbgrToUvjArr[] = { ... };
static const uint8_t kRawToRgb24IndicesArr[] = { ... };
```

Good:

```cpp
static const int16_t kABGRToUVJCoefficients[] = { ... };
static const uint8_t kRAWToRGB24Indices[] = { ... };
```
