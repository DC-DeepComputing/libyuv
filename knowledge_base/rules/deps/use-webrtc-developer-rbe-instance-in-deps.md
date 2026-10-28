---
name: libyuv/deps/use-webrtc-developer-rbe-instance-in-deps
description: When configuring the default Remote Build Execution (RBE) instance for developer builds in DEPS, use the WebRTC developer instance rather than untrusted trybot instances
rubric: correctness
severity: should_fix
---

**Rule.** When configuring the default Remote Build Execution (RBE) instance
variable (rbe_instance) for developer builds in DEPS, set it to the WebRTC
developer instance ('projects/rbe-webrtc-developer/instances/default_instance')
rather than trybot or CI instances ('rbe-webrtc-untrusted' or
'rbe-webrtc-trusted').

**Why.** libyuv shares RBE infrastructure with WebRTC, and developers
authenticating for local remote builds have access permissions for the developer
pool ('rbe-webrtc-developer'), whereas untrusted or trusted pools are restricted
to automated trybots and continuous integration builders.

## Examples

Bad:

```cpp
# DEPS
vars = {
  'rbe_instance': Str('projects/rbe-webrtc-untrusted/instances/default_instance'),
}
```

Good:

```cpp
# DEPS
vars = {
  'rbe_instance': Str('projects/rbe-webrtc-developer/instances/default_instance'),
}
```
