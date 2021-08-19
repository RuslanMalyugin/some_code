  .intel_syntax noprefix
  .text
  .global summ
summ:
  push rbx
  mov rax, 0
.start_loop:   
  cmp rdi, rax
  jle .return_rax
  mov	rbx, QWORD PTR [rsi+rax*4]
  add	rbx, QWORD PTR [rdx+rax*4]
  mov	QWORD PTR [rcx+rax*4], rbx
  inc rax
  jmp .start_loop

.return_rax:
  pop rbx
  ret
