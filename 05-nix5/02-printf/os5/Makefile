all: pc riscv32

pc: 
	make -f pc.mk
	make -f pc.mk run

riscv32: 
	make -f riscv.mk ARCH=riscv32
	make -f riscv.mk ARCH=riscv32 run

clean:
	find . -name '*.o' -delete
	find . -name '*.elf' -delete