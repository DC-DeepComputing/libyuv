---
name: libyuv/measure-string-length-before-strtok
rubric: correctness
severity: should_fix
---

**Rule.** When tokenizing a string buffer in place using strtok, strtok_r, or
strsep, measure the total string length with strlen() (or duplicate the buffer)
before invoking the tokenizer. Because in-place tokenizers overwrite delimiter
characters with null terminators ('\0'), subsequent strlen() calls on the buffer
will only return the length of the first token rather than the original string.

**Why.** strtok and strtok_r mutate the input buffer in place by writing null
terminators at delimiter boundaries. Calling strlen() after tokenization has
started measures only the first token, leading to incorrect string length
calculations and downstream parsing errors.

## Examples

Bad:

```cpp
// Bad: strtok_r mutates `extensions` in place, so strlen returns only the first token's length.
char* saveptr = nullptr;
char* ext = strtok_r(extensions, "_", &saveptr);
size_t extensions_len = strlen(extensions);
```

Good:

```cpp
// Good: Measure strlen before strtok_r modifies delimiter characters to null terminators.
size_t extensions_len = strlen(extensions);
char* saveptr = nullptr;
char* ext = strtok_r(extensions, "_", &saveptr);
```
