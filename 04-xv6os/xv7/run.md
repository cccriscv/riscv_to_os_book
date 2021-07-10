# xv7 -- add c4/editor

```
user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/程式
/C/riscv_to_os_book/04-xv6os/xv7
$ make qemu
gcc -Werror -Wall -I. -o mkfs/mkfs mkfs/mkfs.c
mkfs/mkfs fs.img README hello.c sum.c fib.c arg.c 
user/_cat user/_echo user/_forktest user/_grep user/_init user/_kill user/_ln user/_ls user/_mkdir user/_rm user/_sh user/_stressfs user/_usertests user/_grind user/_wc user/_zombie user/_editor user/_c4 user/_cp
nmeta 46 (boot, super, log blocks 30 inode blocks 
13, bitmap blocks 1) blocks 954 total 1000        
balloc: first 801 blocks have been allocated
balloc: write bitmap block at sector 45
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 256M -smp 3 -nographic -drive 
file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0       

xv6 kernel is booting

hart 1 starting
hart 2 starting
init: starting sh
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2102
hello.c        2 3 82
sum.c          2 4 235
fib.c          2 5 163
arg.c          2 6 132
cat            2 7 28648
echo           2 8 27480
forktest       2 9 13184
grep           2 10 31960
init           2 11 28512
kill           2 12 27464
ln             2 13 c27432
ls             2 14 30864
mkdir          2 15 27560
rm             2 16 27544
sh             2 17 46520
stressfs       2 18 28496
usertests      2 194 155904
grind          2 20 42728
wc             2 21 29736
zombie         2 22 26960
editor         2 23 35512
 c4             2 24 73344
cp             2 25 28152
console        3 26 0
$ fib.c
f(7)=13
exit(8) cycle = 920
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2102
hello.c        2 3 82
sum.c          2 4 235
fib.c          2 5 163
arg.c          2 6 132
cat            2 7 28648
echo           2 8 27480
forktest       2 9 13184
grep           2 10 31960
init           2 11 28512
kill           2 12 27464
ln             2 13 27432
ls             2 14 30864
mkdir          2 15 27560
rm             2 16 27544
sh             2 17 46520
stressfs       2 18 28496
usertests      2 19 155904
grind          2 20 42728
wc             2 21 29736
zombie         2 22 26960
editor         2 23 35512
c4             2 24 73344
cp             2 25 28152
console        3 26 0
$ c4 sum.c
sum(10)=55
exit(0) cycle = 303
$ QEMU: Terminated
```
