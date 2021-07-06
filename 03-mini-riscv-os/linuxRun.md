# mini-riscv-os run on linux

```
guest@localhost:~/sp/10-riscv/03-mini-riscv-os$ cd 01-HelloOs/
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/01-HelloOs$ ls
doc.md  Makefile  os.c  os.elf  os.ld  run.md  start.s
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/01-HelloOs$ make clean
rm -f *.elf
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/01-HelloOs$ make
riscv64-unknown-elf-gcc -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T os.ld -o os.elf start.s os.c
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/01-HelloOs$ make qemu
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel os.elf
Hello OS!
QEMU: Terminated
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/01-HelloOs$ cd ..
guest@localhost:~/sp/10-riscv/03-mini-riscv-os$ cd 02-ContextSwitch/
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/02-ContextSwitch$ ls
doc.md  lib.c  lib.h  Makefile  os.c  os.h  os.ld  riscv.h  run.md  start.s  sys.h  sys.s
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/02-ContextSwitch$ make clean
rm -f *.elf
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/02-ContextSwitch$ make
riscv64-unknown-elf-gcc -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T os.ld -o os.elf start.s sys.s lib.c os.c
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/02-ContextSwitch$ make qemu
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel os.elf
OS start
Task0: Context Switch Success !
QEMU: Terminated
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/02-ContextSwitch$ cd ..
guest@localhost:~/sp/10-riscv/03-mini-riscv-os$ cd 03-MultiTasking/
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/03-MultiTasking$ ls
doc.md  lib.c  lib.h  Makefile  os.c  os.h  os.ld  riscv.h  run.md  start.s  sys.h  sys.s  task.c  task.h  user.c
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
OQEMU: Terminated

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
timer_handler: 6
QEMU: Terminated

guest@localhost:~/sp/10-riscv/03-mini-riscv-os$ cd 05-Preemptive/
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/05-Preemptive$ ls
doc.md  lib.h     os.c    os.h   riscv.h  start.s  sys.s   task.h   timer.h
lib.c   Makefile  os.elf  os.ld  run.md   sys.h    task.c  timer.c  user.c
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/05-Preemptive$ make clean
rm -f *.elf
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/05-Preemptive$ make
riscv64-unknown-elf-gcc -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -T os.ld -o os.elf start.s sys.s lib.c timer.c task.c os.c user.c
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/05-Preemptive$ make qmeu
make: *** No rule to make target 'qmeu'.  Stop.
guest@localhost:~/sp/10-riscv/03-mini-riscv-os/05-Preemptive$ make qemu
Press Ctrl-A and then X to exit QEMU
qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel os.elf
OS start
OS: Activate next task
Task0: Created!
Task0: Running...
timer_handler: 1
OS: Back to OS

OS: Activate next task
Task1: Created!
Task1: Running...
timer_handler: 2
OS: Back to OS

OS: Activate next task
Task0: Running...
timer_handler: 3
OS: Back to OS

OS: Activate next task
Task1: Running...
QEMU: Terminated
```