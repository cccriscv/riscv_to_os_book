# SPIKE

* https://github.com/riscv/riscv-isa-sim

## usage

```
root@localhost:~/ccc/riscv# riscv64-unknown-elf-gcc -o hello hello.c
root@localhost:~/ccc/riscv# spike -m256 pk hello
bbl loader
Hello!
```