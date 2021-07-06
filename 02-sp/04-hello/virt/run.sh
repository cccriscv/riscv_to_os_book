riscv64-unknown-elf-gcc -march=rv32g -mabi=ilp32 -static -mcmodel=medany \
  -fvisibility=hidden -nostdlib -nostartfiles -Thello.ld -I. \
  ../hello.s -o hello.elf

qemu-system-riscv32 -nographic -machine virt -bios none -kernel hello.elf
