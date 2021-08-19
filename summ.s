	.arch armv5t
	.file	"inf04-1.c"
	.text
	.section	.rodata.str1.1,"aMS",%progbits,1
.LC0:
	.ascii	"%d\000"
	.section	.text.startup,"ax",%progbits
	.align	2
	.global	main
	.syntax unified
	.arm
	.fpu softvfp
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	mov	r2, #1
	push	{r4, lr}
	mov	r0, r2
	ldr	r1, .L3
	bl	__printf_chk
	mov	r0, #0
	pop	{r4, pc}
.L4:
	.align	2
.L3:
	.word	.LC0
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",%progbits
