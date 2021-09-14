.data
arr1: .word 0x19, 0x34, 0x14, 0x32, 0x52, 0x23, 0x61, 0x29
arr2: .word 0x18, 0x17, 0x33, 0x16, 0xFA, 0x20, 0x55, 0xAC
.text
.global main

do_sort:
	mov r1, #0 // first loop int i=-1
	sub r1, r1, #1
	first_loop1:
	cmp r1, #6
	beq exit

	add r1, r1, #1 // let r1 to be 0, int i = 0

	movs r7, #7 // second loop int j=7

	second_loop2:
	ldrb r6, [r0, r7]
	sub r7, r7, #1
	ldrb r5, [r0, r7]
	cmp r6, r5

	bgt change
	//blt change

	cmp r7, r1
	beq first_loop1

	b second_loop2
	change:
	mov r4, r5
	mov r5, r6
	mov r6, r4

	strb r5, [r0, r7]
	add r7, r7, #1 // address
	strb r6, [r0, r7]
	sub r7, r7, #1

	cmp r7, r1
	beq first_loop1

	b second_loop2
	exit:
	bx lr
/*
do_sort:
	mov r3, #4
	mov r1, #0 // first loop int i=-1
	sub r1, r1, #1
	first_loop1:
	cmp r1, #6
	beq exit

	add r1, r1, #1 // let r1 to be 0, int i = 0

	movs r7, #7 // second loop int j=7

	second_loop2:
	mov r2, r7
	mul r7, r7, r3

	ldrb r6, [r0, r7]
	mov r7, r2

	sub r7, r7, #1
	mov r2, r7
	mul r7, r7, r3
	ldrb r5, [r0, r7]
	mov r7, r2

	cmp r6, r5
	bhi change

	cmp r7, r1
	beq first_loop1

	b second_loop2
	change:
	mov r4, r5
	mov r5, r6
	mov r6, r4

	mov r2, r7
	mul r7, r7, r3

	strb r5, [r0, r7]

	mov r7, r2

	add r7, r7, #1 // address
	mov r2, r7
	mul r7, r7, r3

	strb r6, [r0, r7]
	mov r7, r2

	sub r7, r7, #1

	cmp r7, r1
	beq first_loop1

	b second_loop2
	exit:
	bx lr
*/
main:

ldr r0, =arr1
bl do_sort
ldr r0, =arr2
bl do_sort

L: b L
