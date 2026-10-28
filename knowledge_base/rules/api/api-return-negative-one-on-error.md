---
name: libyuv/api/api-return-negative-one-on-error
description: In libyuv functions returning an integer status code, return -1 for errors or unsupported parameters and 0 on success, rather than positive integers
rubric: readability
severity: should_fix
---

**Rule.** In libyuv functions that return an integer status code, return 0 on
success and -1 to signal error, invalid arguments, or unsupported parameters. Do
not return positive non-zero integers (such as 1) to indicate errors or
unsupported options.

**Why.** libyuv consistently adheres to the convention where 0 denotes success
and -1 denotes failure across all status-returning functions. Returning
arbitrary positive numbers is non-standard and can cause callers expecting
negative error values to mishandle failures.

## Examples

Bad:

```cpp
int ConvertWithFilter(const uint8_t* src, uint8_t* dst, int width, int height,
                      FilterMode filter) {
  switch (filter) {
    case kFilterNone:
      return CopyPlane(src, width, dst, width, width, height);
    case kFilterLinear:
      return 1;  // BAD: Returning positive integer for unsupported mode
    default:
      return -1;
  }
}
```

Good:

```cpp
int ConvertWithFilter(const uint8_t* src, uint8_t* dst, int width, int height,
                      FilterMode filter) {
  switch (filter) {
    case kFilterNone:
      return CopyPlane(src, width, dst, width, width, height);
    case kFilterLinear:
      return -1;  // GOOD: Return -1 for unsupported parameter
    default:
      return -1;
  }
}
```
