---
name: libyuv/omit-redundant-base-architecture-macro-in-feature-guards
description: When guarding architecture vector extensions (e.g., RVV, LSX, LASX), omit redundant base architecture checks (like __riscv or __loongarch__) in preprocessor guards
rubric: readability
severity: blocking
---

**Rule.** When guarding architecture-specific vector extensions, header
inclusions (e.g., `<riscv_vector.h>`), or row function declarations/definitions
using compiler predefined extension macros (such as `__riscv_vector`,
`__loongarch_sx`, `__loongarch_asx`, or `__mips_msa`), do not redundantly check
the base architecture macro (such as `__riscv`, `__loongarch__`, or `__mips`).
Guard feature definitions and includes using `!defined(LIBYUV_DISABLE_<FEATURE>)
&& defined(<EXTENSION_MACRO>)`.

**Why.** Compiler predefined extension macros are target-specific and only
defined when targeting that architecture with the respective extension enabled.
Testing the base architecture macro alongside the extension macro is redundant
and clutters preprocessor conditions.

## Examples

Bad:

```cpp
// Redundant __riscv check when guarding RVV header includes and function declarations
#if !defined(LIBYUV_DISABLE_RVV) && defined(__riscv) && defined(__riscv_vector)
#include <riscv_vector.h>
#define HAS_AB64TOARGBROW_RVV
#define HAS_ARGBTOAB64ROW_RVV
#endif

// Redundant __loongarch__ check when guarding LSX functions
#if !defined(LIBYUV_DISABLE_LSX) && defined(__loongarch__) && defined(__loongarch_sx)
#define HAS_I422TOARGBROW_LSX
#endif
```

Good:

```cpp
// Check only the extension macro and disable toggle for RVV
#if !defined(LIBYUV_DISABLE_RVV) && defined(__riscv_vector)
#include <riscv_vector.h>
#define HAS_AB64TOARGBROW_RVV
#define HAS_ARGBTOAB64ROW_RVV
#endif

// Check only the extension macro and disable toggle for LSX
#if !defined(LIBYUV_DISABLE_LSX) && defined(__loongarch_sx)
#define HAS_I422TOARGBROW_LSX
#endif
```
