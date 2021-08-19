  #include <sys/syscall.h>
  .intel_syntax noprefix
	.text
	.global	_start

_start:
  endbr64
  push rax
  push rdx
  push rdi
  lea rsi, BYTE PTR char[rip] 
  mov rdx, 1

.cycle:
  mov rax, SYS_read
  mov rdi, 0
  syscall
  cmp rax, 0
  jz .end_of_cycle
  mov rax, SYS_write
  mov rdi, 1
  syscall
  jmp .cycle

.end_of_cycle:
  mov rax, SYS_exit
  syscall
  pop rdi
  pop rdx
  pop rax

  .data
char: .byte 0 