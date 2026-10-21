---
name: libyuv/source/prefer-arithmetic-division-over-signed-shifts
rubric: correctness
severity: should_fix
---

**Rule.** Use arithmetic division and multiplication (e.g., width / 16 or width
* 4) rather than bitwise shifts (width >> 4 or width << 2) on signed integer
variables (such as int width). Compilers automatically optimize division and
multiplication by constant powers of two into efficient instructions while
avoiding signed shift undefined behavior and UBSan diagnostics.

**Why.** Bitwise shifting signed integer types can trigger Undefined Behavior
Sanitizer (UBSan) warnings or undefined/implementation-defined behavior if
values are negative or overflow signed bounds. Using standard division (/) and
multiplication (*) guarantees standard arithmetic semantics, and modern
optimizing compilers emit identical shift or load-effective-address
instructions.

## Examples

Bad:

```cpp
// Bad: Bitwise shifts on signed integer variables can trigger UBSan warnings or UB.
int len = width >> 4;
int half_width = (width + 1) >> 1;
src += (width << 2) - 64;
```

Good:

```cpp
// Good: Arithmetic division and multiplication are safe and optimized by the compiler.
int len = width / 16;
int half_width = (width + 1) / 2;
src += (width * 4) - 64;
```
