# 02-mylib (RISC-V)

## run

```
user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系 
統程式/10-riscv/05-embed-riscv-os/01-uart/riscv (master)
$ make
riscv64-unknown-elf-gcc -I./ -I../src -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T os.ld -o os.elf start.s os.c board.c ../src/mylib.c ../test/test.c

user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系 
統程式/10-riscv/05-embed-riscv-os/01-uart/riscv (master)
$ make qemu
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel os.elf
hello ccc, you are 51 years old!
type some thing: xxx
you type: xxx
QEMU: Terminated
```
