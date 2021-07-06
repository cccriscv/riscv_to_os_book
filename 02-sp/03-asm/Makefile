add: add.s
	riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -o add.elf add.s
	riscv64-unknown-elf-objcopy -O binary add.elf add.bin
	riscv64-unknown-elf-objdump -d add.elf

clean:
	rm -f add.elf add.bin
