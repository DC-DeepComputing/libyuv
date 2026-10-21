---
name: libyuv/source/accumulate-in-vector-registers-across-reduction-loops
rubric: safety
severity: should_fix
---

**Rule.** In SIMD reduction loops (such as HammingDistance, SumSquareError,
HashDjb2, or SAD), accumulate intermediate results across iterations in SIMD
vector registers. Do not perform intra-vector horizontal additions or extract
vector elements/lanes to scalar registers inside the loop body; defer horizontal
reductions and scalar extraction until after the loop terminates.

**Why.** Horizontal vector additions and SIMD-to-scalar lane extractions inside
loop bodies introduce high instruction latency, cross-domain register transfer
penalties, and execution stalls that serialize execution across iterations.
Accumulating within SIMD vector registers maximizes execution port throughput
and vector parallelism, requiring only a single horizontal reduction and scalar
extraction sequence upon exiting the loop.

## Examples

Bad:

```cpp
// Variant 1: Horizontal addition and scalar copy inside reduction loop
for (i = 0; i < count; i += 32) {
  reg0 = __msa_dotp_s_w(vec0, vec0);
  reg1 = __msa_dotp_s_w(vec1, vec1);
  reg0 += reg1;
  tmp0 = __msa_hadd_s_d(reg0, reg0);
  sse += (uint32)__msa_copy_u_w((v4i32)tmp0, 0);
  sse += (uint32)__msa_copy_u_w((v4i32)tmp0, 2);
}

// Variant 2: Scalar lane extraction inside bit count / Hamming distance loop
for (i = 0; i < count; i += 32) {
  vec0 = __msa_pcnt_d((v2i64)src0);
  vec1 = __msa_pcnt_d((v2i64)src1);
  vec0 += vec1;
  diff += (uint32)__msa_copy_u_w((v4i32)vec0, 0);
  diff += (uint32)__msa_copy_u_w((v4i32)vec0, 2);
}
```

Good:

```cpp
// Variant 1: Accumulate in vector registers and reduce once after loop finishes
v4i32 reg0 = {0}, reg1 = {0};
for (i = 0; i < count; i += 32) {
  reg0 = __msa_dpadd_s_w(reg0, vec0, vec0);
  reg1 = __msa_dpadd_s_w(reg1, vec1, vec1);
}
reg0 += reg1;
tmp0 = __msa_hadd_s_d(reg0, reg0);
sse = (uint32)__msa_copy_u_w((v4i32)tmp0, 0) + (uint32)__msa_copy_u_w((v4i32)tmp0, 2);

// Variant 2: Vector accumulation across loop, deferring scalar extraction
v2i64 vec0 = {0}, vec1 = {0};
for (i = 0; i < count; i += 32) {
  vec0 += __msa_pcnt_d((v2i64)src0);
  vec1 += __msa_pcnt_d((v2i64)src1);
}
vec0 += vec1;
diff = (uint32)__msa_copy_u_w((v4i32)vec0, 0) + (uint32)__msa_copy_u_w((v4i32)vec0, 2);
```
