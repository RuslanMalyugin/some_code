  .text
  .global solve

solve:
  push {r4, r5}
  mov r4, r0
  mov r5, #0

.loop:
  mla r4, r5, r4, r1
  mla r4, r5, r4, r2
  mla r4, r5, r4, r3 @ try with current x
  cmp r4, #0 @ check for solition
  mov r4, r0
  beq .exit @ if x is solition - exit
  add r5, r5, #1
  cmp r5, #255
  beq .exit_not_found @ if we tried all x from (0, 256) and didnt find
  bne .loop @ go loop

.exit:
  mov r0, r5
  pop {r4, r5}
  bx lr

.exit_not_found:
  mov r0, #0
  pop {r4, r5}
  bx lr