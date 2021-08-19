	.file	"inf06-0.c"
	.intel_syntax noprefix
	.text
	.globl	calc
	.type	calc, @function
calc:
.LFB0:
	.cfi_startproc
	endbr64
	addsd	xmm0, xmm1
	addsd	xmm1, xmm2
	divsd	xmm0, xmm1
	ret
	.cfi_endproc
.LFE0:
	.size	calc, .-calc
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
