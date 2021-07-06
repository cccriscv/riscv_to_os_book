	.file	"hanoi.c"
	.option nopic
	.text
	.align	2
	.globl	Hanoi
	.type	Hanoi, @function
Hanoi:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	a2,-44(s0)
	sw	a3,-48(s0)
	sw	zero,-20(s0)
	lw	a4,-36(s0)
	li	a5,1
	bne	a4,a5,.L2
	li	a5,1
	sw	a5,-20(s0)
	j	.L3
.L2:
	lw	a5,-36(s0)
	addi	a5,a5,-1
	lw	a3,-44(s0)
	lw	a2,-48(s0)
	lw	a1,-40(s0)
	mv	a0,a5
	call	Hanoi
	mv	a4,a0
	lw	a5,-20(s0)
	add	a5,a5,a4
	sw	a5,-20(s0)
	lw	a3,-48(s0)
	lw	a2,-44(s0)
	lw	a1,-40(s0)
	li	a0,1
	call	Hanoi
	mv	a4,a0
	lw	a5,-20(s0)
	add	a5,a5,a4
	sw	a5,-20(s0)
	lw	a5,-36(s0)
	addi	a5,a5,-1
	lw	a3,-48(s0)
	lw	a2,-40(s0)
	lw	a1,-44(s0)
	mv	a0,a5
	call	Hanoi
	mv	a4,a0
	lw	a5,-20(s0)
	add	a5,a5,a4
	sw	a5,-20(s0)
.L3:
	lw	a5,-20(s0)
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	Hanoi, .-Hanoi
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	li	a5,1
	sw	a5,-20(s0)
	li	a5,2
	sw	a5,-24(s0)
	li	a5,3
	sw	a5,-28(s0)
	li	a5,4
	sw	a5,-32(s0)
	lw	a3,-28(s0)
	lw	a2,-24(s0)
	lw	a1,-20(s0)
	lw	a0,-32(s0)
	call	Hanoi
	sw	a0,-36(s0)
	lw	a5,-36(s0)
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 7.2.0"
