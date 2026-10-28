---
name: libyuv/api/document-format-conversion-macro-aliases
description: When defining preprocessor macro aliases for format conversion functions across bit depths or pixel layouts, document why the implementations are equivalent
rubric: readability
severity: should_fix
---

**Rule.** When aliasing image format conversion functions using #define (such as
aliasing higher bit-depth conversions or symmetric channel permutations),
document in a preceding comment why the implementations are equivalent (e.g.,
explaining that formats share most-significant-bit alignment or identical memory
layouts). Keep the explanation alongside the macro declarations so users and
maintainers understand why a dedicated conversion routine is not required.

**Why.** Macro aliases between different pixel formats or bit depths (such as
aliasing P012 to P010) appear non-obvious or like potential copy-paste errors
without context. Documenting shared memory representations, MSB alignment, or
channel symmetry clarifies why the underlying conversion routine is identical
and prevents accidental reimplementation.

## Examples

Bad:

```cpp
// Convert P012 to ARGB with matrix.
#define P012ToARGBMatrix P010ToARGBMatrix
#define P012ToAR30Matrix P010ToAR30Matrix
```

Good:

```cpp
// P012 and P010 use most significant bits so the conversion is the same.
// Convert P012 to ARGB with matrix.
#define P012ToARGBMatrix P010ToARGBMatrix
#define P012ToAR30Matrix P010ToAR30Matrix
```
