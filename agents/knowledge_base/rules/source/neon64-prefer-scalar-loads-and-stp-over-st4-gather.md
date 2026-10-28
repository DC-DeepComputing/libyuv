---
name: libyuv/source/neon64-prefer-scalar-loads-and-stp-over-st4-gather
description: In AArch64 inline assembly gathering non-contiguous scalar values without SIMD math, prefer scalar loads and stp pair stores over st4 lane stores
rubric: safety
severity: should_fix
---

**Rule.** In AArch64 NEON inline assembly routines that gather or pack
non-contiguous or strided scalar values (such as 32-bit words) without
performing SIMD vector calculations, prefer scalar general-purpose register
loads (ldr w) combined with store pairs (stp) over vector single-lane loads (ldr
s) and single-element lane structured stores (st4 with lane index, e.g., st4
{v0.s, v1.s, v2.s, v3.s}[0]).

**Why.** Single-element lane stores (st4 ...[0]) and lane insertion operations
incur severe execution latency penalties (approximately 42 cycles on
microarchitectures like Cortex-A510) due to complex lane-insert and store
micro-operations. When data values are merely gathered and packed without SIMD
transformations, scalar integer GPR loads (ldr w) and paired stores (stp)
execute with significantly lower latency and higher throughput across all
AArch64 cores.

## Examples

Bad:

```cpp
// Inefficient: Vector single-lane loads and st4 lane store incur ~42 cycle latency on Cortex-A510
"ldr         s0, [%[src], %[i]]            \n"
"ldr         s1, [%[src1], %[i]]           \n"
"ldr         s2, [%[src2], %[i]]           \n"
"ldr         s3, [%[src3], %[i]]           \n"
"st4         {v0.s, v1.s, v2.s, v3.s}[0], [%[dst]], #16 \n"
```

Good:

```cpp
// Efficient: Scalar GPR loads and paired stp stores avoid lane store latency penalties
"ldr         w10, [%[src], %[i]]           \n"
"ldr         w11, [%[src1], %[i]]          \n"
"ldr         w12, [%[src2], %[i]]          \n"
"ldr         w13, [%[src3], %[i]]          \n"
"stp         w10, w11, [%[dst]], #8        \n"
"stp         w12, w13, [%[dst]], #8        \n"
```
