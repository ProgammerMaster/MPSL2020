.syntax unified
.cpu cortex-m4
.thumb
.data
leds: .byte 0

.text
.global main
.equ RCC_AHB2ENR, 0x4002104C
.equ GPIOB_MODER, 0x48000400
.equ GPIOB_OTYPER, 0x48000404
.equ GPIOB_OSPEEDR, 0x48000408
.equ GPIOB_PUPDR,  0x4800040C
.equ GPIOB_ODR, 0x48000414

.equ moder_mask_bit, 0xFFFFFC03
.equ moder_bit, 0x00000154
.equ speed_bit, 0x2A8
.equ X, 0x27100
.equ Y,0x3
//.equ T, 0xFFFFFFFF

main:
bl GPIO_init
mov r5, #1
mov r4, #0 // this is for special flag

ldr r6, =leds
ldr r3, [r6]
add r3, r3, #2


loop:
bl DisplayLED // PB 1 2 3 4
bl Delay

cmp r4, #1
bne jump
cmp r3, #6
beq init

cmp r3, #2
bne jump
mov r4, #0
mov r3, #6
b loop

init:
mov r3, #2
b loop


jump:

cmp r3, #24
beq left1

cmp r3, #16
beq left2

cmp r3, #2
beq add_one

cmp r4, #1
beq right_shift
left_shift:
lsl r3, #1
b loop

right_shift:
lsr r3, #1
b loop

add_one:
mov r3, #6
b loop

left1:
cmp r4, #1
beq right_shift

mov r3, #16
mov r4, #1
b loop

left2:
mov r3, #24
b loop


GPIO_init:
movs r0, #0x2
ldr  r1, =RCC_AHB2ENR
str  r0, [r1]

ldr r1, =#0xFFF
ldr r0, =GPIOB_ODR
str r1, [r0]

ldr r0, =moder_bit
ldr  r1, =GPIOB_MODER
ldr  r2, [r1]
ldr r3, =moder_mask_bit
and  r2, r3
eor r2, r0
str  r2, [r1]

ldr r0, =speed_bit
ldr r1, =GPIOB_OSPEEDR
strh r0, [r1]



ldr r0, =GPIOB_ODR
bx lr

DisplayLED:
ldr r0, =leds
lsr r3, #1
str r3, [r0]
lsl r3, #1

eor r3, -1
ldr r0, =GPIOB_ODR
strh r3, [r0]
eor r3, -1


bx lr





Delay:
ldr r7, =X
A1:
ldr r6, =Y
A2:
sub r6, r6, r5
cmp r6, #0
bne A2
sub r7, r7, r5
cmp r7, #0
bne A1
bx lr




