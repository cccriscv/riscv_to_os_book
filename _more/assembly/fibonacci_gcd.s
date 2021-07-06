	.file	"fibonacci_gcd.c"
	.option nopic
	.text
	.align	2
	.globl	gcd
	.type	gcd, @function
gcd:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	lw	a4,-36(s0)
	lw	a5,-40(s0)
	bne	a4,a5,.L2
	lw	a5,-36(s0)
	sw	a5,-20(s0)
	j	.L3
.L2:
	lw	a4,-36(s0)
	lw	a5,-40(s0)
	ble	a4,a5,.L4
	lw	a4,-36(s0)
	lw	a5,-40(s0)
	sub	a5,a4,a5
	sw	a5,-36(s0)
	j	.L5
.L4:
	lw	a4,-40(s0)
	lw	a5,-36(s0)
	sub	a5,a4,a5
	sw	a5,-40(s0)
.L5:
	lw	a1,-40(s0)
	lw	a0,-36(s0)
	call	gcd
	sw	a0,-20(s0)
.L3:
	lw	a5,-20(s0)
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	gcd, .-gcd
	.align	2
	.globl	fib
	.type	fib, @function
fib:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	sw	s1,20(sp)
	addi	s0,sp,32
	sw	a0,-20(s0)
	lw	a4,-20(s0)
	li	a5,1
	bgt	a4,a5,.L8
	lw	a5,-20(s0)
	j	.L9
.L8:
	lw	a5,-20(s0)
	addi	a5,a5,-1
	mv	a0,a5
	call	fib
	mv	s1,a0
	lw	a5,-20(s0)
	addi	a5,a5,-2
	mv	a0,a5
	call	fib
	mv	a5,a0
	add	a5,s1,a5
.L9:
	mv	a0,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	lw	s1,20(sp)
	addi	sp,sp,32
	jr	ra
	.size	fib, .-fib
	.align	2
	.globl	hThread1
	.type	hThread1, @function
hThread1:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	lw	a1,-40(s0)
	lw	a0,-36(s0)
	call	gcd
	sw	a0,-20(s0)
	lw	a5,-20(s0)
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	hThread1, .-hThread1
	.align	2
	.globl	hThread2
	.type	hThread2, @function
hThread2:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	lw	a0,-36(s0)
	call	fib
	sw	a0,-20(s0)
	lw	a5,-20(s0)
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	hThread2, .-hThread2
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	li	a5,1989
	sw	a5,-20(s0)
	li	a5,867
	sw	a5,-24(s0)
	li	a5,18
	sw	a5,-28(s0)
	lw	a1,-24(s0)
	lw	a0,-20(s0)
	call	hThread1
	sw	a0,-32(s0)
	lw	a0,-28(s0)
	call	hThread2
	sw	a0,-36(s0)
	li	a5,0
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 7.2.0"
