---
name: libyuv/no-defines-on-gn-group-targets
rubric: correctness
severity: should_fix
---

**Rule.** In GN build files (`BUILD.gn`, `*.gni`), place preprocessor `defines`
that are read in header files inside a `config()` exported via `public_configs`
or `all_dependent_configs` so dependent targets receive them. For internal or
source-only definitions, specify `defines` directly on compiling targets
(`source_set`, `static_library`, `executable`) rather than on `group` targets.

**Why.** GN `group` targets do not compile sources or propagate their direct
`defines` to dependencies, leaving source files compiled without required
macros. Conversely, defines placed in a target's private `defines` list do not
reach downstream consumers, causing mismatched declarations or build errors when
public headers evaluate those macros.

## Examples

Bad:

```cpp
// Bad: defines on a group target do not apply to sources in dependencies
group("libyuv") {
  public_deps = [ ":libyuv_internal" ]
  defines = [ "HAVE_JPEG" ]
}

// Bad: header-read defines in private target defines do not reach consumers
static_library("libyuv_internal") {
  defines = [ "LIBYUV_DISABLE_NEON" ]
}
```

Good:

```cpp
// Good: source-only defines placed directly on the compiling target
static_library("libyuv_internal") {
  defines = [ "HAVE_JPEG" ]
}

// Good: header-read defines exported via config to all dependent targets
config("libyuv_config") {
  defines = [ "LIBYUV_DISABLE_NEON" ]
}

group("libyuv") {
  public_deps = [ ":libyuv_internal" ]
  all_dependent_configs = [ ":libyuv_config" ]
}
```
