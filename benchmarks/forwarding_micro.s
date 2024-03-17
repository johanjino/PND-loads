	.text
	.file	"forwarding_micro.c"
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3                               // -- Begin function main
.LCPI0_0:
	.xword	0x4009327bb2fec56d              // double 3.1496499999999998
.LCPI0_1:
	.xword	0x4005dbd512ec6bcf              // double 2.7323400000000002
	.text
	.globl	main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:
	stp	x29, x30, [sp, #-32]!           // 16-byte Folded Spill
	.cfi_def_cfa_offset 32
	str	x19, [sp, #16]                  // 8-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 32
	.cfi_offset w19, -16
	.cfi_offset w30, -24
	.cfi_offset w29, -32
	ldr	x0, [x1, #8]
	mov	x1, xzr
	mov	w2, #10
	bl	strtol
	mov	x19, x0
	sbfiz	x0, x0, #2, #32
	bl	malloc
	sub	w8, w19, #1
	adrp	x9, .LCPI0_0
	adrp	x10, .LCPI0_1
	scvtf	d2, w19
	//APP
	dsb	sy
	isb

	//NO_APP
	scvtf	d0, w8
	ldr	d1, [x9, :lo12:.LCPI0_0]
	ldr	d3, [x10, :lo12:.LCPI0_1]
	fmul	d0, d0, d1
	fmul	d1, d2, d3
	fdiv	d0, d0, d1
	fdiv	d0, d0, d1
	fcvtzu	x8, d0
	ldr	w8, [x0, x8, lsl #2]
	str	w8, [x29, #28]
	ldr	w8, [x29, #28]
	sdiv	w9, w8, w19
	msub	w8, w9, w19, w8
	cmp	w8, #0
	cset	w0, eq
	.cfi_def_cfa wsp, 32
	ldr	x19, [sp, #16]                  // 8-byte Folded Reload
	ldp	x29, x30, [sp], #32             // 16-byte Folded Reload
	.cfi_def_cfa_offset 0
	.cfi_restore w19
	.cfi_restore w30
	.cfi_restore w29
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        // -- End function
	.ident	"clang version 16.0.0"
	.section	".note.GNU-stack","",@progbits
	.addrsig
