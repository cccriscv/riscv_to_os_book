## RISC-V 上 gcc 工具鏈的使用

RISC-V 的 C 語言編譯器是修改過的 gcc，64 位元版的 RISC-V gcc 稱為 riscv64-unknown-elf-gcc ，我們可以用 where 指令找出其位置：

```sh
$ where riscv64-unknown-elf-gcc
D:\install\FreedomStudio-2020-06-3-win64\SiFive\riscv64-unknown-elf-gcc-8.3.0-2020.04.1\bin\riscv64-unknown-elf-gcc.exe
```

我們可以利用這個 gcc 來編譯 C 語言程式，或者當組譯器來組譯組合語言程式碼。

首先讓我們寫一個 c 語言程式如下:

File: add.c

```cpp
int add(int a, int b) {
  return a+b;
}
```

接著使用 riscv64-unknown-elf-gcc 將其編譯為組合語言。

```
$ riscv64-unknown-elf-gcc -S add.c -o add.s
```

此時我們會看到 add.s 這個組合語言檔產生出來，內容如下：

```s
	.file	"add.c"
	.option nopic
	.attribute arch, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	1
	.globl	add
	.type	add, @function
add:
	addi	sp,sp,-32
	sd	s0,24(sp)
	addi	s0,sp,32
	mv	a5,a0
	mv	a4,a1
	sw	a5,-20(s0)
	mv	a5,a4
	sw	a5,-24(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	addw	a5,a4,a5
	sext.w	a5,a5
	mv	a0,a5
	ld	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	add, .-add
	.ident	"GCC: (SiFive GCC 8.3.0-2020.04.1) 8.3.0"
```

這就是一個 RISC-V 的組合語言檔了！

接著我們用下列指令將其轉換成 ELF 格式的目的檔：

```
$ riscv64-unknown-elf-gcc -c add.s -o add.elf
```

然後用 objdump 程式將其反組譯回組合語言，此時可以看到其機器碼：

```
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

您會看到機器碼中有些指令是 4 碼 (像是 sd s0,24(sp) 這個指令的機器碼為 ec22)，另一些指令是 8 碼 (像是 sw a5,-20(s0) 這個指令的機器碼為 fef42623)，其中 4 碼的代表 16 bits 的壓縮模式指令 (一個十六進位對應到 4 個二進位，4*4=16bit)，而8碼的代表標準的 32 bits 指令。

## 不同架構下的 gcc 編譯方法

當我們使用下列指令，直接將 add.c 編譯為目的檔 add.elf 時，其產生的架構預設為《64位元且壓縮》的 rv64gc 格式，因此產生出來的指令長度會混雜 32 位元與 16 位元兩種。

必須注意的是，RISC-V 64 位元模式指的是具有 64 位元暫存器，但指令長度最大仍然是 32 位元的，例如以下組合語言中的 `sd s0,24(sp)` 是將 64 位元的 s0 暫存器存入到 24+sp 這個記憶體位址內，總共佔據 8 bytes (64 bits)。

```
$ riscv64-unknown-elf-gcc -c add.c -o add.elf
// 上一行相當於 riscv64-unknown-elf-gcc -march=rv64gc -c add.c -o add.elf

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

但是如果我們不使用壓縮格式節省空間，那麼就可以指定 -march=rv64g 產生完全不壓縮的 32 位元指令碼如下：

```
user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02-sp/02-gcc (master)
$ riscv64-unknown-elf-gcc -march=rv64g -c add.c -o add64.elf

user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02-sp/02-gcc (master)
$ riscv64-unknown-elf-objdump -d add64.elf

add64.elf:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <add>:
   0:   fe010113                addi    sp,sp,-32
   4:   00813c23                sd      s0,24(sp)
   8:   02010413                addi    s0,sp,32
   c:   00050793                mv      a5,a0
  10:   00058713                mv      a4,a1
  14:   fef42623                sw      a5,-20(s0)
  18:   00070793                mv      a5,a4
  1c:   fef42423                sw      a5,-24(s0)
  20:   fec42703                lw      a4,-20(s0)
  24:   fe842783                lw      a5,-24(s0)
  28:   00f707bb                addw    a5,a4,a5
  2c:   0007879b                sext.w  a5,a5
  30:   00078513                mv      a0,a5
  34:   01813403                ld      s0,24(sp)
  38:   02010113                addi    sp,sp,32
  3c:   00008067                ret
```

但是如果我們的 RISC-V 處理器是 32 位元的，那麼就應該用下列指令去產生 32 bit 暫存器的機器碼。

(這時候要指定機器的 abi 格式為 ilp32，意思是 int, long, pointer 都是 32 位元，以產生 32 位元格式的二進位檔)。

```
user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02-sp/02-gcc (master)
$ riscv64-unknown-elf-gcc -march=rv32gc -c add.c -o add32.elf
cc1.exe: error: ABI requires -march=rv64

user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02-sp/02-gcc (master)
$ riscv64-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -c add.c -o add32.elf

user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02-sp/02-gcc (master)
$ riscv64-unknown-elf-objdump -d add32.elf

add32.elf:     file format elf32-littleriscv


Disassembly of section .text:

00000000 <add>:
   0:   1101                    addi    sp,sp,-32
   2:   ce22                    sw      s0,28(sp)
   4:   1000                    addi    s0,sp,32
   6:   fea42623                sw      a0,-20(s0)
   a:   feb42423                sw      a1,-24(s0)
   e:   fec42703                lw      a4,-20(s0)
  12:   fe842783                lw      a5,-24(s0)
  16:   97ba                    add     a5,a5,a4
  18:   853e                    mv      a0,a5
  1a:   4472                    lw      s0,28(sp)
  1c:   6105                    addi    sp,sp,32
  1e:   8082                    ret
```

## 結語

現在，您應該已經會使用 RISC-V 的 gcc 與 objdump 等工具了，也就是 riscv64-unknown-elf-gcc 與 riscv64-unknown-elf-objdump。

但是到目前為止，雖然已經能編譯程式了，卻無法執行這些編譯出來的 ELF 目的檔 (就算有寫 main 函數也不行)。

因為我們電腦的 CPU 是 x86 處理器，而編譯出來的 ELF 檔卻是 RISC-V 的機器碼，所以我們無法執行這些碼。

我們必須使用 QEMU 虛擬機來執行 RISC-V 的機器碼！

(說明： RISC-V 組織原本在 linux 上有釋出一個稱為 spike 的虛擬機環境，但在 windows 沒有，而且 spike 似乎已經停止維護，改成以 QEMU 為主了)

接下來，我們將使用 gcc + qemu + make 來開發並執行 RISC-V 上的程式專案。
