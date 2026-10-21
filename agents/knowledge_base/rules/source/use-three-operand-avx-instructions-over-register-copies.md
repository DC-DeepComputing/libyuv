---
name: libyuv/source/use-three-operand-avx-instructions-over-register-copies
rubric: safety
severity: should_fix
---

**Rule.** In x86 AVX, AVX2, and AVX-512 inline assembly and kernels, use
non-destructive 3-operand vector instructions (such as `vpsllw`, `vpsrlw`,
`vpsraw`, `vpand`, `vpor`, `vpxor`, `vpaddw`, `vpsubw`, `vpmullw`,
`vpunpcklbw`/`vpunpckhbw`, or `vpunpckldq`/`vpunpckhdq`) with separate source
and destination registers (`vop src2, src1, dst`) to write results directly into
distinct destination registers. Do not emit redundant vector
register-to-register copy instructions (such as `vmovdqa`, `vmovdqu`, `vmovaps`,
or `vmovups`) prior to performing operations. Additionally, when a shared source
register is consumed by multiple operations (including unpack pairs or
shifts/masks), order and schedule the non-destructive operations writing to
separate destination registers before any operation that mutates or overwrites
the shared source register.

**Why.** Legacy 2-operand SSE instructions are destructive operations that
overwrite the first source operand, necessitating explicit register copies when
input values are reused. In contrast, VEX- and EVEX-encoded AVX instructions
support non-destructive 3-operand forms where the destination register is
independent of both source operands. Ordering operations (such as unpacks) so
that the separate target register is written first preserves the shared source
register intact for subsequent or in-place operations. Emitting redundant
register copy instructions (`vmovdqa`/`vmovdqu`/`vmovaps`/`vmovups`) is an
artifact of porting SSE code that wastes instruction bytes, consumes decode
bandwidth, burns execution port cycles, and increases vector register pressure
in performance-critical SIMD row loops.

## Examples

Bad:

```cpp
// Bad: Redundant vmovdqa register copies before in-place shift operations
"vmovdqa     %%ymm0, %%ymm1                \n"
"vmovdqa     %%ymm0, %%ymm2                \n"
"vpsllw      $0x1, %%ymm1, %%ymm1          \n"
"vpsllw      $0xb, %%ymm2, %%ymm2          \n"

// Bad: Redundant register copy before unpack operations
"vmovdqa     %%ymm0, %%ymm1                \n"
"vpunpcklbw  %%ymm2, %%ymm0, %%ymm0        \n"
"vpunpckhbw  %%ymm2, %%ymm1, %%ymm1        \n"
```

Good:

```cpp
// Good: Direct non-destructive 3-operand AVX instructions write to distinct targets
"vpsllw      $0x1, %%ymm0, %%ymm1          \n"
"vpsllw      $0xb, %%ymm0, %%ymm2          \n"

// Good: Unpack to separate register first to preserve %%ymm0 for in-place unpack
"vpunpckhbw  %%ymm2, %%ymm0, %%ymm1        \n"
"vpunpcklbw  %%ymm2, %%ymm0, %%ymm0        \n"
```
