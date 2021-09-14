.syntax unified
.cpu cortex-m4
.thumb
.data
leds: .byte 0
.text
.global main
.equ RCC_AHB2ENR, 0x4002104C
.equ GPIOA_MODER, 0x48000000
.equ GPIOA_OSPEEDR, 0x48000008
.equ GPIOA_ODR, 0x48000014
.equ GPIOB_MODER, 0x48000400
.equ GPIOB_OTYPER, 0x48000404
.equ GPIOB_OSPEEDR, 0x48000408
.equ GPIOB_PUPDR,  0x4800040C
.equ GPIOB_ODR, 0x48000414
.equ GPIOC_MODER, 0x48000800
.equ GPIOC_IDR, 0x48000810 //to active Pc 13
.equ moder_mask_bit, 0xFFFFFFFC
.equ moder_bit, 0x154
.equ speed_bit, 0x2A8
.equ X, 0x27100
.equ Y, 0x3
.equ Ans, 0x0
main:
bl GPIO_init
loop:
bl check_press
b loop


check_press:
ldr r1, =GPIOC_IDR
ldr r0, [r1]
mov r2, #1
lsl r2, #13
and r0, r2
cmp r0, #0
beq stop//is pressing
bx lr

GPIO_init:
movs r0, #0x6
ldr  r1, =RCC_AHB2ENR
str  r0, [r1]


ldr  r1, =GPIOB_MODER
ldr  r2, [r1]
ldr r3, =moder_mask_bit
and  r2, r3
add r2, r2, #1
str  r2, [r1]
// for Pc13
ldr r1, =GPIOC_MODER
ldr r2, [r1]
ldr r3, =#0xF3FFFF00
and r2, r3
str r2, [r1]

ldr r1, =GPIOB_ODR
mov r2, #0xFF
str r2, [r1]

bx lr

stop:
ldr r7, =X
AA1:
ldr r6, =Y
AA2:
sub r6, r6, #1
cmp r6, #0
bne AA2
sub r7, r7, #1
cmp r7, #0
bne AA1
wait2:
ldr r0, =GPIOC_IDR
ldr r1, [r0]
mov r2, #1
lsl r2, #13
and r1, r1, r2
cmp r1, #0
beq wait2
b stop2
stop2:
ldr r7, =X
AAA1:
ldr r6, =Y//500
AAA2:
sub r6, r6, #1
cmp r6, #0
bne AAA2
sub r7, r7, #1
cmp r7, #0
bne AAA1

roll_in_ans:
ldr r0, =GPIOC_IDR
ldr r1, [r0]
ldr r5, =#0xF
and r1, r5
ldr r2, =Ans
eor r1, r2
cmp r1, #0
beq jump// right answer
bl display
bl Delay
bl display2
b loop

jump:
bl display
bl Delay
bl display2
bl Delay
bl display
bl Delay
bl display2
bl Delay
bl display
bl Delay
bl display2
b loop
display2:
ldr r5, =GPIOB_ODR
mov r3, #1
str r3, [r5]
bx lr
display:
ldr r5, =GPIOB_ODR
ldr r4, [r5]
ldr r2,=#0xFFFFFFFC
and r4,r2
//mov r3, #0
str r4, [r5]
bx lr
Delay:
ldr r7, =X
A1:
ldr r6, =Y
A2:
sub r6, r6, #1
cmp r6, #0
bne A2
sub r7, r7, #1
cmp r7, #0
bne A1
bx lr
