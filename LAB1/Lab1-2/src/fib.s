.text
.global main
.equ N, 49

main:
//movs r0, #N
ldr r0, =N
bl fib
L: b L

fib:
cmp r0, #100
bgt exit1

cmp r0, #0
beq exit2
blt exit1


cmp r0, #1
beq exit3

movs r1, #2 // set a counter
movs r4, #0 // initialization
movs r2, #0
movs r3, #1

	loop1:
	add r4, r2, r3
	bvs over_flow
	movs r2, r3
	movs r3, r4

	cmp r1, r0
	beq exit

	add r1, r1, #1
	b loop1

	exit:
	bx lr
	exit1:
	movs r4, #0
	sub r4, r4, #1
	bx lr
	exit2:
	movs r4, #0
	bx lr
	exit3:
	movs r4, #1
	bx lr
	over_flow:
	movs r4, #0
	sub r4, r4, #2
	bx lr


