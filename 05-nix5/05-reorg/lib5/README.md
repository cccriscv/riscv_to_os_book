# C-lib


## build

```     
$ make ARCH=pc
gcc -I. -c stdio.c -o out/pc/stdio.o
gcc -I. -c string.c -o out/pc/string.o
ar -rcs out/pc/lib5.a out/pc/stdio.o out/pc/string.o
rm out/pc/stdio.o out/pc/string.o

$ make ARCH=riscv64
riscv64-unknown-elf-gcc -I. -nostdlib -fno-builtin -mcmodel=medany -march=rv64gc -c stdio.c -o out/riscv64/stdio.o
riscv64-unknown-elf-gcc -I. -nostdlib -fno-builtin -mcmodel=medany -march=rv64gc -c string.c -o out/riscv64/string.o
riscv64-unknown-elf-ar -rcs out/riscv64/lib5.a out/riscv64/stdio.o out/riscv64/string.o
rm out/riscv64/stdio.o out/riscv64/string.o

$ make ARCH=riscv32
riscv64-unknown-elf-gcc -I. -nostdlib -fno-builtin -mcmodel=medany -march=rv32gc -mabi=ilp32 -c stdio.c -o out/riscv32/stdio.o
riscv64-unknown-elf-gcc -I. -nostdlib -fno-builtin -mcmodel=medany -march=rv32gc -mabi=ilp32 -c string.c -o 
out/riscv32/string.o
riscv64-unknown-elf-ar -rcs out/riscv32/lib5.a out/riscv32/stdio.o out/riscv32/string.o
rm out/riscv32/stdio.o out/riscv32/string.o
```

## Reference

* https://github.com/JacobPolloreno/Mini-Lib-C