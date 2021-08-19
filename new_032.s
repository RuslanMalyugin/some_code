  .text
  .global solve
solve:
  push {r4, r5}
  mov r4, r0
  mov r5, #0
.L3:
  mla r4, r5, r4, r1
  mla r4, r5, r4, r2
  mla r4, r5, r4, r3
  cmp r4, #0
  mov r4, r0
  beq .L4
  add r5, r5, #1
  cmp r5, #255
  beq .L5
  bne .L3
.L4:
  mov r0, r5
  pop {r4, r5}
  bx lr
.L5:
  mov r0, #0
  pop {r4, r5}
  bx lr
