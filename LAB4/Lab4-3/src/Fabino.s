.syntax unified
.cpu cortex-m4
.thumb
.data
    ans1: .word 0x0
    ans2: .word 0x1
.text
    .global main
    .equ RCC_AHB2ENR, 0x4002104C
    .equ GPIOA_BASE, 0x48000000
    .equ GPIO_BSRR_OFFSET, 0x18
    .equ GPIO_BRR_OFFSET, 0x28
    .equ GPIOA_MODER, 0x48000000
    .equ GPIOA_OTYPER, 0x48000004
    .equ GPIOA_OSPEEDR, 0x48000008
    .equ GPIOA_PUPDR, 0x4800000C
    .equ GPIOA_ODR, 0x48000014
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_IDR, 0x48000810 //to active Pc 13
    .equ DECODE_MODE,   0x09
    .equ SHUTDOWN,  0x0C
    .equ INTENSITY, 0x0A
    .equ SCAN_LIMIT, 0x0B
    .equ DISPLAY_TEST, 0x0F
    .equ DATA, 0x20 //PA5 00100000
    .equ LOAD, 0x40 //PA6 01000000
    .equ CLK, 0x80 //PA7 10000000
    .equ X, 0x3E8 // 1000(DEC)
    .equ Y, 0x3E8 // 1000(DEC)
main:
    BL GPIO_init
    BL max7219_init

    ldr r2, =#0
    B Display0toF
GPIO_init:
    //TODO: Initialize three GPIO pins as output for max7219 DIN, CS
    // enable AHB2 clock
    movs r0, #0x5
    ldr r1, =RCC_AHB2ENR
    str r0, [r1]
    // set PA5, PA6, PA7 as output mode
    movs r0, #0x5400
    ldr r1, =GPIOA_MODER
    ldr r2, [r1]
    and r2, #0xFFFF03FF
    orrs r2,r2,r0
    str r2,[r1]
    // set PA5 as high speed mode
    movs r0, #0xA800
    ldr r1, =GPIOA_OSPEEDR
    str r0,[r1]

	// for Pc13
	ldr r1, =GPIOC_MODER
	ldr r2, [r1]
	ldr r3, =#0xF3FFFF00
	and r2, r3
	str r2, [r1]

    ldr r1, =GPIOA_ODR
    BX LR

max7219_init:
    //TODO: Initialize max7219 registers
    push {r0, r1, r2, lr}
    ldr r0, =#DECODE_MODE
    ldr r1, =#0xFF
    BL MAX7219Send
    ldr r0, =#DISPLAY_TEST
    ldr r1, =#0x0
    BL MAX7219Send
    ldr r0, =#SCAN_LIMIT
    ldr r1, =0x7
    BL MAX7219Send
    ldr r0, =#INTENSITY
    ldr r1, =#0xA
    BL MAX7219Send
    ldr r0, =#SHUTDOWN
    ldr r1, =#0x1
    BL MAX7219Send
    pop {r0, r1, r2, pc}
    BX LR
Display0toF:
    //TODO: Display 0 to F at first digit on 7-SEG LED. Display one per second.
	push {lr}
	bl check_press
	pop {lr}
    mov r0, #1
    ldr r7, =ans1
    ldr r1, [r7]
    ldr r2, [r7]
    continue:

    // divide 10
    mov r9,#10
    udiv r1, r9
    cmp r1, #0
    beq blank


    mul r1, r9
    sub r1, r2, r1
    BL MAX7219Send
    udiv r2, r9
    mov r1, r2
    add r0, r0, #1
    cmp r0,#9
    beq Display0toF
    b continue

blank:
//mul r1, r9
//sub r1, r2, r1
mov r1, r2
push {lr}
BL MAX7219Send
pop {lr}
add r0, r0, #1
cmp r0,#9
beq Display0toF
blank2:
mov r1,#0xF
push {lr}
BL MAX7219Send
pop {lr}
add r0, r0, #1
cmp r0,#9
beq Display0toF
b blank2
MAX7219Send:
    //input parameter: r0 is ADDRESS , r1 is DATA
    //TODO: Use this function to send a message to max7219
    push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, lr}
    lsl r0, r0, #8
    add r0, r0, r1
    ldr r1, =#GPIOA_BASE
    ldr r2, =#LOAD
    ldr r3, =#DATA
    ldr r4, =#CLK
    ldr r5, =#GPIO_BSRR_OFFSET
    ldr r6, =#GPIO_BRR_OFFSET
    mov r7, #16 //r7 = i
    //BX LR
.max7219send_loop:
    mov r8, #1
    sub r9, r7, #1
    lsl r8, r8, r9 // r8 = mask
    str r4, [r1,r6]//HAL_GPIO_WritePin(GPIOA, CLOCK, 0);
    tst r0, r8
    beq .bit_not_set//bit not set
    str r3, [r1,r5]
    b .if_done
.bit_not_set:
    str r3, [r1,r6]
.if_done:
    str r4, [r1,r5]
subs r7, r7, #1
    bgt .max7219send_loop
    str r2, [r1,r6]
    str r2, [r1,r5]
    pop {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, lr}
    BX LR

check_press:
	ldr r1, =GPIOC_IDR
	ldr r0, [r1]
	mov r2, #1
	lsl r2, #13
	and r0, r2
	cmp r0, #0
	beq stop//is pressing
	bx lr
stop:
mov r7, #500
AA1:
mov r6, #500
AA2:
sub r6, r6, #1
cmp r6, #0
bne AA2
sub r7, r7, #1
cmp r7, #0
bne AA1
mov r6,#0
wait2:
ldr r0, =GPIOC_IDR
ldr r1, [r0]
mov r2, #1
lsl r2, #13
and r1, r1, r2
cmp r1, r2
beq stop2
add r6,r6,#1
ldr r7,=#0x249F0
cmp r6,r7
bne wait2
ldr r0,=ans1
mov r1,#0
str r1,[r0]
ldr r3,=ans2
mov r2,#1
str r2,[r3]
b wait3
wait3:
ldr r0, =GPIOC_IDR
ldr r1, [r0]
mov r2, #1
lsl r2, #13
and r1, r1, r2
cmp r1, #1
bge stop3
b wait3
stop2:
mov r7, #1 //500
AAA1:
mov r6, #1 //500
AAA2:
sub r6, r6, #1
cmp r6, #0
bne AAA2
sub r7, r7, #1
cmp r7, #0
bne AAA1
b fib
stop3:
mov r7, #1 //500
AAAA1:
mov r6, #1 //500
AAAA2:
sub r6, r6, #1
cmp r6, #0
bne AAAA2
sub r7, r7, #1
cmp r7, #0
bne AAAA1
b Display0toF

// display


fib:
ldr r0, =ans1
ldr r1, [r0]

ldr r3, =ans2
ldr r2, [r3]
ldr r5, =#0x5F5E0FF
cmp r2,r5
beq overflow
add r4, r1, r2
cmp r4,r5
bgt overflow
str r2, [r0]
str r4, [r3]
b Display0toF
overflow:
str r2, [r0]
str r5,[r3]
b Display0toF



