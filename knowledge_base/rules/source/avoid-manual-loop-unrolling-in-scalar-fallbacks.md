---
name: libyuv/source/avoid-manual-loop-unrolling-in-scalar-fallbacks
description: When writing or modifying scalar C fallback row functions, avoid manual loop unrolling and write simple scalar loops to allow compiler auto-vectorization.
rubric: safety
severity: nit
---

**Rule.** In scalar C fallback implementations (such as *_common.cc row
functions), write simple single-element iteration loops instead of manually
unrolling iterations with remainder tail checks. Modern optimizing compilers
(such as Clang) can automatically vectorize and unroll simple scalar loops,
whereas manual unrolling complicates pattern matching, degrades maintenance, and
often inhibits auto-vectorization unless multi-compiler profiling demonstrates a
clear win.

**Why.** Manual loop unrolling and residual tail handling increase code
complexity and obscure simple loop idioms from compiler auto-vectorizers. Clean,
non-unrolled scalar loops are easier to maintain and allow modern compilers to
optimize and vectorize the code across target architectures.

## Examples

Bad:

```cpp
int x;
for (x = 0; x < width - 1; x += 2) {
  dst_u[x] = src_uv[0] >> shift;
  dst_v[x] = src_uv[1] >> shift;
  dst_u[x + 1] = src_uv[2] >> shift;
  dst_v[x + 1] = src_uv[3] >> shift;
  src_uv += 4;
}
if (width & 1) {
  dst_u[width - 1] = src_uv[0] >> shift;
  dst_v[width - 1] = src_uv[1] >> shift;
}
```

Good:

```cpp
int x;
for (x = 0; x < width; ++x) {
  dst_u[x] = src_uv[0] >> shift;
  dst_v[x] = src_uv[1] >> shift;
  src_uv += 2;
}
```
