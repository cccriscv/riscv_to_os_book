all: pc virt32

pc:
	make -f pc.mk
	make -f pc.mk run

virt32:
	make -f riscv.mk ARCH=virt32
	make -f riscv.mk ARCH=virt32 run

clean:
	find . -name '*.o' -delete
	find . -name '*.elf' -delete