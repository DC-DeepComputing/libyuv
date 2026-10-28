---
name: libyuv/source/match-neon-load-store-element-size-to-data-type
description: In Arm NEON load and store assembly and intrinsics, match element size suffixes and register arrangements (e.g., .16/.32, .8h/.4s) to logical data and pixel widths.
rubric: correctness
severity: should_fix
---

**Rule.** In Arm NEON inline assembly and intrinsics (AArch32 and AArch64),
match the element size suffix (such as `.16` or `.32` in
`vld1`/`vst1`–`vld4`/`vst4`) and vector arrangement (such as `.8h` or `.4s` in
`ld1`/`st1`–`ld4`/`st4`) to the logical element or pixel width of the buffer.
For deinterleaving loads and stores (`vld2`/`ld2`, `vld4`/`ld4`) used to
subsample or downsample multi-byte pixel formats (such as 2-byte UV/NV12 pairs,
4-byte ARGB, or 16-bit samples), specify the deinterleave element size matching
the full multi-byte pixel width (`.16`/`.8h` for 2-byte pairs, `.32`/`.4s` for
4-byte pixels) so whole pixels are deinterleaved across registers rather than
splitting individual 8-bit bytes (`.8`/`.8b`/`.16b`). Reserve 8-bit
deinterleaving (`.8`/`.8b`) for operations that intentionally split color
channels into separate registers (such as planar conversion or box averaging).

**Why.** Using matching element sizes documents data layout and lane counts for
downstream SIMD operations. Crucially, for multi-structure deinterleaving
instructions (`vld2`/`ld2`, `vld4`/`ld4`), element size dictates hardware
deinterleaving behavior: loading 16-bit UV data or 32-bit ARGB data with 8-bit
element sizes separates individual color channels across registers instead of
alternating pixel tuples, leading to incorrect downsampling results.

## Examples

Bad:

```cpp
// Bad: Loading 16-bit data with byte suffix .8 instead of matching .16
"vld1.8      {d0}, [%0]!                   \n"  // 01234567 (16b)
"vmovl.u16   q0, d0                        \n"

// Bad: vld2.8 deinterleaves individual 8-bit bytes, splitting U and V channels
"vld2.8      {d0, d2}, [%0]!               \n"  // incorrect UV downsampling
"vld2.8      {d1, d3}, [%0]!               \n"
```

Good:

```cpp
// Good: Loading 16-bit data with matching .16 element-size suffix
"vld1.16     {d0}, [%0]!                   \n"  // 0123 (16b)
"vmovl.u16   q0, d0                        \n"

// Good: vld2.16 deinterleaves 16-bit UV pixel pairs, keeping {U, V} intact
"vld2.16     {d0, d2}, [%0]!               \n"  // correct UV downsampling
"vld2.16     {d1, d3}, [%0]!               \n"
```
