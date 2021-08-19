    .intel_syntax noprefix
    .global syscall
    .text
syscall:

    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    push r11
    push rcx

    push r9
    push r8
    push rcx
    push rdx
    push rsi
    push rdi

    pop rax

    pop rdi
    pop rsi
    pop rdx
    pop r10
    pop r8

    syscall

    pop rcx
    pop r11
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    ret
    