---
name: libyuv/document-bounds-preventing-arithmetic-overflow
description: When arithmetic relies on domain- or format-specific limits, document why calculations cannot overflow using concrete numerical bounds and operand ranges rather than vague descriptions
rubric: readability
severity: nit
---

**Rule.** Precede arithmetic operations (such as buffer allocations, stride
multiplications, or dimension scaling) that rely on domain, format, or header
field bounds with comments detailing why integer overflow is impossible. State
concrete numerical or type bounds (such as UINT16_MAX, INT_MAX, bit lengths, or
exact step counts) rather than leaving safety implicit, adding redundant runtime
checks, or using vague relative descriptions (such as 'slightly larger' or
'small enough').

**Why.** Without comments explaining format-specific or protocol-defined
constraints, arithmetic operations on parser and image dimensions appear
vulnerable to integer overflow, leading to false-positive security warnings,
redundant checks, or confusion during audits. Furthermore, vague descriptions
leave reference thresholds ambiguous, preventing reviewers and maintainers from
verifying whether an arithmetic calculation is truly safe from overflow.

## Examples

Bad:

```cpp
// Variant 1: Implicit safety with redundant 64-bit casting and bounds check
int64_t databuf_size = (int64_t)scanlines_size * databuf_stride;
if (databuf_size < 0 || databuf_size > INT_MAX) {
  return LIBYUV_FALSE;
}
databuf_[i] = new uint8_t[databuf_size];

// Variant 2: Vague safety comment lacking concrete reference bounds
// JPEG width is stored as an u16, so `databuf_stride` is at most slightly larger.
int databuf_size = scanlines_size * databuf_stride;
```

Good:

```cpp
// Cannot overflow:
//  - JPEG width is stored as an u16, so `databuf_stride` (width rounded up to
//    DCTSIZE) is at most slightly larger than UINT16_MAX.
//  - Sampling factor is stored in 4 bits, so scanlines_size is < 16.
int databuf_size = scanlines_size * databuf_stride;
databuf_[i] = new uint8_t[databuf_size];
```
