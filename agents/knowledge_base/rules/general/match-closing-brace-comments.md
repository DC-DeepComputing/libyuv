---
name: libyuv/match-closing-brace-comments
rubric: readability
severity: blocking
---

**Rule.** Ensure comments on closing braces accurately match the specific block
being terminated. For nested `namespace libyuv { extern "C" { ... } }` blocks,
label the inner closing brace `} // extern "C"` and the outer closing brace `}
// namespace libyuv`.

**Why.** Closing brace comments clarify which scope is being terminated at the
end of long files or nested blocks; duplicate or mismatched labels mislead
readers about whether an `extern "C"` linkage block or namespace is still open.

## Examples

Bad:

```cpp
#ifdef __cplusplus
}  // namespace libyuv
}  // namespace libyuv
#endif
```

Good:

```cpp
#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
```
