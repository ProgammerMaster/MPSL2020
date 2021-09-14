.syntax unified
.cpu cortex-m4
.thumb
.data
password: .byte 0xC
X: .long 250
Y: .long 1000
temp: .long 0xF
led: .long 0x78

.text
.global main
.equ RCC_AHB2ENR, 0x4002104C
.equ GPIOB_MODER, 0x48000400
.equ GPIOB_OTYPER, 0x48000404
.equ GPIOB_OSPEEDR, 0x48000408
.equ GPIOB_PUPDR, 0x4800040C
.equ GPIOB_ODR, 0x48000414
.equ GPIOC_MODER, 0x48000800
.equ GPIOC_IDR, 0x48000810
main:
BL GPIO_init
wait:
ldr r3, =X
ldr r4, =Y
movs r5,#0
//lsl r5,#13
ldr r0, =GPIOC_IDR
ldr r1, [r0]
lsr r1, #13
cmp r1,r5
beq Stop2
b wait
Stop2:
ldr r6,[r3]
L11111:
ldr r7,[r4]
L22222:
subs r7, #1
bne L22222
subs r6, #1
bne L11111
pass:
ldr r0,=temp
ldr r1,[r0]
ldr r0,=password
ldr r2,[r0]

ldr r0,=GPIOC_IDR
ldr r3,[r0]
ands r2,r1
ands r3,r1
cmp r2,r3
beq Yes
No:
bl Display1
bl Delay
bl Display2
b wait
Yes:
bl Display1
bl Delay
bl Display2
bl Delay
bl Display1
bl Delay
bl Display2
bl Delay
bl Display1
bl Delay
bl Display2
b wait
Display1:
ldr r1,=GPIOB_ODR
movs r0,#0
str r0,[r1]
bx lr
Display2:
ldr r1,=GPIOB_ODR
ldr r0,=led
ldr r8,[r0]
//movs r0,#(1<<5)
str r8,[r1]
bx lr
GPIO_init:
//TODO: Initial LED GPIO pins as output
movs r0, #0x6
ldr r1, =RCC_AHB2ENR
str r0, [r1]
movs r0, #0x1540
ldr r1, =GPIOB_MODER
ldr r2, [r1]
and r2, #0xFFFFC03F//3,4,5,6
orrs r2, r2, r0
str r2, [r1]
//-----------
ldr r1, =GPIOC_MODER
ldr r0,[r1]
ldr r2, =#0xF3FFFF00
and r0,r2
str r0,[r1]
//---------------
movs r0, #0x2A80
ldr r1, =GPIOB_OSPEEDR
str r0, [r1]
ldr r1,=GPIOB_ODR
ldr r0,=led
BX LR
Delay:
ldr r3, =X
ldr r4, =Y
ldr r6, [r3]
L1:
ldr r7, [r4]
L2:
subs r7, #1
bne L2
subs r6, #1
bne L1
BX LR
