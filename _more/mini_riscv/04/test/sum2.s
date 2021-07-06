# x1:sum, x2:i
addi x1, x0, 0  # 00: sum = 0
addi x2, x0, 1  # 04: i = 1
addi x3, x0, 11 # 08: x3 = 11
# loop:
add  x1, x1, x2 # 12: sum = sum + i
addi x2, x2, 1  # 16: i = i + 1
blt  x2, x3, 12 # 20: if (i<=10) goto loop
addi x1, x1, 0  # 24: show x1, should be 55
