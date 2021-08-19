	.file	"c_inf05-0.c"
	.intel_syntax noprefix
	.text
	.globl	summ
	.type	summ, @function
summ:
.LFB13:
	.cfi_startproc
	endbr64
	mov	rcx, QWORD PTR A[rip]
	mov	rsi, QWORD PTR B[rip]
	xor	eax, eax
	mov	rdi, QWORD PTR R[rip]
.L2:
	cmp	DWORD PTR N[rip], eax
	jle	.L5
	mov	edx, DWORD PTR [rsi+rax*4]
	add	edx, DWORD PTR [rcx+rax*4]
	mov	DWORD PTR [rdi+rax*4], edx
	inc	rax
	jmp	.L2
.L5:
	ret
	.cfi_endproc
.LFE13:
	.size	summ, .-summ
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%d"
	.text
	.globl	everyday795
	.type	everyday795, @function
everyday795:
.LFB14:
	push	rbp
	mov	ebp, esi
	push	rbx
	mov	ebx, edi
	lea	rdi, .LC0[rip]
	sub	rsp, 24
	mov	rax, QWORD PTR fs:40
	mov	QWORD PTR 8[rsp], rax
	xor	eax, eax
	lea	rsi, 4[rsp]
	call	scanf
	imul	ebx, DWORD PTR 4[rsp]
	xor	eax, eax
	lea	rsi, .LC0[rip]
	mov	edi, 1
	lea	edx, [rbx+rbp]
	mov	DWORD PTR 4[rsp], edx
	call	printf
	mov	rax, QWORD PTR 8[rsp]
	xor	rax, QWORD PTR fs:40
	add	rsp, 24
	pop	rbx
	pop	rbp
	ret
.L7:
	add	rsp, 24
	pop	rbx
	pop	rbp
	ret
.LFE14:
	.size	everyday795, .-everyday795
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
