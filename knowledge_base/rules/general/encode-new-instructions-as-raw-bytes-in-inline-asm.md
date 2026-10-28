---
name: libyuv/encode-new-instructions-as-raw-bytes-in-inline-asm
description: When executing newly added CPU instructions in inline assembly for feature detection or probing, encode them as raw bytecode (.byte/.word) rather than unrecognised mnemonics
rubric: correctness
severity: blocking
---

**Rule.** When executing cutting-edge or newly standardized CPU instructions in
inline assembly (such as in CPU feature detection routines or runtime probe
tests), encode the instructions using raw byte sequences (e.g., `.byte` or
`.word`) or guard them with compiler feature/version checks. Do not use plain
instruction mnemonics that may be unrecognized by older compiler and assembler
toolchains in downstream CI environments.

**Why.** Older compiler or assembler versions in downstream projects and CI
environments fail to compile code containing recently introduced instruction
mnemonics (such as new AVX-512, AMX, SVE2, or RISC-V extensions). Emitting raw
bytecode ensures portable assembly across all supported toolchains while still
executing the target instruction.

## Examples

Bad:

```cpp
// Probe for new instruction support using mnemonic
__asm__ volatile("vdpphps %%xmm0, %%xmm0, %%xmm0" : : : "xmm0");
__asm__("csrr %0, vlenb" : "=r"(vlenb));
```

Good:

```cpp
// Probe for new instruction support using raw bytecode
__asm__ volatile(".byte 0x62, 0xf2, 0x7c, 0x08, 0x52, 0xc0" : : : "xmm0");
__asm__(".word 0xC22022F3" /* CSRR t0, vlenb */ : "=r"(vlenb));
```
