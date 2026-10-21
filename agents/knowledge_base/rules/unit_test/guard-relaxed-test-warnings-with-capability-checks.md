---
name: libyuv/unit_test/guard-relaxed-test-warnings-with-capability-checks
rubric: correctness
severity: should_fix
---

**Rule.** When relaxing test assertions (such as ASSERT_EQ or EXPECT_EQ) into
non-fatal diagnostic warnings or GTEST_SKIP() to accommodate emulators or
variable runtime environments (e.g., host /proc/cpuinfo mismatches), guard the
warning message with the specific capability check (such as ArmCpuCaps() or
RiscvCpuCaps()) instead of logging unconditionally. Ensure the test emits a
warning only when the expected feature flag (such as kCpuHasNEON or kCpuHasRVV)
is actually absent from the queried environment, and document why the mismatch
can occur in comments.

**Why.** Unconditionally logging a warning when relaxing a test assertion
produces false-positive warnings on standard hardware where the feature is fully
supported and present. Guarding the warning with an explicit capability check
preserves test diagnostics in emulated environments without polluting clean test
runs on real hardware.

## Examples

Bad:

```cpp
// BAD: Logs warning unconditionally whenever /proc/cpuinfo exists,
// even when the CPU has NEON support.
if (FileExists("/proc/cpuinfo")) {
  printf("WARNING: Neon build enabled but CPU does not have Neon\n");
}
```

Good:

```cpp
// GOOD: Verify capability is actually missing before emitting warning.
if (FileExists("/proc/cpuinfo")) {
  if (kCpuHasNEON != ArmCpuCaps("/proc/cpuinfo")) {
    // This can happen on Arm emulator when /proc/cpuinfo is from the host.
    printf("WARNING: Neon build enabled but CPU does not have Neon\n");
  }
}
```
