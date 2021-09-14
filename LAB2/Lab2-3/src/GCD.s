.data
result: .word 0
max_size: .word 0
.text
m: .word 0x4E
n: .word 0x82
GCD:
//TODO: Implement your GCD function
cmp r0,#0//m==0
beq mzero
cmp r1,#0//n==0
beq nzero
mov r4,r0
lsr r4,#1
lsl r4,#1
cmp r4,r0 //m%2==0
beq m_mod2zero
mov r4,r0
lsr r4,#1
lsl r4,#1
cmp r4,r0
beq n_mod2zero //n%2==0
sub r6,r0,r1
cmp r6,#0
bgt m_greater
sub r6,r1,r0
mov r1,r0 // n = m
mov r0,r6 // m = n - m
push {lr}
bl GCD
mrs r4,msp
sub r4,r2,r4
cmp r4,r3
bgt update_max
pop {r7}
mov lr,r7
BX LR
m_greater:
mov r0,r6 // m = m - n
push {lr}
bl GCD
pop {r7}
mov lr,r7
BX LR
m_mod2zero:
mov r4,r1
lsr r4,#1
lsl r4,#1
cmp r4,r1
beq mn_mod2zero // n%2==0
lsr r0,#1
push {lr}
bl GCD
mrs r4,msp
sub r4,r2,r4
cmp r4,r3
bgt update_max
pop {r7}
mov lr,r7
BX LR
n_mod2zero:
lsr r1,#1
push {lr}
bl GCD

mrs r4,msp
sub r4,r2,r4
cmp r4,r3
bgt update_max
pop {r7}
mov lr,r7
BX LR
mn_mod2zero:
lsr r0,#1
lsr r1,#1
push {lr}
bl GCD
lsl r5,#1 //2*GCD(m,n)
mrs r4,msp
sub r4,r2,r4
cmp r4,r3
bgt update_max
pop {r7}
mov lr,r7
BX LR
update_max:
movs r3,r4
pop {r7}
mov lr,r7
BX LR
mzero:
mov r5,r1
BX LR
nzero:
mov r5,r0
BX LR
.global main
main:
// r0 = m, r1 = n
ldr r3,=m
ldr r4,=n
ldr r0,[r3]
ldr r1,[r4]
mrs r2,msp
mov r3,#0 // maxsize
BL GCD
ldr r2,=max_size
str r3,[r2]
ldr r2,=result
str r5,[r2]
L: b L
// get return val and store into result
