---
name: libyuv/deps/roll-pinned-deps-alongside-revision-vars
description: When rolling dependencies in DEPS, verify that pinned sub-dependencies like tools and buildtools are updated alongside top-level revision variables
rubric: correctness
severity: should_fix
---

**Rule.** When updating dependency revisions in `DEPS` (such as
`chromium_revision`), verify that all pinned sub-dependencies—including
`src/tools`, `src/buildtools`, `src/build`, and `src/testing`—are updated to
matching revisions corresponding to the new roll. Do not update a top-level
revision variable while leaving pinned repository paths at stale commit hashes.

**Why.** When sub-dependencies specify explicit commit hashes instead of
referencing shared variables, updating only the top-level revision leaves tools
and build scripts out of sync. This causes build breakages and sync failures,
such as clang update scripts failing or invoking mismatched toolchains.

## Examples

Bad:

```cpp
vars = {
  'chromium_revision': '2fec1883d8c89392206801daa91f95eb2f8ab3e1',
}

deps = {
  'src/tools':
    Var('chromium_git') + '/chromium/src/tools' + '@' + '54fd165044db88eca930ab9d20a6340b76136d91',
}
```

Good:

```cpp
vars = {
  'chromium_revision': '2fec1883d8c89392206801daa91f95eb2f8ab3e1',
}

deps = {
  'src/tools':
    Var('chromium_git') + '/chromium/src/tools' + '@' + '73a128cf7335a71785ed9324d6a15b5559ca8050',
}
```
