riscv64-unknown-elf-gcc -march=rv32imac -mabi=ilp32 -T default.lds -o main.o -nostdlib -fno-builtin start.s main.c

qemu-system-riscv32 -nographic -smp 4 -machine virt -bios none -kernel main.o

