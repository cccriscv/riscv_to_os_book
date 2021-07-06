addi x1, x0, 0
sub x2, x1, x4
addi	x5,x5,-48
addi	x5,x5,48
sw	x7,44(x6)
lw	x9,-40(x8)
beq	x4,x5,10
jal 10(x3)
# jr	200(x5)
# ecall	100(x7)
