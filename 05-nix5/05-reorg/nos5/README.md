# os5

## reference

* https://github.com/mit-pdos/xv6-riscv


## riscv32

```
user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系
統程式/10-riscv/05-os5/02-printf/os5 (master)        
$ make ARCH=riscv32
riscv64-unknown-elf-gcc -I./kernel -I../lib5 -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T os.ld -o ./arch/riscv32/os.elf kernel/start.s kernel/os.c kernel/main.c arch/riscv32/board.c ../lib5/string.c ../lib5/stdio.c

user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系
統程式/10-riscv/05-os5/02-printf/os5 (master)        
$ make ARCH=riscv32 run
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel ./arch/riscv32/os.elf
hello ccc, you are 51 years old!
type some thing: xxx
you type: xxx
QEMU: Terminated
```

## pc

```
user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系
統程式/10-riscv/05-os5/02-printf/os5 (master)        
$ make -f pc.mk
gcc -I../lib5 -L../lib5/out/pc -l5 kernel/main.c -o arch/pc/os

user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系
統程式/10-riscv/05-os5/02-printf/os5 (master)        
$ make -f pc.mk run
gcc -I../lib5 -L../lib5/out/pc -l5 kernel/main.c -o arch/pc/os
arch/pc/os
hello ccc, you are 51 years old!
type some thing: xxx
you type: xxx
```