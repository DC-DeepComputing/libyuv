---
name: libyuv/source/order-2d-transform-loops-to-write-destination-rows
rubric: safety
severity: should_fix
---

**Rule.** When implementing 2D pixel transformations or transpositions (such as
90/270-degree rotation or matrix transposes), order the loop nest so the inner
loop writes sequentially across destination rows and reads columns from the
source buffer. Avoid inner loops that write strided or backwards destination
columns.

**Why.** Contiguous sequential writes allow CPU store buffers and
write-combining buffers to coalesce memory stores into full cache lines,
maximizing memory bus throughput. Strided destination writes prevent write
combining and cause severe memory penalties, whereas strided column reads are
cached across consecutive iterations.

## Examples

Bad:

```cpp
// Bad: Inner loop writes strided destination columns.
for (int i = 0; i < width; ++i) {
  for (int j = 0; j < height; ++j) {
    dst[j * dst_stride + i] = src[i * src_stride + j];
  }
}
```

Good:

```cpp
// Good: Inner loop writes contiguous destination rows and reads source columns.
for (int i = 0; i < width; ++i) {
  for (int j = 0; j < height; ++j) {
    dst[i * dst_stride + j] = src[j * src_stride + i];
  }
}
```
