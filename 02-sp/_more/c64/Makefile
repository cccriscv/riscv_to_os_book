fib.bin: fib.c
	riscv64-unknown-elf-gcc -S fib.c
	riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 -nostdlib -o fib.elf fib.s
	riscv64-unknown-elf-objcopy -O binary fib.elf fib.bin
	riscv64-unknown-elf-objdump -d fib.elf

clean:
	rm -f fib.elf fib.bin
