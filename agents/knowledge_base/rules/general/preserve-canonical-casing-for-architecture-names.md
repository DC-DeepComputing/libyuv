---
name: libyuv/preserve-canonical-casing-for-architecture-names
description: When naming PascalCase functions, types, or identifiers referencing compound architecture names or proper nouns, preserve canonical casing like LoongArch and AArch64
rubric: readability
severity: should_fix
---

**Rule.** When naming functions, types, constants, or identifiers in PascalCase
or CamelCase that reference compound architecture names or proper nouns (such as
LoongArch or AArch64), preserve the canonical capitalization of each constituent
word (e.g., LoongArchCpuCaps, AArch64CpuCaps) rather than lowercasing subsequent
components (e.g., LoongarchCpuCaps, Aarch64CpuCaps).

**Why.** Preserving canonical capitalization for compound architecture names and
proper nouns ensures consistent naming across library APIs, prevents subtle
spelling divergences between platform ports, and matches official architecture
specifications.

## Examples

Bad:

```cpp
// Bad: Lowercases the second constituent component ('arch' instead of 'Arch').
LIBYUV_API SAFEBUFFERS int LoongarchCpuCaps(void);

// Bad: Lowercases 'Arch' in AArch64.
LIBYUV_API SAFEBUFFERS int Aarch64CpuCaps(unsigned long hwcap);
```

Good:

```cpp
// Good: Preserves canonical compound casing 'LoongArch'.
LIBYUV_API SAFEBUFFERS int LoongArchCpuCaps(void);

// Good: Preserves canonical casing 'AArch64'.
LIBYUV_API SAFEBUFFERS int AArch64CpuCaps(unsigned long hwcap);
```
