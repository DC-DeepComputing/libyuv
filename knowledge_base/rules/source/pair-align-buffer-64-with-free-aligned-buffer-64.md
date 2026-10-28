---
name: libyuv/source/pair-align-buffer-64-with-free-aligned-buffer-64
description: When allocating temporary aligned buffers with align_buffer_64 or align_buffer_page_end, pair each allocation with free_aligned_buffer_* across all exit paths and do not free sub-buffers
rubric: safety
severity: blocking
---

**Rule.** Always pair every align_buffer_64(var, size) or align_buffer_page_end
allocation with its corresponding free_aligned_buffer_64(var) or
free_aligned_buffer_page_end across all exit paths, including early error
returns. Call free_aligned_buffer_* strictly on the base allocated buffer; when
consolidating multiple temporary row or plane buffers into a single buffer with
offset pointers, do not call free_aligned_buffer_* on derived sub-buffer offset
pointers.

**Why.** align_buffer_64 and align_buffer_page_end dynamically allocate heap
memory using malloc() or system allocators and declare a hidden var##_mem
tracking pointer that free_aligned_buffer_* frees. Failing to invoke
free_aligned_buffer_* on normal or error exit paths causes a permanent memory
leak. Conversely, calling free_aligned_buffer_* on derived offset pointers fails
or corrupts state because sub-buffers lack macro-generated allocation trackers.

## Examples

Bad:

```cpp
// 1. Leaking allocated buffers on early exit:
{
  align_buffer_64(row_u, width);
  align_buffer_64(row_v, width);
  if (!row_u || !row_v) {
    return 1;  // Bug: row_u is leaked if row_v allocation fails.
  }
  RAWToARGBRow(src_raw, row_u, width);
  ARGBToUVJ444Row(row_u, dst_u, dst_v, width);
  // Bug: missing free_aligned_buffer_64 before exiting scope.
}

// 2. Calling free on sub-buffer offset pointer:
align_buffer_64(row, row_size * 2 + src_width * 4);
uint8_t* row_argb = row + row_size * 2;
// ...
free_aligned_buffer_64(row);
free_aligned_buffer_64(row_argb);  // Error: row_argb_mem does not exist.
```

Good:

```cpp
// 1. Properly freeing on all error and normal return paths:
{
  align_buffer_64(row_u, width);
  align_buffer_64(row_v, width);
  if (!row_u || !row_v) {
    free_aligned_buffer_64(row_u);
    free_aligned_buffer_64(row_v);
    return 1;
  }
  RAWToARGBRow(src_raw, row_u, width);
  ARGBToUVJ444Row(row_u, dst_u, dst_v, width);
  free_aligned_buffer_64(row_u);
  free_aligned_buffer_64(row_v);
}

// 2. Freeing only the base allocation when using sub-buffer offsets:
align_buffer_64(row, row_size * 2 + src_width * 4);
uint8_t* row_argb = row + row_size * 2;
// ...
free_aligned_buffer_64(row);
```
