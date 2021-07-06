```
guest@localhost:~/sp/10-riscv/03-mini-riscv-os$ cd 05-Preemptive/
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/05-Preemptive$ ls
05-Preemptive--強制切換.md  lib.h     os.c  os.ld      riscv.h  sys.h  task.c  timer.c  trap.c
lib.c                       Makefile  os.h  README.md  start.s  sys.s  task.h  timer.h  user.c
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/05-Preemptive$ make
riscv64-unknown-elf-gcc -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T os.ld -o os.elf start.s sys.s lib.c timer.c task.c os.c user.c trap.c
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/05-Preemptive$ make qemu
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel os.elf
OS start
OS: Activate next task
Task0: Created!
Task0: Running...
timer interruption!
timer_handler: 1
OS: Back to OS

OS: Activate next task
Task1: Created!
Task1: Running...
timer interruption!
timer_handler: 2
OS: Back to OS

OS: Activate next task
Task0: Running...
timer interruption!
timer_handler: 3
OS: Back to OS

OS: Activate next task
Task1: Running...
timer interruption!
timer_handler: 4
OS: Back to OS

OS: Activate next task
Task0: Running...
QEMU: Terminated
```