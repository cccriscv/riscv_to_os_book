LIB = ../../lib
OS = ../../os

CC = riscv64-unknown-elf-gcc
CFLAGS = -I$(OS) -I$(LIB) -nostdlib -fno-builtin -mcmodel=medany -march=rv32ima -mabi=ilp32

QEMU = qemu-system-riscv32
QFLAGS = -nographic -smp 4 -machine virt -bios none

OBJDUMP = riscv64-unknown-elf-objdump

all: os.elf

os.elf: $(OS)/start.s $(OS)/os.c $(OS)/board.c $(LIB)/string.c $(LIB)/stdio.c ../test.c
	$(CC) $(CFLAGS) -T $(OS)/os.ld -o os.elf $^

qemu: $(TARGET)
	@qemu-system-riscv32 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	$(QEMU) $(QFLAGS) -kernel os.elf

clean:
	rm -f *.elf
