# xv6 作業系統閱讀筆記 (2) 理解檔案系統結構 -- 從 cat.c 的 open() 指令開始追蹤

在前一篇中，我們透過 cat.c 的 `write(1, buf, n)` 指令追蹤了 xv6 的輸出原理，特別是 file.c:filewrite 是如何被檔案表 ftable 對應到 console 裝置而輸出到 uart 的那段程式流程。

在這一篇中，我們繼續追蹤 cat.c 中的 open() 與 read() ，以便理解 xv6 當中那個基於 inode 所串起來的《檔案系統結構》。

## cat.c

首先在讓我們回顧一下 cat.c 的原始碼！

user/cat.c

```cpp
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char buf[512];

void
cat(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    if (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  if(n < 0){
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0);
    exit(0);
  }

  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}

```

## open()

現在讓我們將焦點放在 `fd = open(argv[i], 0)` 這個指令上面。

對於 cat README 這樣的指令， `argv[i]` 就是 "README" 這個字串，於是該指令就是 `fd = open("README", 0)` 。

如同上一篇所示，open(), read() 等系統呼叫，被定義在 user/usys.S 這個檔案中，

user/usys.S

```s
.global open
open:
 li a7, SYS_open
 ecall
 ret
.global read
read:
 li a7, SYS_read
 ecall
 ret
.global write
write:
 li a7, SYS_write
 ecall
 ret
.global close
close:
 li a7, SYS_close
 ecall
 ret
```

透過 ecall 呼叫軟體中斷，會讓系統《自陷到核心模式》，並呼叫 syscalls 中的 sys_open() 函數。


```cpp
static uint64 (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
};

void
syscall(void)
{
  int num;
  struct proc *p = myproc();

  num = p->trapframe->a7;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    p->trapframe->a0 = syscalls[num]();
  } else {
    printf("%d %s: unknown sys call %d\n",
            p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}

```

sys_open() 定義在 sysfile.c 中，其功能為取得對應的 inode ，創建《檔案描述子》，放入檔案表 ftable 中，然後傳回其代號 fd。

kernel/sysfile.c

```cpp
uint64
sys_open(void)
{
  char path[MAXPATH];
  int fd, omode;
  struct file *f;
  struct inode *ip;
  int n;

  if((n = argstr(0, path, MAXPATH)) < 0 || argint(1, &omode) < 0)
    return -1;

  begin_op();

  if(omode & O_CREATE){ // 模式為創建新檔案
    ip = create(path, T_FILE, 0, 0); // 創建新的 inode
    if(ip == 0){
      end_op();
      return -1;
    }
  } else {
    if((ip = namei(path)) == 0){ // 取得該路徑名對應的 inode
      end_op();
      return -1;
    }
    ilock(ip); // 將該 inode 鎖定
    if(ip->type == T_DIR && omode != O_RDONLY){
      iunlockput(ip); // 若是目錄且要寫入，那就先解鎖該 inode
      end_op();
      return -1;
    }
  }

  if(ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)){
    iunlockput(ip); // 若是裝置，那也先解鎖該 inode
    end_op();
    return -1;
  }

  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){ // 分配檔案描述子
    if(f) // 若分配失敗就關閉之。
      fileclose(f);
    iunlockput(ip); // 分配成功的話，先解鎖該 inode
    end_op();
    return -1;
  }
  // 設定 FILE (struct file) 結構 (也就是檔案描述子)
  if(ip->type == T_DEVICE){
    f->type = FD_DEVICE;
    f->major = ip->major;
  } else {
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

  if((omode & O_TRUNC) && ip->type == T_FILE){
    itrunc(ip); // 若是檔案且模式為 O_TRUNC，就將該 inode 清空。
  }

  iunlock(ip);
  end_op();

  return fd; // 傳回檔案描述子代碼 (在檔案表 ftable 中的位置)
}

```

## read()

類似的，read() 函數也是轉交到 sys_read() 去處理 

kernel/sysfile.c

```cpp
uint64
sys_read(void)
{
  struct file *f;
  int n;
  uint64 p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argaddr(1, &p) < 0)
    return -1;
  return fileread(f, p, n);
}
```

sys_read() 又轉到 file.c 的 fileread() 中

```cpp
// Read from file f.
// addr is a user virtual address.
int
fileread(struct file *f, uint64 addr, int n)
{
  int r = 0;

  if(f->readable == 0)
    return -1;

  if(f->type == FD_PIPE){
    r = piperead(f->pipe, addr, n);
  } else if(f->type == FD_DEVICE){
    if(f->major < 0 || f->major >= NDEV || !devsw[f->major].read)
      return -1;
    r = devsw[f->major].read(1, addr, n);
  } else if(f->type == FD_INODE){
    ilock(f->ip);
    if((r = readi(f->ip, 1, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
  } else {
    panic("fileread");
  }

  return r;
}
```

如果 read() 的對象是檔案的話，那麼就會在剛剛 open() 時已經取得其 inode，於是上列程式中的`readi(f->ip, 1, addr, f->off, n)` 指令就會被執行。


readi() 會透過 inode (f->ip) 讀取對應的資料進來。

## inode, superblock 與檔案系統

問題是，inode 的結構到底長得怎麼樣，他是如何組織的呢？請看下圖：

![](img/Ext2-inode.gif)

在 xv6/UNIX 等系統中，inode 是用來指向區塊 BLOCK 的結構。

從更上層的角度看，硬碟 (或磁碟分割區，像是 ext2/ext3 ....) 是以區塊為單位，每個區塊大小都相同。

xv6 中整個硬碟被分成下列的區塊結構，其中寫 1 的代表單一區塊，寫 * 的代表占用多個區塊。

```
[ boot block | sb block | log | inode blocks | free bit map | data blocks ]
      1            1        *         *             1              *
```

每個區塊大小為 1k (1024 bytes)，這在 kernel/fs.h 當中有定義。

```
#define BSIZE 1024  // block size
```

xv6 把第一個區塊留給啟動程式，第二個區塊是超級區塊 (superblock)，超級區塊紀錄了總體的結構資訊。

```cpp
// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
struct superblock { // 超級區塊
  uint magic;        // Must be FSMAGIC      // 用來辨識的魔數:0x10203040
  uint size;         // Size of file system image (blocks) // 全部區塊數
  uint nblocks;      // Number of data blocks // 資料區塊數
  uint ninodes;      // Number of inodes.     // inodes 數量
  uint nlog;         // Number of log blocks  // 日誌 log 的區塊數
  uint logstart;     // Block number of first log block   // log 的首區塊
  uint inodestart;   // Block number of first inode block // inode 的首區塊
  uint bmapstart;    // Block number of first free map block // free bitmap 的首區塊
};

#define FSMAGIC 0x10203040
```

file.h 中定義了記憶體內的 inode 結構。

```cpp

// in-memory copy of an inode
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;          // inode has been read from disk?

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT+1];
};
```

但是存到硬碟時不需要存這麼多資訊，因此還有個 dinode 結構在 fs.h 當中：

```cpp
#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEVICE only)
  short minor;          // Minor device number (T_DEVICE only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+1];   // Data block addresses
};
```

更詳細的 inode 與超級區塊資訊請參看鳥哥的文章：

http://linux.vbird.org/linux_basic/0230filesystem.php#harddisk-inode

ext2 檔案系統: http://linux.vbird.org/linux_basic/0230filesystem/ext2_filesystem.jpg

inode: http://linux.vbird.org/linux_basic/0230filesystem/inode.jpg


讓我們回到剛剛的 `readi(f->ip, 1, addr, f->off, n)` 函數繼續追蹤下去。

kernel/fs.c

```cpp
// Read data from inode.
// Caller must hold ip->lock.
// If user_dst==1, then dst is a user virtual address;
// otherwise, dst is a kernel address.
int
readi(struct inode *ip, int user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  struct buf *bp;

  if(off > ip->size || off + n < off)
    return 0;
  if(off + n > ip->size)
    n = ip->size - off;

  for(tot=0; tot<n; tot+=m, off+=m, dst+=m){
    bp = bread(ip->dev, bmap(ip, off/BSIZE));
    m = min(n - tot, BSIZE - off%BSIZE);
    if(either_copyout(user_dst, dst, bp->data + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      break;
    }
    brelse(bp);
  }
  return tot;
}
```

## 緩衝模組 bio.c

readi() 會呼叫 bio.c 中的 bread() 函數讀取對應區塊進來，然後放入緩衝區內。

bio.c 模組，主要是為基礎的檔案讀寫函數 virtio_disk.c 模組，加上緩衝的功能，以下的 struct bcache 是其緩衝結構。

kernel/bio.c

```cpp
struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head;
} bcache;
```

在 bread() 函數中，若資料已經在緩中區內，則可以用 bget() 直接取得，若不在緩衝區內，則必須呼叫《虛擬硬碟存取函數》 `virtio_disk_rw(b, 0)` 去讀取對應的區塊 (第 blockno 個區塊)。

```cpp
// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}
```

## 虛擬硬碟存取 virtio.h/virtio_disk.c

virtio_disk_rw() 是 xv6 用來存取 qemu 虛擬硬碟的函數，透過 virtio 這個存取協定，可以讓 qemu 在執行 xv6 時，能夠比較有效率，而不需要去模擬 CPU 在存取 DMA 裝置時的所有動作。

```cpp
void
virtio_disk_rw(struct buf *b, int write)
{
  uint64 sector = b->blockno * (BSIZE / 512);

  acquire(&disk.vdisk_lock);

  // the spec's Section 5.2 says that legacy block operations use
  // three descriptors: one for type/reserved/sector, one for the
  // data, one for a 1-byte status result.

  // allocate the three descriptors.
  int idx[3];
  while(1){
    if(alloc3_desc(idx) == 0) {
      break;
    }
    sleep(&disk.free[0], &disk.vdisk_lock);
  }

  // format the three descriptors.
  // qemu's virtio-blk.c reads them.

  struct virtio_blk_req *buf0 = &disk.ops[idx[0]];

  if(write)
    buf0->type = VIRTIO_BLK_T_OUT; // write the disk
  else
    buf0->type = VIRTIO_BLK_T_IN; // read the disk
  buf0->reserved = 0;
  buf0->sector = sector;

  disk.desc[idx[0]].addr = (uint64) buf0;
  disk.desc[idx[0]].len = sizeof(struct virtio_blk_req);
  disk.desc[idx[0]].flags = VRING_DESC_F_NEXT;
  disk.desc[idx[0]].next = idx[1];

  disk.desc[idx[1]].addr = (uint64) b->data;
  disk.desc[idx[1]].len = BSIZE;
  if(write)
    disk.desc[idx[1]].flags = 0; // device reads b->data
  else
    disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
  disk.desc[idx[1]].flags |= VRING_DESC_F_NEXT;
  disk.desc[idx[1]].next = idx[2];

  disk.info[idx[0]].status = 0xff; // device writes 0 on success
  disk.desc[idx[2]].addr = (uint64) &disk.info[idx[0]].status;
  disk.desc[idx[2]].len = 1;
  disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
  disk.desc[idx[2]].next = 0;

  // record struct buf for virtio_disk_intr().
  b->disk = 1;
  disk.info[idx[0]].b = b;

  // tell the device the first index in our chain of descriptors.
  disk.avail->ring[disk.avail->idx % NUM] = idx[0];

  __sync_synchronize();

  // tell the device another avail ring entry is available.
  disk.avail->idx += 1; // not % NUM ...

  __sync_synchronize();

  *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number

  // Wait for virtio_disk_intr() to say request has finished.
  while(b->disk == 1) {
    sleep(b, &disk.vdisk_lock);
  }

  disk.info[idx[0]].b = 0;
  free_chain(idx[0]);

  release(&disk.vdisk_lock);
}
```

## 整體的檔案系統結構


整體來看，xv6 的檔案系統有七個層次，如下表所示：


```
7. File Descriptor (file.c/FILE)       // ftable.file 是 struct file (FILE) 的陣列
                                       // fd 就是其索引值

6. Pathname (fs.c/namei+dirlookup)     // 透過 namei() 查找對應某路徑的 inode

5. Directory (fs.h/struct dirent)      // 目錄以 dirent 形式存在 inode 中

4. Inode (fs.c/struct inode/dinode)    // 一個檔案對應一個 inode

3. Logging (log.c)                     // 日誌，防止寫到一半出現狀況。

2. Buffer cache (bio.c)                // 區塊讀取函數 bread(dev, i)/bwrite(dev, i) 
                                       // 有緩衝功能。

1. Disk (virtio_disk.c)                // 透過 virtio 協定請求讀寫硬碟
```

## 結語

這樣，透過 cat.c 中的 open()/read() 函數呼叫，我們差不多把 xv6 檔案系統的結構與對應的函數追蹤了一遍。

透過這樣的追蹤，我想讀者應該大致掌握了 xv6 檔案系統的面貌，若要進一步閱讀 xv6 原始碼時，應該會比較有頭緒才對！



