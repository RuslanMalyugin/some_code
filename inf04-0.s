  .text
  .global A
  .global B 
  .global C 
  .global D
  .global calculate

calculate:
  ldr r1, =A
  ldr r1, [r1]
  ldr r2, =B
  ldr r2, [r2]
  ldr r3, =C
  ldr r3, [r3]
  ldr r4, =D
  ldr r4, [r4]
.comm R, 4,4 
  ldr r5, =R
  mov r0, #0
  mul r0, r1, r2
  mla r0, r3, r4, r0
  str r0, [r5]
  bx lr