---
name: libyuv/omit-redundant-stream-flush-before-exit
description: When returning from main() or exiting normally, do not call fflush(stdout); standard streams flush automatically on process exit.
rubric: readability
severity: nit
---

**Rule.** Do not call fflush(stdout), fflush(NULL), or equivalent stream flush
methods immediately before returning from main() or invoking exit(), as the C
and C++ runtime automatically flushes all open output streams on normal process
termination. Reserve explicit flushes for situations where output must be forced
before long-running operations, inter-process communication, or operations that
risk abnormal termination.

**Why.** The C and C++ standards guarantee that all open buffered output streams
are flushed and closed upon normal process exit or return from main().
Explicitly flushing standard output at the exit point adds redundant boilerplate
that duplicates runtime cleanup.

## Examples

Bad:

```cpp
printf("cpuid exiting\n");
fflush(stdout);
return 0;
```

Good:

```cpp
printf("cpuid exiting\n");
return 0;
```
