  .intel_syntax noprefix
  .text
  .global calc
  .global vector_sum

calc:
  addsd	xmm0, xmm1
  pxor xmm1, xmm1
  cvtsi2sd xmm1, edi
  addsd	xmm1, xmm2
  divsd	xmm0, xmm1
  ret

vector_sum:
  push rbx
  mov rbx, 0
  sal rdi, 2
.loop:
  movaps	xmm0, XMMWORD PTR [rsi + rbx]
  addps xmm0, XMMWORD PTR [rdx + rbx]
  movaps XMMWORD PTR [rcx + rbx], xmm0
  add rbx, 16
  cmp rbx, rdi
  jne .loop
  pop rbx
  ret
