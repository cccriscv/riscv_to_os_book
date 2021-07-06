# 01b-key

## Build & Run

```sh
user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/ 
系統程式/10-riscv/03-mini-riscv-os/01b-gets (master)$ make clean
rm -f *.elf

user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/ 
系統程式/10-riscv/03-mini-riscv-os/01b-gets (master)$ make
riscv64-unknown-elf-gcc -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T os.ld -o os.elf start.s os.c

user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/ 
系統程式/10-riscv/03-mini-riscv-os/01b-gets (master)$ make qemu
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt 
-bios none -kernel os.elf
type something:hello
you type:hello
QEMU: Terminated
```
