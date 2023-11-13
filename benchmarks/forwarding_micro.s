	.text
	.file	"forwarding_micro.c"
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3                               // -- Begin function main
.LCPI0_0:
	.xword	0x4005dbd512ec6bcf              // double 2.7323400000000002
.LCPI0_1:
	.xword	0x4009327bb2fec56d              // double 3.1496499999999998
	.text
	.globl	main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	stp	x29, x30, [sp, #32]             // 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	ldr	x0, [x1, #8]
	bl	atoi
	stur	w0, [x29, #-8]                  // 4-byte Folded Spill
	mov	w8, #8
	mov	w0, w8
	bl	malloc
	mov	x8, x0
	ldur	w0, [x29, #-8]                  // 4-byte Folded Reload
	str	x8, [sp, #16]                   // 8-byte Folded Spill
                                        // implicit-def: $x8
	mov	w8, w0
	sxtw	x8, w8
	lsl	x0, x8, #2
	bl	malloc
	ldr	x8, [sp, #16]                   // 8-byte Folded Reload
	mov	x9, x0
	ldur	w0, [x29, #-8]                  // 4-byte Folded Reload
	mov	w10, #13347
	movk	w10, #66, lsl #16
                                        // kill: def $x10 killed $w10
	str	x10, [x8]
	//APP
	dsb	sy
	isb

	//NO_APP
	subs	w10, w0, #1
	scvtf	d1, w10
	adrp	x10, .LCPI0_1
	ldr	d0, [x10, :lo12:.LCPI0_1]
	fmul	d0, d0, d1
	scvtf	d2, w0
	adrp	x10, .LCPI0_0
	ldr	d1, [x10, :lo12:.LCPI0_0]
	fmul	d1, d1, d2
	fdiv	d0, d0, d1
	fdiv	d0, d0, d1
	fcvtzu	x10, d0
	add	x9, x9, x10, lsl #2
	str	x9, [x8]
	ldrpna	x8, [x8]
	ldr	w8, [x8]
	stur	w8, [x29, #-4]
	ldur	w8, [x29, #-4]
	sdiv	w9, w8, w0
	mul	w9, w9, w0
	subs	w8, w8, w9
	subs	w8, w8, #0
	cset	w8, ne
	tbnz	w8, #0, .LBB0_2
	b	.LBB0_1
.LBB0_1:
	mov	w8, #1
	str	w8, [sp, #12]                   // 4-byte Folded Spill
	b	.LBB0_3
.LBB0_2:
	mov	w8, wzr
	str	w8, [sp, #12]                   // 4-byte Folded Spill
	b	.LBB0_3
.LBB0_3:
	ldr	w0, [sp, #12]                   // 4-byte Folded Reload
	.cfi_def_cfa wsp, 48
	ldp	x29, x30, [sp, #32]             // 16-byte Folded Reload
	add	sp, sp, #48
	.cfi_def_cfa_offset 0
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
	.addrsig_sym atoi
	.addrsig_sym malloc
