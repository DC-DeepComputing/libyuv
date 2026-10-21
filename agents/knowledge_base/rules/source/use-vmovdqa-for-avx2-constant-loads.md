---
name: libyuv/source/use-vmovdqa-for-avx2-constant-loads
rubric: safety
severity: should_fix
---

**Rule.** In AVX2 and AVX-512 inline assembly kernels, use direct aligned vector
loads (`vmovdqa` or `vmovdqa32`) rather than 128-bit broadcast instructions
(`vbroadcastf128` or `vbroadcasti128`) when loading full 32-byte (256-bit)
constant tables or struct arrays (such as `ArgbConstants` or `YuvConstants`)
into YMM registers. Reserve 128-bit broadcast instructions for 16-byte constants
that must be duplicated across vector lanes.

**Why.** `ArgbConstants` and `YuvConstants` provide full 32-byte aligned
constant arrays designed for 256-bit SIMD registers. Using `vmovdqa` directly
loads the full 256-bit vector in a single integer load instruction, whereas
`vbroadcastf128` is a floating-point broadcast intended for 128-bit sources and
can introduce domain crossing or unnecessary broadcast execution overhead.

## Examples

Bad:

```cpp
// Loading 32-byte constant arrays from ArgbConstants into YMM registers
"vbroadcastf128 0x20(%4),%%ymm3            \n"  // kRGBToU (32 bytes)
"vbroadcastf128 0x40(%4),%%ymm4            \n"  // kRGBToV (32 bytes)
```

Good:

```cpp
// Loading 32-byte constant arrays from ArgbConstants into YMM registers
"vmovdqa     0x20(%4),%%ymm3               \n"  // kRGBToU (32 bytes)
"vmovdqa     0x40(%4),%%ymm4               \n"  // kRGBToV (32 bytes)
```
