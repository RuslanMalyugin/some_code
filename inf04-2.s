  .text
  .global main
main:
  push {r4, r5, r6, ip, lr}
  ldr r4, =stdin
  ldr r5, =stdout
.loop:
  ldr r0, [r4]
  bl fgetc
  cmn r0, #1
  beq .exit
  mov r6, #'9'
  add r6, r6, #1
  cmp r0, r6
  bpl .loop
  cmp r0, #'0'
  bmi .loop
  b .out
.out:
  ldr r1, [r5]
  bl fputc
  b .loop
.exit:
  pop {r4, r5, r6, ip, pc}