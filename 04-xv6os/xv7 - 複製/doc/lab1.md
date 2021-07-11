# xv6-lab1

* 習題指引 -- https://pdos.csail.mit.edu/6.828/2020/labs/util.html
* 本文程式來源 -- https://blog.mky.moe/mit6828/1-lab01/

## sleep.c

新增 sleep.c 檔案，內容如下：

檔案: [user/sleep.c](./user/sleep.c)

```cpp
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 2)
    write(2, "Error message", strlen("Error message"));

  int x = atoi(argv[1]);

  sleep(x);

  exit(0);
}
```

然後修改 Makefile 加入 $U/_sleep\ 到 UPROGS 中，如下：

```
...

UPROGS=\
	$U/_sleep\
	$U/_cat\

...
```

接著重新編譯 xv6，過程如下：

```
user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/05-xv6-lab1 (master)
$ make qemu
gcc -Werror -Wall -I. -o mkfs/mkfs mkfs/mkfs.c
riscv64-unknown-elf-gcc -Wall -Werror -O -fno-omit-frame-pointer -ggdb -DSOL_UTIL -MD -mcmodel=medany -ffreestanding 
-fno-common -nostdlib -mno-relax -I. -fno-stack-protector -fno-pie -no-pie   -c -o user/sleep.o user/sleep.c
riscv64-unknown-elf-ld -z max-page-size=4096 -N -e main -Ttext 0 -o user/_sleep user/sleep.o user/ulib.o user/usys.o 
user/printf.o user/umalloc.o
riscv64-unknown-elf-objdump -S user/_sleep > user/sleep.asm
riscv64-unknown-elf-objdump -t user/_sleep | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$/d' > user/sleep.sym
mkfs/mkfs fs.img README  user/xargstest.sh user/_sleep user/_cat user/_echo user/_forktest user/_grep user/_init user/_kill user/_ln user/_ls user/_mkdir user/_rm user/_sh user/_stressfs user/_usertests user/_grind user/_wc user/_zombie 
nmeta 46 (boot, super, log blocks 30 inode blocks 13, bitmap blocks 1) blocks 954 total 1000
balloc: first 612 blocks have been allocated
balloc: write bitmap block at sector 45
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

xv6 kernel is booting

hart 2 starting
hart 1 starting
init: starting sh
$ sleep 10
$ sleep 100
$ QEMU 4.2.0 monitor - type 'help' for more information
(qemu) quit
```

## pingpong.c

修改方法參考 sleep.c

檔案: [user/pingpong.c](./user/pingpong.c)

```
user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/05-xv6-lab1 (master)
$ make qemu
...

xv6 kernel is booting

hart 2 starting
hart 1 starting
init: starting sh
$ ls  
.              1 1 1024
..             1 1 1024
README         2 2 2104
xargstest.sh   2 3 99
pingpong       2 4 23368
sleep          2 5 22768
cat            2 6 23944
echo           2 7 22776
forktest       2 8 13112
grep           2 9 27256
init           2 10 23904
kill           2 11 22744
ln             2 12 22728
ls             2 13 26152
mkdir          2 14 22864
rm             2 15 22848
sh             2 16 41800
stressfs       2 17 23776
usertests      2 18 145312
grind          2 19 37976
wc             2 20 25040
zombie         2 21 22272
console        3 22 0
$ pingpong
5: received ping
4: received pong
$ QEMU 4.2.0 monitor - type 'help' for more information
(qemu) quit
```

## primes.c

檔案: [user/primes.c](./user/primes.c)

```
$ make qemu
...

xv6 kernel is booting

hart 2 starting
hart 1 starting
init: starting sh
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2104
xargstest.sh   2 3 99
primes         2 4 24656
pingpong       2 5 23328
sleep          2 6 22728
cat            2 7 23944
echo           2 8 22776
forktest       2 9 13112
grep           2 10 27256
init           2 11 23904
kill           2 12 22744
ln             2 13 22728
ls             2 14 26152
mkdir          2 15 22864
rm             2 16 22848
sh             2 17 41800
stressfs       2 18 23776
usertests      2 19 145312
grind          2 20 37976
wc             2 21 25040
zombie         2 22 22272
console        3 23 0
$ primes
prime 2
prime 3
prime 5
prime 7
prime 11
prime 13
prime 17
prime 19
prime 23
prime 29
prime 31
$ QEMU 4.2.0 monitor - type 'help' for more information
(qemu) quit
```

## find.c

檔案: [user/primes.c](./user/primes.c)

```
user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/05-xv6-lab1 (master)
$ make qemu
gcc -Werror -Wall -I. -o mkfs/mkfs mkfs/mkfs.c
riscv64-unknown-elf-gcc -Wall -Werror -O -fno-omit-frame-pointer -ggdb -DSOL_UTIL -MD -mcmodel=medany -ffreestanding 
-fno-common -nostdlib -mno-relax -I. -fno-stack-protector -fno-pie -no-pie   -c -o user/find.o user/find.c
riscv64-unknown-elf-ld -z max-page-size=4096 -N -e main -Ttext 0 -o user/_find user/find.o user/ulib.o user/usys.o user/printf.o user/umalloc.o
riscv64-unknown-elf-objdump -S user/_find > user/find.asm
riscv64-unknown-elf-objdump -t user/_find | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$/d' > user/find.sym
mkfs/mkfs fs.img README  user/xargstest.sh user/_find user/_primes user/_pingpong user/_sleep user/_cat user/_echo user/_forktest user/_grep user/_init user/_kill user/_ln user/_ls user/_mkdir user/_rm user/_sh user/_stressfs user/_usertests user/_grind user/_wc user/_zombie 
nmeta 46 (boot, super, log blocks 30 inode blocks 13, bitmap blocks 1) blocks 954 total 1000
balloc: first 691 blocks have been allocated
balloc: write bitmap block at sector 45
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

xv6 kernel is booting

hart 2 starting
hart 1 starting
init: starting sh
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2104
xargstest.sh   2 3 99
find           2 4 28640
primes         2 5 24656
pingpong       2 6 23328
sleep          2 7 22728
cat            2 8 23944
echo           2 9 22776
forktest       2 10 13112
grep           2 11 27256
init           2 12 23904
kill           2 13 22744
ln             2 14 22728
ls             2 15 26152
mkdir          2 16 22864
rm             2 17 22848
sh             2 18 41800
stressfs       2 19 23776
usertests      2 20 145312
grind          2 21 37976
wc             2 22 25040
zombie         2 23 22272
console        3 24 0
$ echo > b
$ mkdir a
$ echo > a/b
$ find . b
./zombie
./b
./a/b
$ QEMU 4.2.0 monitor - type 'help' for more information
(qemu) quit
```

## xargs.c

檔案: [user/xargs.c](./user/xargs.c)

```
user@DESKTOP-96FRN6B MINGW64 /d/ccc109/co2os/code/05-xv6-lab1 (master)
$ make qemu
gcc -Werror -Wall -I. -o mkfs/mkfs mkfs/mkfs.c
riscv64-unknown-elf-gcc -Wall -Werror -O -fno-omit-frame-pointer -ggdb -DSOL_UTIL -MD -mcmodel=medany -ffreestanding 
-fno-common -nostdlib -mno-relax -I. -fno-stack-protector -fno-pie -no-pie   -c -o user/xargs.o user/xargs.c
riscv64-unknown-elf-ld -z max-page-size=4096 -N -e main -Ttext 0 -o user/_xargs user/xargs.o user/ulib.o user/usys.o 
user/printf.o user/umalloc.o
riscv64-unknown-elf-objdump -S user/_xargs > user/xargs.asm
riscv64-unknown-elf-objdump -t user/_xargs | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$/d' > user/xargs.sym
mkfs/mkfs fs.img README  user/xargstest.sh user/_xargs user/_find user/_primes user/_pingpong user/_sleep user/_cat user/_echo user/_forktest user/_grep user/_init user/_kill user/_ln user/_ls user/_mkdir user/_rm user/_sh user/_stressfs user/_usertests user/_grind user/_wc user/_zombie 
nmeta 46 (boot, super, log blocks 30 inode blocks 13, bitmap blocks 1) blocks 954 total 1000
balloc: first 716 blocks have been allocated
balloc: write bitmap block at sector 45
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 128M -smp 3 -nographic -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

xv6 kernel is booting

hart 2 starting
hart 1 starting
init: starting sh
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2104
xargstest.sh   2 3 99
xargs          2 4 24456
find           2 5 28640
primes         2 6 24656
pingpong       2 7 23328
sleep          2 8 22728
cat            2 9 23944
echo           2 10 22776
forktest       2 11 13112
grep           2 12 27256
init           2 13 23904
kill           2 14 22744
ln             2 15 22728
ls             2 16 26152
mkdir          2 17 22864
rm             2 18 22848
sh             2 19 41800
stressfs       2 20 23776
usertests      2 21 145312
grind          2 22 37976
wc             2 23 25040
zombie         2 24 22272
console        3 25 0
$ echo hello too | xargs echo bye
bye hello too 
$ echo "1\n2" | xargs -n 1 echo line
$ find . b | xargs grep hello
$ sh < xargstest.sh
$ $ $ $ $ $ $ $ $ $ $ hello
hello
$ $ $ QEMU 4.2.0 monitor - type 'help' for more information
(qemu) quit
```
