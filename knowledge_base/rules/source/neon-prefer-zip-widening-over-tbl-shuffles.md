---
name: libyuv/source/neon-prefer-zip-widening-over-tbl-shuffles
description: In Arm Neon inline assembly routines unpacking or replicating vector data, prefer in-register permutes (zip1/zip2) or widening operations over static shuffle tables and tbl instructions
rubric: safety
severity: should_fix
---

**Rule.** In Arm Neon and SVE inline assembly routines, prefer in-register
permute instructions (such as zip1/zip2 of a vector with itself to duplicate
elements, or uzp/trn) or widening operations (such as umull/uaddl) over static
shuffle tables in memory and tbl instructions when unpacking, replicating, or
widening vector data. Reserve tbl lookup tables for complex, non-linear
permutations that cannot be expressed with register permutes or widening
arithmetic.

**Why.** Loading static shuffle masks from memory incurs cache and memory bus
overhead, consumes extra SIMD registers for table masks, and executes
higher-latency tbl instructions. In-register permutes (zip1/zip2) and widening
instructions (umull/uaddl) execute entirely within the SIMD execution pipeline
with lower latency and without memory loads.

## Examples

Bad:

```cpp
// Bad: Loading a static lookup table from memory to duplicate bytes using tbl.
static const uint8_t kConvert8To16Shuffle[] = {
    0, 0, 255, 255, 1, 1, 255, 255, 2, 2, 255, 255, 3, 3, 255, 255
};
// ...
ldp    q27, q28, [%[kShuffle]]
tbl    v3.16b, {v0.16b}, v28.16b
tbl    v2.16b, {v0.16b}, v27.16b
mul    v3.4s, v3.4s, v31.4s
```

Good:

```cpp
// Good: Using in-register zip and widening arithmetic to duplicate and extend elements.
zip2   v1.16b, v0.16b, v0.16b
zip1   v0.16b, v0.16b, v0.16b
umull2 v3.4s, v1.8h, v31.8h
umull  v2.4s, v1.4h, v31.4h
```
