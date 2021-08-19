  .intel_syntax noprefix
  .text
  .global my_sin

my_sin:
  movsd xmm5, xmm0
  mov rax, 0
  mov rcx, -1
  cvtsi2sd xmm4, rcx 
  cvtsi2sd xmm3, rax
  inc rax
  movsd xmm1, xmm0

.summ:
  mulsd xmm1, xmm4
  mulsd xmm1, xmm5
  mulsd xmm1, xmm5
  add rax, 1
  cvtsi2sd xmm2, rax
  divsd xmm1, xmm2
  add rax, 1
  cvtsi2sd xmm2, rax
  divsd xmm1, xmm2
  addsd xmm0, xmm1
  comisd xmm3, xmm1
  jz .end
  jmp .summ

.end:
  ret
