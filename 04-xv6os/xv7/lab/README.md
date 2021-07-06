# xv6 lab

```
user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系統程式/10-riscv/04-xv6os/xv6lab
$ make -f lab/lab.mk qemu
riscv64-unknown-elf-gcc    -c -o kernel/entry.o kernel/entry.S
riscv64-unknown-elf-gcc -Wall -Werror 
...
mkfs/mkfs fs.img README user/_cat user/_echo user/_forktest user/_grep user/_init user/_kill user/_ln user/_ls user/_mkdir user/_rm user/_sh user/_stressfs user/_usertests user/_grind user/_wc user/_zombie  lab/_hello
nmeta 46 (boot, super, log blocks 30 inode blocks 13, bitmap blocks 1) blocks 954 total 1000
balloc: first 616 blocks have been allocated
balloc: write bitmap block at sector 45
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 256M -smp 3 -nographic -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

xv6 kernel is booting

hart 2 starting
hart 1 starting
init: starting sh
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2102
cat            2 3 23968
echo           2 4 22792
forktest       2 5 13128
grep           2 6 27280
init           2 7 23928
kill           2 8 22768
ln             2 9 22752
ls             2 10 26176
mkdir          2 11 22888
rm             2 12 22872
sh             2 13 41824
stressfs       2 14 23800
usertests      2 15 151208
grind          2 16 38032
wc             2 17 25064
zombie         2 18 22288
hello          2 19 22432
console        3 20 0
$ mkdir lab
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2102
cat            2 3 23968
echo           2 4 22792
forktest       2 5 13128
grep           2 6 27280
init           2 7 23928
kill           2 8 22768
ln             2 9 22752
ls             2 10 26176
mkdir          2 11 22888
rm             2 12 22872
sh             2 13 41824
stressfs       2 14 23800
usertests      2 15 151208
grind          2 16 38032
wc             2 17 25064
zombie         2 18 22288
hello          2 19 22432
console        3 20 0
lab            1 21 32
$ QEMU: Terminated

user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系統程式/10-riscv/04-xv6os/xv6lab
$ make -f lab/lab.mk run
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 256M -smp 3 -nographic -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

xv6 kernel is booting

hart 1 starting
hart 2 starting
init: starting sh
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2102
cat            2 3 23968
echo           2 4 22792
forktest       2 5 13128
grep           2 6 27280
init           2 7 23928
kill           2 8 22768
ln             2 9 22752
ls             2 10 26176
mkdir          2 11 22888
rm             2 12 22872
sh             2 13 41824
stressfs       2 14 23800
usertests      2 15 151208
grind          2 16 38032
wc             2 17 25064
zombie         2 18 22288
hello          2 19 22432
console        3 20 0
lab            1 21 32
$ ./hello
Hello xv6!
$ QEMU: Terminated
```

## CP

```
user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系統程式/10-riscv/04-xv6os/xv6lab
$ make -f lab/lab.mk run
qemu-system-riscv64 -machine virt -bios none -kernel kernel/kernel -m 256M -smp 3 -nographic -drive file=fs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

xv6 kernel is booting

hart 1 starting
hart 2 starting
init: starting sh
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2102
cat            2 3 23968
echo           2 4 22792
forktest       2 5 13128
grep           2 6 27280
init           2 7 23928
kill           2 8 22768
ln             2 9 22752
ls             2 10 26176
mkdir          2 11 22888
rm             2 12 22872
sh             2 13 41824
stressfs       2 14 23800
usertests      2 15 151208
grind          2 16 38032
wc             2 17 25064
zombie         2 18 22288
hello          2 19 22432
cp             2 20 23464
console        3 21 0
$ cp README README.bak
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2102
cat            2 3 23968
echo           2 4 22792
forktest       2 5 13128
grep           2 6 27280
init           2 7 23928
kill           2 8 22768
ln             2 9 22752
ls             2 10 26176
mkdir          2 11 22888
rm             2 12 22872
sh             2 13 41824
stressfs       2 14 23800
usertests      2 15 151208
grind          2 16 38032
wc             2 17 25064
zombie         2 18 22288
hello          2 19 22432
cp             2 20 23464
console        3 21 0
README.bak     2 22 2102
$ grep README.bak xv6
grep: cannot open xv6
$ grep xv6 README.bak
xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
xv6 is inspired by John Lions's Commentary on UNIX 6th Edition 
(Peer
The code in the files that constitute xv6 is
(kaashoek,rtm@mit.edu). The main purpose of xv6 is as a teaching
$ QEMU: Terminated
```