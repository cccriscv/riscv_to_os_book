# x1:sum, x2:i
addi x1, x0, 0 # sum = 0
addi x2, x0, 1 # i = 1
addi x3, x0, 10 # x3 = 10
# loop: (3)
add  x1, x1, x2 # sum = sum + i
addi x2, x2, 1 # i = i + 1

