  .text
  .global main
main:
  push {r0, r1, r2, ip, lr}
  @ исправил на более человеческий вид
  sub sp, sp, #8
  mov r1, sp
  mov r2, #0
  add r2, sp, #4
  ldr r0, =input_int
  bl scanf
  ldr r1, [sp]
  ldr r2, [sp, #4]
  add sp, sp, #8
  add r1, r2, r1
  ldr r0, =output_int
  bl printf
  pop {r0, r1, r2, ip, pc}
input_int:
  .asciz "%d %d"
output_int:
  .asciz "%d"