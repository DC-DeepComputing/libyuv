---
name: libyuv/source/document-non-standard-row-includes
description: When including higher-level or secondary headers in low-level row kernel files, document the required symbol or purpose with an inline comment
rubric: design
severity: nit
---

**Rule.** In low-level row kernel files (`source/row_*.cc`), restrict `#include`
directives to `"libyuv/row.h"` and platform intrinsic headers whenever possible.
When secondary or higher-level headers (such as `"libyuv/convert_from_argb.h"`,
`"libyuv/convert_argb.h"`, or `<string.h>`) are necessary (e.g. for shared
constants or helper functions), document the inclusion with an inline `// For
<Symbol/Reason>` comment.

**Why.** Row implementation files are low-level primitives meant to keep
dependencies minimal and avoid architectural layering violations. Documenting
why a secondary or higher-level header is required clarifies non-obvious
dependencies and prevents accidental include coupling or unused include
accumulation.

## Examples

Bad:

```cpp
// In source/row_neon.cc
#include "libyuv/row.h"
#include "libyuv/convert_from_argb.h"
```

Good:

```cpp
// In source/row_neon.cc
#include "libyuv/row.h"
#include "libyuv/convert_from_argb.h"  // For ArgbConstants
```
