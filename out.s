.section .text
.global main

main:
    pushq %rbp
    movq %rsp, %rbp
    movl $70, %eax
    pushq %rax
    movl $80, %eax
    pushq %rax
    movl -16(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
