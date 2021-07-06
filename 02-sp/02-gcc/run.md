# RISC-V 的 gcc 工具鏈

```sh
user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02/01-cadd (master)
$ riscv64-unknown-elf-gcc -S add.c -o add.s

user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02/01-cadd (master)
$ riscv64-unknown-elf-gcc -c add.c -o add.elf

user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02/01-cadd (master)
$ riscv64-unknown-elf-objdump -d add.elf

add.elf:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <add>:
   0:   1101                    addi    sp,sp,-32
   2:   ec22                    sd      s0,24(sp)
   4:   1000                    addi    s0,sp,32
   6:   87aa                    mv      a5,a0
   8:   872e                    mv      a4,a1
   a:   fef42623                sw      a5,-20(s0)
   e:   87ba                    mv      a5,a4
  10:   fef42423                sw      a5,-24(s0)
  14:   fec42703                lw      a4,-20(s0)
  18:   fe842783                lw      a5,-24(s0)
  1c:   9fb9                    addw    a5,a5,a4
  1e:   2781                    sext.w  a5,a5
  20:   853e                    mv      a0,a5
  22:   6462                    ld      s0,24(sp)
  24:   6105                    addi    sp,sp,32
  26:   8082                    ret

user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02/01-cadd (master)
$ riscv64-unknown-elf-gcc -c add.s -o add.elf

user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02/01-cadd (master)
$ riscv64-unknown-elf-objdump -d add.elf

add.elf:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <add>:
   0:   1101                    addi    sp,sp,-32
   2:   ec22                    sd      s0,24(sp)
   4:   1000                    addi    s0,sp,32
   6:   87aa                    mv      a5,a0
   8:   872e                    mv      a4,a1
   a:   fef42623                sw      a5,-20(s0)
   e:   87ba                    mv      a5,a4
  10:   fef42423                sw      a5,-24(s0)
  14:   fec42703                lw      a4,-20(s0)
  18:   fe842783                lw      a5,-24(s0)
  1c:   9fb9                    addw    a5,a5,a4
  1e:   2781                    sext.w  a5,a5
  20:   853e                    mv      a0,a5
  22:   6462                    ld      s0,24(sp)
  24:   6105                    addi    sp,sp,32
  26:   8082                    ret
```

