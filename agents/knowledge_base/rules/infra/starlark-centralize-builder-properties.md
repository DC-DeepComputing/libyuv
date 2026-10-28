---
name: libyuv/infra/starlark-centralize-builder-properties
description: When defining builder configurations in LUCI Starlark (*.star), centralize shared builder properties and execution settings in helper functions returning new dictionaries
rubric: design
severity: should_fix
---

**Rule.** In LUCI Starlark configuration files (`*.star`), centralize shared
builder property dictionaries (such as remote compilation, Goma/RBE/Siso
settings, or OS dimensions) into dedicated helper functions that construct and
return a dictionary. Avoid duplicating default property dictionaries and
per-platform updates across individual CI and try builder factory functions.

**Why.** CI, try, and cron builders frequently share identical toolchain, remote
execution, and environment configurations. Centralizing dictionary generation
into reusable helper functions establishes a single source of truth, eliminates
duplicate dictionary manipulation, and prevents configuration drift between CI
and CQ try builders.

## Examples

Bad:

```cpp
def ci_builder(name, os, category, short_name = None):
    properties = {
        "mastername": "client.libyuv",
        "$build/goma": {
            "server_host": "goma.chromium.org",
            "use_luci_auth": True,
        },
    }
    if os == "win":
        properties["$build/goma"].update({"enable_ats": True})
    return luci.builder(..., properties = properties)
```

Good:

```cpp
def get_goma_properties(enable_ats = False):
    goma_properties = {
        "server_host": "goma.chromium.org",
        "use_luci_auth": True,
    }
    if enable_ats:
        goma_properties["enable_ats"] = True
    return {"$build/goma": goma_properties}

def ci_builder(name, os, category, short_name = None):
    properties = {"mastername": "client.libyuv"}
    if os == "win":
        properties.update(get_goma_properties(enable_ats = True))
    return luci.builder(..., properties = properties)
```
