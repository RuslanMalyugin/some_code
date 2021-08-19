	.file	"inf07-1.c"
	.intel_syntax noprefix
	.text
	.globl	_start
	.type	_start, @function
_start:
.LFB0:
	.cfi_startproc
	endbr64
	push	rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	sub	rsp, 16
	.cfi_def_cfa_offset 32
	mov	rax, QWORD PTR fs:40
	mov	QWORD PTR 8[rsp], rax
	xor	eax, eax
	lea	rbx, 7[rsp]
.L2:
	xor	esi, esi
	xor	edi, edi
	xor	eax, eax
	mov	ecx, 1
	mov	rdx, rbx
	call	syscall@PLT
	test	rax, rax
	je	.L7
	mov	ecx, 1
	mov	rdx, rbx
	mov	esi, 1
	xor	eax, eax
	mov	edi, 1
	call	syscall@PLT
	jmp	.L2
.L7:
	xor	esi, esi
	mov	edi, 60
	call	syscall@PLT
	mov	rax, QWORD PTR 8[rsp]
	xor	rax, QWORD PTR fs:40
	je	.L4
	call	__stack_chk_fail@PLT
.L4:
	add	rsp, 16
	.cfi_def_cfa_offset 16
	pop	rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE0:
	.size	_start, .-_start
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
