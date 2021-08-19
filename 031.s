	.arch armv5t
	.file	"ams2.c"
	.text
	.align	2
	.global	summ
	.syntax unified
	.arm
	.fpu softvfp
	.type	summ, %function
summ:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r3, #0
.L2:
	cmp	r3, r1
	bxge	lr
	ldr	ip, [r2, r3, lsl #2]
	add	r3, r3, #1
	add	r0, r0, ip
	b	.L2
	.size	summ, .-summ
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",%progbits
