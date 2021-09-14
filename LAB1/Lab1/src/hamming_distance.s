.data
result: .byte 0
.text
.global main
.equ X, 0xABCD
.equ Y, 0xEFAB

main:
ldr R0, =X //This line will cause an error. Why?
ldr R1, =Y
ldr R2, =result
bl hamm
L: b L


hamm:

ldr r3, [r2]
movs r4, r0
movs r5, r1
movs r6, #1
movs r0, #1 // put a count

eor r4, r5

	loop1:

	movs r7, r4
	and r7, r6
	cmp r7, #0
	bne count_one

	loop2:
	lsr r4, #1

	cmp r4, #0
	beq exit

	//add r0, #1
	b loop1

	exit:
	str r3, [r2]
	bx lr



	count_one:
	add r3, #1
	b loop2
bx lr
