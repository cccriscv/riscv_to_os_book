# riscv64-unknown-elf-gcc

## 安裝 FreedomStudio

設定 path 路徑到 FreedomStudio\SiFive\riscv64-unknown-elf-gcc-xxx\bin

例如：

```
D:\ccc\ccc109\sp_more\riscv\FreedomStudio-2020-06-3-win64\SiFive\riscv64-unknown-elf-gcc-8.3.0-2020.04.1\bin
```

然後用 riscv64-unknown-elf-gcc 編譯：

```
PS D:\ccc\ccc109\sp\10-riscv\gcc> riscv64-unknown-elf-gcc -S hello.c -o hello.s
PS D:\ccc\ccc109\sp\10-riscv\gcc> riscv64-unknown-elf-gcc -S fib.c -o fib.s
PS D:\ccc\ccc109\sp\10-riscv\gcc> riscv64-unknown-elf-gcc -S sum.c -o sum.s
```

產生 rv32 的版本

```
$ riscv64-unknown-elf-gcc -S -march=rv32imafdc -mabi=ilp32 hello.c -o hello32.s
```

結果 hello32.s 如下，確實是 32 位元的組合語言。

```
	.file	"hello.c"
	.option nopic
	.attribute arch, "rv32i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.section	.rodata
	.align	2
.LC0:
	.string	"hello!"
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	addi	s0,sp,16
	lui	a5,%hi(.LC0)
	addi	a0,a5,%lo(.LC0)
	call	puts
	li	a5,0
	mv	a0,a5
	lw	ra,12(sp)
	lw	s0,8(sp)
	addi	sp,sp,16
	jr	ra
	.size	main, .-main
	.ident	"GCC: (SiFive GCC 8.3.0-2020.04.1) 8.3.0"

```

## objdump

```
user@DESKTOP-96FRN6B MINGW64 /d/ccc/ccc109/sp/10-riscv/gcc (master)
$ riscv64-unknown-elf-gcc -c hello.c -o hello.elf

user@DESKTOP-96FRN6B MINGW64 /d/ccc/ccc109/sp/10-riscv/gcc (master)
$ riscv64-unknown-elf-objdump -d hello.elf

hello.elf:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <main>:
   0:   1141                    addi    sp,sp,-16
   2:   e406                    sd      ra,8(sp)
   4:   e022                    sd      s0,0(sp)
   6:   0800                    addi    s0,sp,16
   8:   000007b7                lui     a5,0x0
   c:   00078513                mv      a0,a5
  10:   00000097                auipc   ra,0x0
  14:   000080e7                jalr    ra # 10 <main+0x10>
  18:   4781                    li      a5,0
  1a:   853e                    mv      a0,a5
  1c:   60a2                    ld      ra,8(sp)
  1e:   6402                    ld      s0,0(sp)
  20:   0141                    addi    sp,sp,16
  22:   8082                    ret
```

## 產生 ELF

```
$ riscv64-unknown-elf-gcc hello.c -o hello.elf
```



## 產生 binary

使用下列方法

```
$ riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -o foo foo.s
$ riscv64-unknown-elf-objcopy -O binary foo foo.bin
```

實際案例

```
$ riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -o add.elf add.s
d:/ccc/ccc109/sp_more/riscv/freedomstudio-2020-06-3-win64/sifive/riscv64-unknown-elf-gcc-8.3.0-2020.04.1/bin/../lib/gcc/riscv64-unknown-elf/8.3.0/../../../../riscv64-unknown-elf/bin/ld.exe: warning: cannot find entry symbol _start; defaulting to 0000000000000000

$ riscv64-unknown-elf-objcopy -O binary add.elf add.bin
```

## VM spike ??

但是不能直接編譯成執行檔執行！

用 spike ?? 在哪裡？ ?? D:\ccc\ccc109\sp_more\riscv\FreedomStudio-2020-06-3-win64\SiFive\freedom-e-sdk-v20.05.00.02\bsp\spike

## qemu

* D:\ccc\ccc109\sp_more\riscv\FreedomStudio-2020-06-3-win64\SiFive\riscv-qemu-4.2.0-2020.04.0\bin\qemu-system-riscv64.exe

問題，能否讓 xv6 在 windows 裏用 riscv64-unknown-elf-gcc compile 成功呢？


