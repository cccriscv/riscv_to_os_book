# linux run

```
guest@localhost:~/sp/10-riscv/nix-v/test/qemu$ make clean
rm -f *.elf
guest@localhost:~/sp/10-riscv/nix-v/test/qemu$ make
riscv64-unknown-elf-gcc -I../../os -I../../lib -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T ../../os/os.ld -o os.elf ../../os/start.s ../../os/os.c ../../os/board.c ../../lib/string.c ../../lib/stdio.c ../test.c
guest@localhost:~/sp/10-riscv/nix-v/test/qemu$ make qemu
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel os.elf
hello ccc, you are 51 years old!
type some thing: xxx
you type: xxx
QEMU: Terminated
```
