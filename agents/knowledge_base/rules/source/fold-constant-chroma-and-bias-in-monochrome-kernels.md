---
name: libyuv/source/fold-constant-chroma-and-bias-in-monochrome-kernels
rubric: safety
severity: should_fix
---

**Rule.** In monochrome YUV to RGB/ARGB conversion kernels (such as I400/J400 to
ARGB) where chroma U and V components are invariant constants, fold constant UV
multiplication products and channel biases into single signed delta offsets
outside the loop during setup. Avoid performing separate additive and
subtractive bias operations per channel inside the inner loop.

**Why.** Because chroma is constant in monochrome formats, the combined chroma
and bias contributions per RGB channel are loop-invariant. Precalculating the
net signed bias outside the loop eliminates redundant arithmetic instructions
per pixel and allows applying the channel offset with a single saturating
add/accumulate instruction inside the inner loop.

## Examples

Bad:

```cpp
// In the inner loop, adding constant chroma and subtracting bias separately:
#define I400TORGB_SVE                                    \
  "umulh      z0.h, z24.h, z0.h              \n" /* Y */ \
  "add        z17.h, z0.h, z26.h             \n" /* G */ \
  "add        z16.h, z0.h, z4.h              \n" /* B */ \
  "add        z18.h, z0.h, z5.h              \n" /* R */ \
  "uqsub      z17.h, z17.h, z6.h             \n" /* G */ \
  "uqsub      z16.h, z16.h, z25.h            \n" /* B */ \
  "uqsub      z18.h, z18.h, z27.h            \n" /* R */
```

Good:

```cpp
// Fold constant UV products and biases outside the loop:
"sub          z4.h, z4.h, z25.h              \n" /* B bias */ \
"sub          z5.h, z5.h, z27.h              \n" /* R bias */ \
"sub          z6.h, z26.h, z6.h              \n" /* G bias */

// Inner loop applies combined signed offset with single saturating add:
#define I400TORGB_SVE                                    \
  "umulh      z18.h, z24.h, z0.h             \n" /* Y */ \
  "movprfx    z16, z18                       \n"         \
  "usqadd     z16.h, p0/m, z16.h, z4.h       \n" /* B */ \
  "movprfx    z17, z18                       \n"         \
  "usqadd     z17.h, p0/m, z17.h, z6.h       \n" /* G */ \
  "usqadd     z18.h, p0/m, z18.h, z5.h       \n" /* R */
```
