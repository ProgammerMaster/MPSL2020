.syntax unified
.cpu cortex-m4
.thumb
.data
infix_expr: .asciz "{ -99+ [ 10 + 20 - 10 }"
user_stack_bottom: .zero 128
.text
.global main
//move infix_expr here. Please refer to the question below.
main:
BL stack_init
LDR R0, =infix_expr
//mov r0, r6
BL pare_check
L: B L
stack_init:
//TODO: Setup the stack pointer(sp) to user_stack.
ldr sp, =user_stack_bottom
add sp, sp, #128
BX LR


pare_check:

ldrb r1, [r0]

cmp r1, #30
blt exit

cmp r1, #80
bge quote
add r0, r0, 1
b pare_check


quote:
cmp r1, #91
beq first_quote1

cmp r1, #93
beq first_quote2

cmp r1, #123
beq second_quote1

cmp r1, #125
beq second_quote2

first_quote1:
push {r1}
add r0, r0, 1
b pare_check

first_quote2:
pop {r7}
sub r7, r1, r7
cmp r7, #2
bne wrong // error
add r0, r0, 1
b pare_check

second_quote1:
push {r1}
add r0, r0, 1
b pare_check


second_quote2:
pop {r7}
sub r7, r1, r7
cmp r7, #2
bne wrong // error
add r0, r0, 1
b pare_check

wrong:
mov r0, #0
sub r0, r0, #1
bx lr


exit:
mov r0, #0
BX LR
