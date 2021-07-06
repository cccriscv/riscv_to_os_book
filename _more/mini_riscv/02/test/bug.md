# 

root@localhost:~/co/riscv/mini_riscv/01/asm# riscv64-unknown-elf-objcopy -O binary sum1 sum1.bin
root@localhost:~/co/riscv/mini_riscv/01/asm# ls
add  add1  add1.bin  add1.s  sum1  sum1.bin  sum1.s
root@localhost:~/co/riscv/mini_riscv/01/asm# cd ..
root@localhost:~/co/riscv/mini_riscv/01# ./dasm asm/sum1.bin
asm                  addr:code     T op f3 f7 rd rs1 rs2
---------------------------------------------------------
0000:addi x1,x0,0         00000093 I 13  0 00 01  00  00
0004:addi x2,x0,1         00100113 I 13  0 00 02  00  01
0008:addi x3,x0,11        00B00193 I 13  0 00 03  00  0B
000C:Instruction error !  0105908A ? 0A  1 00 01  0B  10
0010:blt x2,x3,-4         FE314EE3 B 63  4 7F 1D  02  03
root@localhost:~/co/riscv/mini_riscv/01# vim asm/sum1.s
root@localhost:~/co/riscv/mini_riscv/01# objdump -d asm/sum1

asm/sum1:     file format elf64-little

objdump: can't disassemble for architecture UNKNOWN!

root@localhost:~/co/riscv/mini_riscv/01# riscv64-unknown-elf-objdump -d asm/sum1

asm/sum1:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <main>:
   0:   00000093                li      ra,0
   4:   00100113                li      sp,1
   8:   00b00193                li      gp,11

000000000000000c <.loop>:
   c:   908a                    add     ra,ra,sp
   e:   0105                    addi    sp,sp,1
  10:   fe314ee3                blt     sp,gp,c <.loop>
root@localhost:~/co/riscv/mini_riscv/01# riscv32-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -o sum1 sum1.s     