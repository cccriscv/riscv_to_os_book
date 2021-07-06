# main

```
user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02/02-main (master)
$ riscv64-unknown-elf-gcc -S main.c -o main.s

user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02/02-main (master)
$ riscv64-unknown-elf-gcc -c main.s -o main.elf

user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02/02-main (master)
$ where qemu-system-riscv64
C:\Program Files\qemu\qemu-system-riscv64.exe

user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/02/02-main (master)
$ qemu-system-riscv64 main.elf

// 當掉

```