

## mkfs

```
user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系
統程式/10-riscv/05-nix5/06-inode/fs5 (master)        
$ gcc mkfs.c fs.c -o mkfs

user@DESKTOP-96FRN6B MINGW64 /d/pmedia/陳鍾誠/課程/系
統程式/10-riscv/05-nix5/06-inode/fs5 (master)        
$ ./mkfs fs.img README.md fs.c
nmeta 46 (boot, super, log blocks 30 inode blocks 13, bitmap blocks 1) blocks 954 total 1000
balloc: first 55 blocks have been allocated
balloc: write bitmap block at sector 45
```
