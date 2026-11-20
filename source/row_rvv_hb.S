# row_rvv_hb.S - Handwritten RVV 1.0 kernels for SpacemiT K3 (VLEN=256)
# libyuv HB iter4 FINAL: reverse/fill/split/merge (m2, stable) + averaging.
# NOTE: iter5 m4 upgrade of split/merge REGRESSED (EMUL=NF*LMUL group trap),
#       reverted to m2. Assemble with -march=rv64gcv.

    .text

# MirrorRow_HB(src=a0,dst=a1,width=a2) dst[i]=src[w-1-i]
    .globl MirrorRow_HB
MirrorRow_HB:
    beqz  a2, 9f
    add   t1, a0, a2
    mv    t3, a1
    vsetvli t0, zero, e8, m2, ta, ma
    vid.v v6
    li    t6, 63
    vrsub.vx v6, v6, t6
    vsetvli t0, zero, e8, m1, ta, ma
    vid.v v8
    li    t6, 31
    vrsub.vx v8, v8, t6
1:  addi  t4, a2, -64
    bltz  t4, 2f
    addi  t2, t1, -64
    vsetvli t0, zero, e8, m2, ta, ma
    vle8.v v2, (t2)
    vrgather.vv v4, v2, v6
    vse8.v v4, (t3)
    addi  t3, t3, 64
    mv    t1, t2
    mv    a2, t4
    j     1b
2:  addi  t4, a2, -32
    bltz  t4, 3f
    addi  t2, t1, -32
    vsetvli t0, zero, e8, m1, ta, ma
    vle8.v v2, (t2)
    vrgather.vv v4, v2, v8
    vse8.v v4, (t3)
    addi  t3, t3, 32
    mv    t1, t2
    mv    a2, t4
    j     2b
3:  beqz  a2, 9f
    vsetvli t0, a2, e8, m1, ta, ma
    sub   t2, t1, t0
    vle8.v v2, (t2)
    vid.v v5
    addi  t6, t0, -1
    vrsub.vx v5, v5, t6
    vrgather.vv v4, v2, v5
    vse8.v v4, (t3)
9:  ret

# MirrorUVRow_HB(src=a0,dst=a1,width=a2) width=(u,v) pairs
    .globl MirrorUVRow_HB
MirrorUVRow_HB:
    beqz  a2, 9f
    slli  t2, a2, 1
    add   t1, a0, t2
    mv    t3, a1
    vsetvli t0, zero, e16, m2, ta, ma
    vid.v v6
    li    t6, 31
    vrsub.vx v6, v6, t6
    vsetvli t0, zero, e16, m1, ta, ma
    vid.v v8
    li    t6, 15
    vrsub.vx v8, v8, t6
1:  addi  t4, a2, -32
    bltz  t4, 2f
    addi  t2, t1, -64
    vsetvli t0, zero, e16, m2, ta, ma
    vle16.v v2, (t2)
    vrgather.vv v4, v2, v6
    vse16.v v4, (t3)
    addi  t3, t3, 64
    mv    t1, t2
    mv    a2, t4
    j     1b
2:  addi  t4, a2, -16
    bltz  t4, 3f
    addi  t2, t1, -32
    vsetvli t0, zero, e16, m1, ta, ma
    vle16.v v2, (t2)
    vrgather.vv v4, v2, v8
    vse16.v v4, (t3)
    addi  t3, t3, 32
    mv    t1, t2
    mv    a2, t4
    j     2b
3:  beqz  a2, 9f
    vsetvli t0, a2, e16, m1, ta, ma
    slli  t5, t0, 1
    sub   t2, t1, t5
    vle16.v v2, (t2)
    vid.v v6
    addi  t6, t0, -1
    vrsub.vx v6, v6, t6
    vrgather.vv v4, v2, v6
    vse16.v v4, (t3)
9:  ret

# SetRow_HB(dst=a0,v8=a1,width=a2) - e8m4 stays fastest on K3
    .globl SetRow_HB
SetRow_HB:
    beqz  a2, 3f
    vsetvli t0, zero, e8, m4, ta, ma
    vmv.v.x v8, a1
1:  vsetvli t0, a2, e8, m4, ta, ma
    vse8.v v8, (a0)
    add   a0, a0, t0
    sub   a2, a2, t0
    bnez  a2, 1b
3:  ret

# ARGBSetRow_HB(dst=a0,v32=a1,width=a2 pixels)
    .globl ARGBSetRow_HB
ARGBSetRow_HB:
    beqz  a2, 3f
    slli  t3, a2, 2
    vsetvli t0, zero, e32, m2, ta, ma
    vmv.v.x v8, a1
    vsetvli t0, zero, e8, m8, ta, ma
1:  vsetvli t0, t3, e8, m8, ta, ma
    vse8.v v8, (a0)
    add   a0, a0, t0
    sub   t3, t3, t0
    bnez  t3, 1b
3:  ret

# split/merge family - e8m2 (m4 regroup regresses, see header)
    .globl SplitARGBRow_HB
SplitARGBRow_HB:
    beqz  a5, 3f
1:  vsetvli t0, a5, e8, m2, ta, ma
    vlseg4e8.v v8, (a0)
    vse8.v v8, (a3)
    vse8.v v10, (a2)
    vse8.v v12, (a1)
    vse8.v v14, (a4)
    slli  t1, t0, 2
    add   a0, a0, t1
    add   a1, a1, t0
    add   a2, a2, t0
    add   a3, a3, t0
    add   a4, a4, t0
    sub   a5, a5, t0
    bnez  a5, 1b
3:  ret

    .globl SplitRGBRow_HB
SplitRGBRow_HB:
    beqz  a4, 3f
1:  vsetvli t0, a4, e8, m2, ta, ma
    vlseg3e8.v v8, (a0)
    vse8.v v8, (a1)
    vse8.v v10, (a2)
    vse8.v v12, (a3)
    add   t1, t0, t0
    add   t1, t1, t0
    add   a0, a0, t1
    add   a1, a1, t0
    add   a2, a2, t0
    add   a3, a3, t0
    sub   a4, a4, t0
    bnez  a4, 1b
3:  ret

    .globl SplitUVRow_HB
SplitUVRow_HB:
    beqz  a3, 3f
1:  vsetvli t0, a3, e8, m2, ta, ma
    vlseg2e8.v v8, (a0)
    vse8.v v8, (a1)
    vse8.v v10, (a2)
    slli  t1, t0, 1
    add   a0, a0, t1
    add   a1, a1, t0
    add   a2, a2, t0
    sub   a3, a3, t0
    bnez  a3, 1b
3:  ret

    .globl MergeARGBRow_HB
MergeARGBRow_HB:
    beqz  a5, 3f
1:  vsetvli t0, a5, e8, m2, ta, ma
    vle8.v v8, (a2)
    vle8.v v10, (a1)
    vle8.v v12, (a0)
    vle8.v v14, (a3)
    vsseg4e8.v v8, (a4)
    add   a0, a0, t0
    add   a1, a1, t0
    add   a2, a2, t0
    add   a3, a3, t0
    slli  t1, t0, 2
    add   a4, a4, t1
    sub   a5, a5, t0
    bnez  a5, 1b
3:  ret

    .globl MergeRGBRow_HB
MergeRGBRow_HB:
    beqz  a4, 3f
1:  vsetvli t0, a4, e8, m2, ta, ma
    vle8.v v8, (a0)
    vle8.v v10, (a1)
    vle8.v v12, (a2)
    vsseg3e8.v v8, (a3)
    add   a0, a0, t0
    add   a1, a1, t0
    add   a2, a2, t0
    slli  t1, t0, 1
    add   t1, t1, t0
    add   a3, a3, t1
    sub   a4, a4, t0
    bnez  a4, 1b
3:  ret

    .globl MergeXRGBRow_HB
MergeXRGBRow_HB:
    beqz  a4, 3f
    vsetvli t0, zero, e8, m2, ta, ma
    li    t2, 255
    vmv.v.x v14, t2
1:  vsetvli t0, a4, e8, m2, ta, ma
    vle8.v v8, (a2)
    vle8.v v10, (a1)
    vle8.v v12, (a0)
    vsseg4e8.v v8, (a3)
    add   a0, a0, t0
    add   a1, a1, t0
    add   a2, a2, t0
    slli  t1, t0, 2
    add   a3, a3, t1
    sub   a4, a4, t0
    bnez  a4, 1b
3:  ret

# ---- iteration 4 averaging (continuous load + vnsrl split) ----
    .globl HalfWidthRow_HB
HalfWidthRow_HB:
    beqz  a2, 9f
    csrwi vxrm, 0
1:
    vsetvli t0, a2, e8, m2, ta, ma
    slli    t1, t0, 1
    vsetvli zero, t1, e8, m4, ta, ma
    vle8.v  v8, (a0)
    vsetvli zero, t0, e8, m2, ta, ma
    vnsrl.wi v16, v8, 0
    vnsrl.wi v18, v8, 8
    vaaddu.vv v16, v16, v18
    vse8.v  v16, (a1)
    add     a0, a0, t1
    add     a1, a1, t0
    sub     a2, a2, t0
    bnez    a2, 1b
9:  ret

    .globl HalfRow_HB
HalfRow_HB:
    beqz  a3, 9f
    csrwi vxrm, 0
    add   t1, a0, a1
1:
    vsetvli t0, a3, e8, m4, ta, ma
    vle8.v  v8, (a0)
    vle8.v  v12, (t1)
    vaaddu.vv v8, v8, v12
    vse8.v  v8, (a2)
    sub   a3, a3, t0
    add   a0, a0, t0
    add   t1, t1, t0
    add   a2, a2, t0
    bgtz  a3, 1b
9:  ret

    .globl HalfWidthUVRow_HB
HalfWidthUVRow_HB:
    beqz  a2, 9f
    csrwi vxrm, 0
1:
    vsetvli t0, a2, e8, m1, ta, ma
    slli    t1, t0, 2
    vsetvli zero, t1, e8, m4, ta, ma
    vle8.v  v8, (a0)
    vsetvli zero, t0, e16, m2, ta, ma
    vnsrl.wi v16, v8, 0
    vnsrl.wi v18, v8, 16
    slli    t2, t0, 1
    vsetvli zero, t2, e8, m2, ta, ma
    vaaddu.vv v16, v16, v18
    vse8.v  v16, (a1)
    add     a0, a0, t1
    add     a1, a1, t2
    sub     a2, a2, t0
    bnez    a2, 1b
9:  ret

    .globl HalfUVRow_HB
HalfUVRow_HB:
    beqz  a5, 9f
    csrwi vxrm, 0
    add   t1, a0, a1
    add   t2, a2, a3
1:
    vsetvli t0, a5, e8, m4, ta, ma
    vle8.v  v8, (a0)
    vle8.v  v16, (t1)
    vaaddu.vv v8, v8, v16
    vle8.v  v12, (a2)
    vle8.v  v20, (t2)
    vaaddu.vv v12, v12, v20
    vsseg2e8.v v8, (a4)
    sub   a5, a5, t0
    add   a0, a0, t0
    add   t1, t1, t0
    add   a2, a2, t0
    add   t2, t2, t0
    slli  t3, t0, 1
    add   a4, a4, t3
    bgtz  a5, 1b
9:  ret

    .globl HalfMergeUVRow_HB
HalfMergeUVRow_HB:
    beqz  a5, 9f
    csrwi vxrm, 0
    srli  a5, a5, 1
    add   t2, a0, a1
    add   t3, a2, a3
    li    t6, 2
1:
    vsetvli t0, a5, e8, m4, ta, ma
    slli    t1, t0, 1
    vsetvli zero, t1, e8, m8, ta, ma
    vle8.v  v8, (a0)
    vle8.v  v16, (t2)
    vsetvli zero, t0, e8, m4, ta, ma
    vnsrl.wi v0, v8, 0
    vnsrl.wi v4, v8, 8
    vwaddu.vx v8, v0, t6
    vwaddu.wv v8, v8, v4
    vnsrl.wi v0, v16, 0
    vnsrl.wi v4, v16, 8
    vwaddu.wv v8, v8, v0
    vwaddu.wv v8, v8, v4
    vnsrl.wi v24, v8, 2
    vsetvli zero, t1, e8, m8, ta, ma
    vle8.v  v8, (a2)
    vle8.v  v16, (t3)
    vsetvli zero, t0, e8, m4, ta, ma
    vnsrl.wi v0, v8, 0
    vnsrl.wi v4, v8, 8
    vwaddu.vx v8, v0, t6
    vwaddu.wv v8, v8, v4
    vnsrl.wi v0, v16, 0
    vnsrl.wi v4, v16, 8
    vwaddu.wv v8, v8, v0
    vwaddu.wv v8, v8, v4
    vnsrl.wi v28, v8, 2
    vsseg2e8.v v24, (a4)
    add   a0, a0, t1
    add   t2, t2, t1
    add   a2, a2, t1
    add   t3, t3, t1
    slli  t5, t0, 1
    add   a4, a4, t5
    sub   a5, a5, t0
    bnez  a5, 1b
9:  ret

    .globl ScaleRowDown2Box_HB
ScaleRowDown2Box_HB:
    beqz  a3, 9f
    csrwi vxrm, 0
    add   t4, a0, a1
    li    t6, 2
1:
    vsetvli t0, a3, e8, m2, ta, ma
    vle16.v v8, (a0)
    vle16.v v12, (t4)
    vnsrl.wi v16, v8, 0
    vnsrl.wi v18, v8, 8
    vnsrl.wi v20, v12, 0
    vnsrl.wi v22, v12, 8
    vwaddu.vx v8, v16, t6
    vwaddu.wv v8, v8, v18
    vwaddu.wv v8, v8, v20
    vwaddu.wv v8, v8, v22
    vnsrl.wi v16, v8, 2
    vse8.v  v16, (a2)
    sub   a3, a3, t0
    slli  t1, t0, 1
    add   a0, a0, t1
    add   t4, t4, t1
    add   a2, a2, t0
    bgtz  a3, 1b
9:  ret
