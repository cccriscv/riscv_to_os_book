fib.bin: fib.c
	riscv64-unknown-elf-gcc -march=rv32imafd -mabi=ilp32 -S fib.c
	riscv64-unknown-elf-gcc -march=rv32imafd -mabi=ilp32 -Tproject.ld -nostdlib -o fib.elf fib.s
	riscv64-unknown-elf-objcopy -O binary fib.elf fib.bin
	riscv64-unknown-elf-objdump -d fib.elf

clean:
	rm -f fib.elf fib.bin

# riscv64-unknown-elf-gcc -march=rv32g -mabi=ilp32 -static -mcmodel=medany \
#   -fvisibility=hidden -nostdlib -nostartfiles -Thello.ld hello.s -o hello