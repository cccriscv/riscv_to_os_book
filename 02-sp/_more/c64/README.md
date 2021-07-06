# RISCV - C

```
$ make
riscv64-unknown-elf-gcc -S fib.c
riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -o fib.elf fib.s
d:/ccc/ccc109/sp_more/riscv/freedomstudio-2020-06-3-win64/sifive/riscv64-unknown-elf-gcc-8.3.0-2020.04.1/bin/../lib/gcc/riscv64-unknown-elf/8.3.0/../../../../riscv64-unknown-elf/bin/ld.exe: warning: cannot find entry symbol _start; defaulting to 0000000000000000
riscv64-unknown-elf-objcopy -O binary fib.elf fib.bin

$ riscv64-unknown-elf-objdump -d fib.elf

fib.elf:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <main>:
   0:   1141                    addi    sp,sp,-16
   2:   e406                    sd      ra,8(sp)
   4:   e022                    sd      s0,0(sp)
   6:   0800                    addi    s0,sp,16
   8:   4529                    li      a0,10
   a:   010000ef                jal     ra,1a <fib>
   e:   87aa                    mv      a5,a0
  10:   853e                    mv      a0,a5
  12:   60a2                    ld      ra,8(sp)
  14:   6402                    ld      s0,0(sp)
  16:   0141                    addi    sp,sp,16
  18:   8082                    ret

000000000000001a <fib>:
  1a:   7179                    addi    sp,sp,-48
  1c:   f406                    sd      ra,40(sp)
  1e:   f022                    sd      s0,32(sp)
  20:   ec26                    sd      s1,24(sp)
  22:   1800                    addi    s0,sp,48
  24:   87aa                    mv      a5,a0
  26:   fcf42e23                sw      a5,-36(s0)
  2a:   fdc42783                lw      a5,-36(s0)
  2e:   2781                    sext.w  a5,a5
  30:   cb81                    beqz    a5,40 <fib+0x26>
  32:   fdc42783                lw      a5,-36(s0)
  36:   0007871b                sext.w  a4,a5
  3a:   4785                    li      a5,1
  3c:   00f71563                bne     a4,a5,46 <fib+0x2c>
  40:   fdc42783                lw      a5,-36(s0)
  44:   a025                    j       6c <fib+0x52>
  46:   fdc42783                lw      a5,-36(s0)
  4a:   37fd                    addiw   a5,a5,-1
  4c:   2781                    sext.w  a5,a5
  4e:   853e                    mv      a0,a5
  50:   fcbff0ef                jal     ra,1a <fib>
  54:   87aa                    mv      a5,a0
  56:   84be                    mv      s1,a5
  58:   fdc42783                lw      a5,-36(s0)
  5c:   37f9                    addiw   a5,a5,-2
  5e:   2781                    sext.w  a5,a5
  60:   853e                    mv      a0,a5
  62:   fb9ff0ef                jal     ra,1a <fib>
  66:   87aa                    mv      a5,a0
  68:   9fa5                    addw    a5,a5,s1
  6a:   2781                    sext.w  a5,a5
  6c:   853e                    mv      a0,a5
  6e:   70a2                    ld      ra,40(sp)
  70:   7402                    ld      s0,32(sp)
  72:   64e2                    ld      s1,24(sp)
  74:   6145                    addi    sp,sp,48
  76:   8082                    ret
```