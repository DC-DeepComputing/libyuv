---
name: libyuv/source/rvv-vsetvl-at-loop-start
rubric: safety
severity: should_fix
---

**Rule.** In RISC-V Vector (RVV) strip-mining loops (using intrinsics or inline
assembly vsetvli), configure the vector length at the start of each loop
iteration rather than outside the loop and at the bottom of the loop body.
Ensure vsetvl is called with the remaining width inside the loop before vector
processing.

**Why.** Calling vsetvl at the bottom of a do-while loop executes a redundant
vector configuration with w == 0 on the final iteration right before loop exit.
Placing vl = vsetvl(w) at the beginning of each iteration eliminates the
redundant zero-length instruction and avoids duplicate setup outside the loop.

## Examples

Bad:

```cpp
// Redundant vsetvl outside and at the bottom of loop
size_t vl = __riscv_vsetvl_e8m2(w);
do {
  // process vector elements using vl...
  w -= vl;
  src += vl;
  dst += vl;
  vl = __riscv_vsetvl_e8m2(w);  // Executes redundant vsetvl(0) on exit
} while (w > 0);
```

Good:

```cpp
// vsetvl configured once at start of each iteration
do {
  size_t vl = __riscv_vsetvl_e8m2(w);
  // process vector elements using vl...
  w -= vl;
  src += vl;
  dst += vl;
} while (w > 0);
```
