---
name: libyuv/source/post-increment-pointers-in-assembly-loops
description: In ARM/AArch64 NEON assembly loops, use post-increment addressing, prefer ldr over ld1 for contiguous loads, and precalculate offset pointer operands outside loops
rubric: safety
severity: should_fix
---

**Rule.** In ARM and AArch64 NEON inline assembly loops, use post-increment
addressing modes on load/store instructions (e.g., [%0], #8, [%0]!, or
post-indexed ldr/ld1/str/st1) rather than executing separate add or sub pointer
arithmetic instructions inside the loop body. For contiguous vector loads in
AArch64 NEON, prefer ldr over ld1 where applicable. When accessing offset data
(such as adjacent pixels or multiple rows) or unrolling consecutive loads,
pre-calculate separate pointer variables outside the inline assembly block and
pass them as operands. Additionally, avoid back-to-back memory instructions that
post-increment the same base register to prevent Address Generation Interlock
(AGI) stalls on in-order cores.

**Why.** Recalculating pointer offsets with separate add or sub instructions
inside tight loop bodies adds instruction overhead and latency on every
iteration. Post-increment addressing folds pointer arithmetic into memory
operations, eliminating redundant instructions. On certain AArch64
microarchitectures (such as Samsung Exynos), ldr achieves higher throughput and
lower latency than ld1 for contiguous loads. Furthermore, on in-order CPU
microarchitectures (like Cortex-A55), executing consecutive memory instructions
that post-increment the same base register causes AGI stalls because subsequent
loads immediately depend on the updated register.

## Examples

Bad:

```cpp
// Bad: Using ld1 with separate add instructions instead of post-increment ldr
"ld1         {v0.8b}, [%0]                 \n"
"add         %0, %0, #8                    \n"
"ld1         {v1.8b}, [%1]                 \n"
"add         %1, %1, #8                    \n"

// Bad: Adjusting pointer back and forth with add instructions inside the loop
"ld1         {v0.8b}, [%0]                 \n"  // offset 0
"add         %x0, %x0, #1                  \n"
"ld1         {v1.8b}, [%0]                 \n"  // offset 1
"add         %x0, %x0, #7                  \n"

// Bad: Back-to-back post-increments causing AGI stalls on in-order cores
"ld2         {v0.4s, v1.4s}, [%[src]], #32 \n"
"ld2         {v2.4s, v3.4s}, [%[src]], #32 \n"
```

Good:

```cpp
// Good: Contiguous ldr loads using post-increment addressing
"ldr         d0, [%0], #8                  \n"
"ldr         d1, [%1], #8                  \n"

// Good: Pre-compute separate offset pointers outside asm and advance independently
const uint8_t* src_temp = src_ptr + 1;
// ...
"ldr         d0, [%0], #8                  \n"  // load offset 0 and advance %0
"ldr         d1, [%1], #8                  \n"  // load offset 1 and advance %1

// Good: Split base registers to avoid AGI stalls on unrolled loads
"ld2         {v0.4s, v1.4s}, [%[src]]      \n"
"add         %[src], %[src], #64           \n"
"ld2         {v2.4s, v3.4s}, [%[src1]]     \n"
"add         %[src1], %[src1], #64         \n"
```
