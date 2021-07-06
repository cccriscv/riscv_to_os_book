K = ./kernel
A = ./arch/$(ARCH)
L = ../lib5

CC = riscv64-unknown-elf-gcc
CFLAGS = -I$(K) -I$(L) -I$(A) -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32
#CFLAGS = -I$(K) -I$(L) -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32 -L$(L)/out/riscv32

QEMU = qemu-system-riscv32
QFLAGS = -nographic -smp 4 -machine virt -bios none

all: os.elf

os.elf: $(K)/start.s $(K)/uart.c $(K)/std.c $(K)/nix.c $(K)/main.c $(K)/console.c $(A)/board.c $(L)/string.c $(L)/stdio.c
	$(CC) $(CFLAGS) -T os.ld -o $(A)/os.elf $^

#os.elf: $(K)/start.s $(K)/os.c $(K)/main.c $(A)/board.c
#	$(CC) $(CFLAGS) -l5 -T os.ld -o $(A)/os.elf $^

run: $(TARGET)
	@qemu-system-riscv32 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	$(QEMU) $(QFLAGS) -kernel $(A)/os.elf
