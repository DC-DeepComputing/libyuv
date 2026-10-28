---
name: libyuv/unit_test/adapt-test-output-column-widths
description: When formatting test or benchmark console tables across variable data ranges, adapt column widths dynamically instead of unconditionally widening fixed print fields
rubric: readability
severity: nit
---

**Rule.** When formatting test, histogram, or benchmark console output that
supports both standard (e.g., 8-bit) and extended (e.g., 10-bit or 12-bit) data
ranges, adapt column formatting widths dynamically or preserve compact
formatting for standard ranges. Do not unconditionally increase fixed print
field widths across all cases when narrower formatting suffices for standard
outputs.

**Why.** Hardcoding wide column widths (such as %10d) makes standard 8-bit test
and histogram output excessively wide and harder to read in terminal output when
narrower column widths are sufficient.

## Examples

Bad:

```cpp
template <size_t N = 256>
static void PrintHistogram(int rh[N], int gh[N], int bh[N]) {
  for (int i = 0; i < N; ++i) {
    if (rh[i] || gh[i] || bh[i]) {
      printf("\t%10d", i - N / 2);
    }
  }
}
```

Good:

```cpp
static void PrintHistogram(int* rh, int* gh, int* bh, int count) {
  int width = count >= 20000 ? 6 : (count >= 2000 ? 5 : 4);
  char format[16];
  snprintf(format, sizeof(format), "\t%%%dd", width);
  for (int i = 0; i < count; ++i) {
    if (rh[i] || gh[i] || bh[i]) {
      printf(format, i - count / 2);
    }
  }
}
```
