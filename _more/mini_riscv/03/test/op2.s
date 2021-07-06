add x1, x2, x3
sub x2, x1, x4
addi	x5,x5,-48
addi	x5,x5,48
sw	x7,44(x6)
lw	x9,-40(x8)
beq	x4,x5,10
jal 10(x3)
sll x1, x2, x3
srl x1, x2, x3
sra x1, x2, x3
add x1, x2, x3
sub x1, x2, x3
xor x1, x2, x3
or x1, x2, x3
and x1, x2, x3
slt x1, x2, x3
sltu x1, x2, x3
slli x1, x2, 3
srli x1, x2, 3
srai x1, x2, 3
addi x1, x2, 3
xori x1, x2, 3
ori x1, x2, 3
andi x1, x2, 3
slti x1, x2, 3
sltiu x1, x2, 3
lui x1, 2
auipc x1, 2
beq x1, 2(x3)
bne x1, 2(x3)
blt x1, 2(x3)
bge x1, 2(x3)
bltu x1, 2(x3)
bgeu x1, 2(x3)
jalr x1, 2(x3)
fence
fence.i
ecall
ebreak
csrrw x1, mtvec, x3
csrrs x1, mepc, x3
csrrc x1, mcause, x3
csrrwi x1, mie, 3
csrrsi x1, mip, 3
csrrci x1, mtval, 3
csrrw x1, mtscratch, x3
csrrs x1, mstatus, x3
csrrc x1, mideleg, x3
csrrwi x1, satp, 3
lb x1, 2(x3)
lh x1, 2(x3)
lw x1, 2(x3)
lbu x1, 2(x3)
lhu x1, 2(x3)
sb x1, 2(x3)
sh x1, 2(x3)
sw x1, 2(x3)
