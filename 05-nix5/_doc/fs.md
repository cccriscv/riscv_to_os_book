# file system

* https://github.com/jserv/simplefs
* [第八章、Linux 磁碟與檔案系統管理(鳥哥)](http://linux.vbird.org/linux_basic/0230filesystem/0230filesystem-centos5.php) (紙本 8-8頁)
* [Linux 核心設計: 檔案系統概念及實作手法 (jserv)](https://hackmd.io/@sysprog/linux-file-system)
* [Linux核心學習筆記（一） 虛擬檔案系統VFS](https://www.itread01.com/content/1550240301.html)

```cpp
// File system implementation.  Five layers:
//   + Blocks: allocator for raw disk blocks.
//   + Log: crash recovery for multi-step updates.
//   + Files: inode allocator, reading, writing, metadata.
//   + Directories: inode with special contents (list of other inodes!)
//   + Names: paths like /usr/rtm/xv6/fs.c for convenient naming.

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
struct superblock {
  uint magic;        // Must be FSMAGIC
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint inodestart;   // Block number of first inode block
  uint bmapstart;    // Block number of first free map block
};

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

// Directory is a file containing a sequence of dirent structures.
struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe; // FD_PIPE
  struct inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

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

## inode

* [理解inode](http://www.ruanyifeng.com/blog/2011/12/inode.html)
* [詳解Linux Inode](https://iter01.com/581787.html)

* https://zh.wikipedia.org/wiki/Inode

檔案系統建立（格式化）時，就把儲存區域分為兩大連續的儲存區域。一個用來儲存檔案系統物件的元資訊資料，這是由inode組成的表，每個inode預設是256位元組或者128位元組。另一個用來儲存「檔案系統物件」的內容資料，劃分為512位元組的磁區，以及由8個磁區組成的4K位元組的塊。塊是讀寫時的基本單位。一個檔案系統的inode的總數一般情況下是固定的。這限制了該檔案系統所能儲存的檔案系統物件的總數目。典型的實現下，所有inode占用了檔案系統1%左右的儲存容量。

檔案系統中每個「檔案系統物件」對應一個「inode」資料，並用一個整數值來辨識。這個整數常被稱為inode號碼（「i-number」或「inode number」）。由於檔案系統的inode表的儲存位置、總條目數量都是固定的，因此可以用inode號碼去索引尋找inode表。

Inode儲存了檔案系統物件的一些元資訊，如所有者、存取權限（讀、寫、執行）、類型（是檔案還是目錄）、內容修改時間、inode修改時間、上次存取時間、對應的檔案系統儲存塊的位址，等等。知道了1個檔案的inode號碼，就可以在inode元資料中查出檔案內容資料的儲存位址。

檔名與目錄名是「檔案系統物件」便於使用的別名。一個檔案系統物件可以有多個別名，但只能有一個inode，並用這個inode來索引檔案系統物件的儲存位置。

1. inode不包含檔名或目錄名的字串，只包含檔案或目錄的「元資訊」。
2. Unix的檔案系統的目錄也是一種檔案。打開目錄，實際上就是讀取「目錄檔案」。目錄檔案的結構是一系列目錄項（dirent）的列表。每個目錄項，由兩部分組成：所包含檔案或目錄的名字，以及該檔案或目錄名對應的inode號碼。
3. 檔案系統中的一個檔案是指存放在其所屬目錄的「目錄檔案」中的一個目錄項，其所對應的inode的類別為「檔案」；檔案系統中的一個目錄是指存放在其「父目錄檔案」中的一個目錄項，其所對應的inode的類別為「目錄」。可見，多個「檔案」可以對應同一個inode；多個「目錄」可以對應同一個inode。
4. 檔案系統中如果兩個檔案或者兩個目錄具有相同的inode號碼，那麼就稱它們是「硬連結」關係。實際上都是這個inode的別名。換句話說，一個inode對應的所有檔案（或目錄）中的每一個，都對應著檔案系統某個「目錄檔案」中唯一的一個目錄項。
5. 建立一個目錄時，實際做了3件事：在其「父目錄檔案」中增加一個條目；分配一個inode；再分配一個儲存塊，用來儲存當前被建立目錄包含的檔案與子目錄。被建立的「目錄檔案」中自動生成兩個子目錄的條目，名稱分別是：「.」和「..」。前者與該目錄具有相同的inode號碼，因此是該目錄的一個「硬連結」。後者的inode號碼就是該目錄的父目錄的inode號碼。所以，任何一個目錄的"硬連結"總數，總是等於它的子目錄總數（含隱藏目錄）加2。即每個「子目錄檔案」中的「..」條目，加上它自身的「目錄檔案」中的「.」條目，再加上「父目錄檔案」中的對應該目錄的條目。
6. 通過檔名打開檔案，實際上是分成三步實現：首先，作業系統找到這個檔名對應的inode號碼；其次，通過inode號碼，取得inode資訊；最後，根據inode資訊，找到檔案資料所在的block，讀出資料。

## POSIX inode

POSIX標準強制規範了檔案系統的行為。每個「檔案系統物件」必須具有：

1. 以位元組為單位表示的檔案大小。
2. 裝置ID，標識容納該檔案的裝置。
3. 檔案所有者的User ID。
4. 檔案的Group ID
5. 檔案的模式（mode），確定了檔案的類型，以及它的所有者、它的group、其它使用者存取此檔案的權限。
6. 額外的系統與使用者標誌（flag），用來保護該檔案。
7. 3個時間戳，記錄了inode自身被修改（ctime, inode change time）、檔案內容被修改（mtime, modification time）、最後一次存取（atime, access time）的時間。
8. 1個連結數，表示有多少個硬連結指向此inode。
9. 到檔案系統儲存位置的指標。通常是1K位元組或者2K位元組的儲存容量為基本單位。
10. 使用stat系統呼叫可以查詢一個檔案的inode號碼及一些元資訊。
