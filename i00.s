	.arch armv5t
	.file	"00.c"
	.text
	.align	2
	.global	calculate
	.syntax unified
	.arm
	.fpu softvfp
	.type	calculate, %function
calculate:
	ldr	r1, .L2+4
	ldr	r2, [r3]
	ldr	r3, .L2+8
	ldr	r1, [r1]
	ldr	r0, [r3]
	ldr	r3, .L2+12
	ldr	r3, [r3]
	mul	r3, r1, r3
	mla	r0, r2, r0, r3
	bx	lr
.L3:
	.align	2
.L2:
	.word	A
	.word	D
	.word	B
	.word	C
	.size	calculate, .-calculate
	.comm	D,4,4
	.comm	C,4,4
	.comm	B,4,4
	.comm	A,4,4
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",%progbits
