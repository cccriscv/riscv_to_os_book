```
guest@localhost:~/sp/10-riscv/03-mini-riscv-os$ cd 04-TimerInterrupt/
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/04-TimerInterrupt$ ls
04-TimerInterrupt--RISC-V的時間中斷.md  lib.h     os.c  os.ld    run.md   sys.h  timer.c
lib.c                                   Makefile  os.h  riscv.h  start.s  sys.s  timer.h
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/04-TimerInterrupt$ make
riscv64-unknown-elf-gcc -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T os.ld -o os.elf start.s sys.s lib.c timer.c os.c
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/04-TimerInterrupt$ make qemu
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel os.elf
OS start
timer_handler: 1
timer_handler: 2
timer_handler: 3
timer_handler: 4
timer_handler: 5
QEMU: Terminated
```