K = .
A = ../arch/$(ARCH)
L = ../lib5
V = ../vfs5
IO = ../io5

CC = riscv64-unknown-elf-gcc
CFLAGS = -I$(K) -I$(L) -I$(A) -I$(V) -I$(IO) -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32
#CFLAGS = -I$(K) -I$(L) -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -L$(L)/out/riscv32

QEMU = qemu-system-riscv32
QFLAGS = -nographic -smp 4 -machine virt -bios none

all: os.elf

os.elf: $(K)/start.s $(K)/os.c $(K)/main.c $(IO)/uart.c $(IO)/io.c $(V)/vfs.c $(V)/console.c $(V)/ramfile.c $(A)/board.c $(L)/string.c $(L)/stdio.c
	$(CC) $(CFLAGS) -T os.ld -o $(A)/os.elf $^

#os.elf: $(K)/start.s $(K)/os.c $(K)/main.c $(A)/board.c
#	$(CC) $(CFLAGS) -l5 -T os.ld -o $(A)/os.elf $^

run: $(TARGET)
	@qemu-system-riscv32 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	$(QEMU) $(QFLAGS) -kernel $(A)/os.elf
