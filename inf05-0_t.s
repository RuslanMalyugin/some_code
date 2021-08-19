  .intel_syntax noprefix
  .text
  .global summ
  .global everyday795
summ:
  mov	rcx, QWORD PTR A[rip]
  mov	rsi, QWORD PTR B[rip]
  mov	rdi, QWORD PTR R[rip]
  mov rax, 0
.start_loop:   
  cmp QWORD PTR N[rip],rax
  jle .return_rax
  mov	rdx, QWORD PTR [rsi+rax*4]
  add	rdx, QWORD PTR [rcx+rax*4]
  mov	QWORD PTR [rdi+rax*4], rdx
  inc rax
  jmp .start_loop

.return_rax:
  ret


everyday795:
  push rbp
  push rbx
  mov	rbp, rsi
  mov	rbx, rdi
  lea	rdi,  int_format[rip]
  sub	rsp, 24
  lea rsi, input_num[rip]
  mov rax, 0
  call scanf
  mov rsi, input_num[rip]
  imul	rsi, rbx
  add rsi, rbp
  lea	rdi, int_format[rip]
  mov rax, 0
  call printf
  add rsp, 24
  pop	rbx
  pop	rbp

  .data
int_format:
  .string	"%d"

input_num:
  .long 0
