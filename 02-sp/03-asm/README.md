# RISC-V Hello Assembly

* [Writing and Understanding RISC-V Assembly Code](https://bananatronics.org/writing-and-understanding-risc-v-assembly-code/)
* [RISC-V Assembly Language](https://web.eecs.utk.edu/~smarz1/courses/ece356/notes/assembly/)

```
user@DESKTOP-96FRN6B MINGW64 /d/ccc/ccc109/sp/10-riscv/01-asm (master)
$ make
riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -o add.elf add.s
d:/ccc/ccc109/sp_more/riscv/freedomstudio-2020-06-3-win64/sifive/riscv64-unknown-elf-gcc-8.3.0-2020.04.1/bin/../lib/gcc/riscv64-unknown-elf/8.3.0/../../../../riscv64-unknown-elf/bin/ld.exe: warning: cannot find entry symbol _start; defaulting to 0000000000000000
riscv64-unknown-elf-objcopy -O binary add.elf add.bin

user@DESKTOP-96FRN6B MINGW64 /d/ccc/ccc109/sp/10-riscv/01-asm (master)
$ riscv64-unknown-elf-objdump -d add.elf

add.elf:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <main>:
   0:   00500e93                li      t4,5
   4:   02500f13                li      t5,37
   8:   01df0fb3                add     t6,t5,t4
```
