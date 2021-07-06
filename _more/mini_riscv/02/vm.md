# 自製 RISC-V 工具鏈

## 執行

```
PS D:\ccc\ccc109\co\riscv\mini_riscv\v0.6> make
g++ -std=c++11 asm.cpp riscv.cpp -o asm
g++ -std=c++11 vm.cpp riscv.cpp -o vm
PS D:\ccc\ccc109\co\riscv\mini_riscv\v0.6> ./asm s/sum1  
============= PASS1 ================
// # x1:sum, x2:i
// addi x1, x0, 0  # 00: sum = 0
// addi x2, x0, 1  # 04: i = 1
// addi x3, x0, 11 # 08: x3 = 11
// # loop:
// add  x1, x1, x2 # 12: sum = sum + i
// addi x2, x2, 1  # 16: i = i + 1
// blt  x2, x3, -8 # 20: if (i<=10) goto loop
// addi x1, x1, 0  # 24: show x1, should be 55
============= PASS2 ================
// # x1:sum, x2:i
// addi x1, x0, 0  # 00: sum = 0
0000:00000093 I 13 0 00 r:01 00 00 imm:0
0004:00100113 I 13 0 00 r:02 00 00 imm:1
// addi x3, x0, 11 # 08: x3 = 11
0008:00B00193 I 13 0 00 r:03 00 00 imm:11
// # loop:
// add  x1, x1, x2 # 12: sum = sum + i
000C:002080B3 R 33 0 00 r:01 01 02 imm:-1
// addi x2, x2, 1  # 16: i = i + 1
0010:00110113 I 13 0 00 r:02 02 00 imm:1
// blt  x2, x3, -8 # 20: if (i<=10) goto loop
0014:FE314CE3 B 63 4 00 r:00 02 03 imm:-8
// addi x1, x1, 0  # 24: show x1, should be 55
0018:00008093 I 13 0 00 r:01 01 00 imm:0
PS D:\ccc\ccc109\co\riscv\mini_riscv\v0.6> ./vm s/sum1.bin
addi x1,x0,0         0000:00000093 I 13  0 00 01  00  00
R[1]=0
PC=4 mTop=28
addi x2,x0,1         0004:00100113 I 13  0 00 02  00  01
R[2]=1
PC=8 mTop=28
addi x3,x0,11        0008:00B00193 I 13  0 00 03  00  0B
R[3]=11
PC=12 mTop=28
add x1,x1,x2         000C:002080B3 R 33  0 00 01  01  02
R[1]=1
PC=16 mTop=28
addi x2,x2,1         0010:00110113 I 13  0 00 02  02  01
R[2]=2
PC=20 mTop=28
blt x2,x3,-8         0014:FE314CE3 B 63  4 7F 19  02  03
R[25]=0
PC=12 mTop=28
add x1,x1,x2         000C:002080B3 R 33  0 00 01  01  02
R[1]=3
PC=16 mTop=28
addi x2,x2,1         0010:00110113 I 13  0 00 02  02  01
R[2]=3
PC=20 mTop=28
blt x2,x3,-8         0014:FE314CE3 B 63  4 7F 19  02  03
R[25]=0
PC=12 mTop=28
add x1,x1,x2         000C:002080B3 R 33  0 00 01  01  02
R[1]=6
PC=16 mTop=28
addi x2,x2,1         0010:00110113 I 13  0 00 02  02  01
R[2]=4
PC=20 mTop=28
blt x2,x3,-8         0014:FE314CE3 B 63  4 7F 19  02  03
R[25]=0
PC=12 mTop=28
add x1,x1,x2         000C:002080B3 R 33  0 00 01  01  02
R[1]=10
PC=16 mTop=28
addi x2,x2,1         0010:00110113 I 13  0 00 02  02  01
R[2]=5
PC=20 mTop=28
blt x2,x3,-8         0014:FE314CE3 B 63  4 7F 19  02  03
R[25]=0
PC=12 mTop=28
add x1,x1,x2         000C:002080B3 R 33  0 00 01  01  02
R[1]=15
PC=16 mTop=28
addi x2,x2,1         0010:00110113 I 13  0 00 02  02  01
R[2]=6
PC=20 mTop=28
blt x2,x3,-8         0014:FE314CE3 B 63  4 7F 19  02  03
R[25]=0
PC=12 mTop=28
add x1,x1,x2         000C:002080B3 R 33  0 00 01  01  02
R[1]=21
PC=16 mTop=28
addi x2,x2,1         0010:00110113 I 13  0 00 02  02  01
R[2]=7
PC=20 mTop=28
blt x2,x3,-8         0014:FE314CE3 B 63  4 7F 19  02  03
R[25]=0
PC=12 mTop=28
add x1,x1,x2         000C:002080B3 R 33  0 00 01  01  02
R[1]=28
PC=16 mTop=28
addi x2,x2,1         0010:00110113 I 13  0 00 02  02  01
R[2]=8
PC=20 mTop=28
blt x2,x3,-8         0014:FE314CE3 B 63  4 7F 19  02  03
R[25]=0
PC=12 mTop=28
add x1,x1,x2         000C:002080B3 R 33  0 00 01  01  02
R[1]=36
PC=16 mTop=28
addi x2,x2,1         0010:00110113 I 13  0 00 02  02  01
R[2]=9
PC=20 mTop=28
blt x2,x3,-8         0014:FE314CE3 B 63  4 7F 19  02  03
R[25]=0
PC=12 mTop=28
add x1,x1,x2         000C:002080B3 R 33  0 00 01  01  02
R[1]=45
PC=16 mTop=28
addi x2,x2,1         0010:00110113 I 13  0 00 02  02  01
R[2]=10
PC=20 mTop=28
blt x2,x3,-8         0014:FE314CE3 B 63  4 7F 19  02  03
R[25]=0
PC=12 mTop=28
add x1,x1,x2         000C:002080B3 R 33  0 00 01  01  02
R[1]=55
PC=16 mTop=28
addi x2,x2,1         0010:00110113 I 13  0 00 02  02  01
R[2]=11
PC=20 mTop=28
blt x2,x3,-8         0014:FE314CE3 B 63  4 7F 19  02  03
R[25]=0
PC=24 mTop=28
addi x1,x1,0         0018:00008093 I 13  0 00 01  01  00
R[1]=55
PC=28 mTop=28
```

## 搭配 gcc

若想用 vm 執行官方 riscv64-unknown-elf-gcc 產生的碼，可以用

```
$ riscv64-unknown-elf-gcc -S fib.c
$ riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -o fib fib.s
$ riscv64-unknown-elf-objcopy -O binary fib fib.bin
```

