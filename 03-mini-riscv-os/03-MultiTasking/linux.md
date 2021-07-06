
```
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/03-MultiTasking$ ls
03-MultiTasking--RISC-V的協同式多工.md  lib.h     os.c  os.ld    run.md   sys.h  task.c  user.c
lib.c                                   Makefile  os.h  riscv.h  start.s  sys.s  task.h
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/03-MultiTasking$ make 
riscv64-unknown-elf-gcc -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T os.ld -o os.elf start.s sys.s lib.c task.c os.c user.c
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/03-MultiTasking$ make qemu
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel os.elf
OS start
OS: Activate next task
Task0: Created!
Task0: Now, return to kernel mode
OS: Back to OS

OS: Activate next task
Task1: Created!
Task1: Now, return to kernel mode
OS: Back to OS

OS: Activate next task
Task0: Running...
OS: Back to OS

OS: Activate next task
Task1: Running...
OS: Back to OS

OS: Activate next task
Task0: Running...
OS: Back to OS

OS: Activate next task
Task1: Running...
QEMU: Terminated
```
