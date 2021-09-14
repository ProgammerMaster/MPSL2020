.syntax unified
.cpu cortex-m4
.thumb
.data
result: .zero 8
.text
.global main
.equ X, 0x12
.equ Y, 0x8765
.equ Z,0xFFFF
.equ B,0xFFFFFFFF
main:
LDR R0, =X
LDR R1, =Y
LDR R2, =result
LDR R7, =Z
BL kara_mul
L:
B L
kara_mul:
add r3, r0,#0
lsr r3,#16 //leftmost X
add r4, r0,#0
and r4, r4, r7 //rightmost X
add r5, r1,#0
lsr r5,#16 //leftmost Y
add r6, r1,#0
and r6, r6, r7 //rightmost Y
mul r0,r3,r5//XLYL
mul r1,r4,r6//XRYR
add r3,r3,r4//XL+XR
add r4,r5,r6//YL+YR
push {r0}//XLYL
push {r1}//XRYR
///////////////////////////////////////
//mul r3,r3,r4//(XL+XR)(YL+YR)
mov r0,r3//(XL+XR)
mov r1,r4//(YL+YR)
add r3, r0,#0
lsr r3,#16 //leftmost X
add r4, r0,#0
and r4, r4, r7 //rightmost X
add r5, r1,#0
lsr r5,#16 //leftmost Y
add r6, r1,#0
and r6, r6, r7 //rightmost Y
mul r0,r3,r5//XLYL
mul r1,r4,r6//XRYR
add r3,r3,r4//XL+XR
add r4,r5,r6//YL+YR
mul r3,r3,r4
sub r3,r3,r0//(XL+XR)(YL+YR)-XLYL
sub r3,r3,r1//(XL+XR)(YL+YR)-XLYL-XRYR
and r9,r3,r7//(XL+XR)(YL+YR)-XLYL-XRYR 's rightmost
lsl r9,r9,#16 //shift 16bit
lsr r3,#16//(XL+XR)(YL+YR)-XLYL-XRYR 's leftmost
adds r9,r9,r1
adc r8,r0,r3
///////////////////////////////////////
pop {r1}//XRYR
pop {r0}//XLYL
//push {r9}//(XL+XR)(YL+YR) right
//push {r8}//(XL+XR)(YL+YR) left
mov r3,r9
mov r4,r8
adds r5,r0,r1
mov r6,#0
adc r6,r6,#0
sub r4,r4,r6 //(XL+XR)(YL+YR)-XLYL-XRYR
cmp r3,r5
beq eq
bls borrow
b eq
eq:
lsl r4,r4,#16
sub r3,r3,r5 //(XL+XR)(YL+YR)-XLYL-XRYR
and r9,r3,r7//(XL+XR)(YL+YR)-XLYL-XRYR 's rightmost
lsl r9,r9,#16 //shift 16bit
lsr r3,#16//(XL+XR)(YL+YR)-XLYL-XRYR 's leftmost
adds r9,r9,r1
adc r8,r0,r3
add r8,r8,r4
//
//leftmost result:r8
//rightmost result:r9
strd r8,r9,[r2]
//TODO: Separate the leftmost and rightmost halves into
//different registers; then do the Karatsuba algorithm.
BX LR
borrow:
sub r4,r4,#1
sub r3,r3,r5 //(XL+XR)(YL+YR)-XLYL-XRYR
ldr r5,=B
add r3,r3,r5
add r3,r3,#1
lsl r4,r4,#16
and r9,r3,r7//(XL+XR)(YL+YR)-XLYL-XRYR 's rightmost
lsl r9,r9,#16 //shift 16bit
lsr r3,#16//(XL+XR)(YL+YR)-XLYL-XRYR 's leftmost
adds r9,r9,r1
adc r8,r0,r3
add r8,r8,r4
strd r8,r9,[r2]
BX LR
