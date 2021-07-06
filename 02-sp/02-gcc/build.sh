riscv64-unknown-elf-gcc -S add.c -o add.s
riscv64-unknown-elf-gcc -c add.s -o add.elf
riscv64-unknown-elf-objdump -d add.elf

riscv64-unknown-elf-gcc -march=rv64gc -c add.c -o add.elf
riscv64-unknown-elf-objdump -d add.elf

riscv64-unknown-elf-gcc -march=rv64g -c add.c -o add64.elf
riscv64-unknown-elf-objdump -d add64.elf

riscv64-unknown-elf-gcc -march=rv64g -mabi=lp64 -c add.c -o add64.elf
riscv64-unknown-elf-objdump -d add64.elf

riscv64-unknown-elf-gcc -march=rv32gc -mabi=ilp32 -c add.c -o add32.elf
riscv64-unknown-elf-objdump -d add32.elf

riscv64-unknown-elf-gcc -march=rv32imafd -mabi=ilp32 -c add.c -o add32.elf
riscv64-unknown-elf-objdump -d add32.elf
