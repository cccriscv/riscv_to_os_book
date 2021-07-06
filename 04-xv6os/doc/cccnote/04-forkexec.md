# xv6 作業系統閱讀筆記 (4) 使用者行程 -- 從 init 開始追蹤 fork+exec

核心啟動進入 scheduler() 之後，就會選取行程來執行，而第一個被選取的行程，是 init 程式。

我們可以看到，在 xv6 作業系統的主程式 main.c 當中，核心是先執行了 userinit() 之後，才在最後呼叫scheduler() 的。

所以在 scheduler() 開始執行前， init 程式就被載入為第一個行程了。

kernel/main.c

```cpp
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){  // 第 0 的 hart 用來跑核心
    consoleinit();   // 準備好 console
    printfinit();    // 準備好 printf
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator // 準備好實體分頁
    kvminit();       // create kernel page table // 準備好核心分頁表
    kvminithart();   // turn on paging // 啟動分頁表
    procinit();      // process table // 準備好行程表
    trapinit();      // trap vectors  // 設定好 trap 中斷
    trapinithart();  // install kernel trap vector  // 安裝核心的中斷向量
    plicinit();      // set up interrupt controller // 設定中斷控制器
    plicinithart();  // ask PLIC for device interrupts // 設定裝置中斷
    binit();         // buffer cache // 檔案系統: 緩衝快取
    iinit();         // inode cache  // 檔案系統: inode快取
    fileinit();      // file table   // 檔案系統: 設置檔案表
    virtio_disk_init(); // emulated hard disk // 檔案系統: 設置 virtio 虛擬硬碟
    userinit();      // first user process // 啟動第一個使用者行程 init
    __sync_synchronize();
    started = 1;     // 啟動已完成
  } else {  // 其他的 hart 用來跑一般程式
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging // 啟動分頁表
    trapinithart();   // install kernel trap vector  // 安裝核心的中斷向量
    plicinithart();   // ask PLIC for device interrupts // 設定裝置中斷
  }

  scheduler(); // 進入排程系統 (無窮迴圈)
}
```

main() 中呼叫的 userinit() 函數定義如下：

kernel/proc.c

```cpp
// Set up first user process.
void
userinit(void)
{
  struct proc *p;

  p = allocproc();
  initproc = p;
  
  // allocate one user page and copy init's instructions
  // and data into it.
  uvminit(p->pagetable, initcode, sizeof(initcode));
  p->sz = PGSIZE;

  // prepare for the very first "return" from kernel to user.
  p->trapframe->epc = 0;      // user program counter
  p->trapframe->sp = PGSIZE;  // user stack pointer

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}
```

而其中的 initcode 這個變數，則是一段奇特的十六進位碼如下：

```cpp
// a user program that calls exec("/init")
// od -t xC initcode
uchar initcode[] = {
  0x17, 0x05, 0x00, 0x00, 0x13, 0x05, 0x45, 0x02,
  0x97, 0x05, 0x00, 0x00, 0x93, 0x85, 0x35, 0x02,
  0x93, 0x08, 0x70, 0x00, 0x73, 0x00, 0x00, 0x00,
  0x93, 0x08, 0x20, 0x00, 0x73, 0x00, 0x00, 0x00,
  0xef, 0xf0, 0x9f, 0xff, 0x2f, 0x69, 0x6e, 0x69,
  0x74, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};
```

上面那堆 `uchar initcode[] = { 0x17, 0x05, 0x00 ...` 到底是甚麼呢？

其實就是下列組合語言的機器碼，被直接寫在 initcode[] 陣列裡面了。

user/initcode.S

```s
# Initial process that execs /init.
# This code runs in user space.

#include "syscall.h"

# exec(init, argv)
.globl start
start:
        la a0, init
        la a1, argv
        li a7, SYS_exec
        ecall

# for(;;) exit();
exit:
        li a7, SYS_exit
        ecall
        jal exit

# char init[] = "/init\0";
init:
  .string "/init\0"

# char *argv[] = { init, 0 };
.p2align 2
argv:
  .long init
  .long 0
```

就是透過上面這個奇怪的方式， xv6 把 initcode 強行塞入到分頁表中，然後呼叫 SYS_exec 把 init 這個執行檔載入到記憶體裡，成為第一個被執行的使用者 (user mode) 程式。

## init.c

終於我們可以仔細看看 init 執行檔的原始碼了，這個 user mode 天字第一號的 init.c 程式了。

user/init.c

```cpp
// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){ // 開啟 console 裝置
    mknod("console", CONSOLE, 0); // 若開啟失敗，則創建該裝置為第 0 號檔案 (stdin)。
    open("console", O_RDWR);
  }
  dup(0);  // stdout // 複製 stdin 到 stdout
  dup(0);  // stderr // 複製 stdin 到 stderr

  for(;;){
    printf("init: starting sh\n");
    pid = fork(); // fork 出一個子行程
    if(pid < 0){
      printf("init: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      exec("sh", argv); // 將該子行程替換為 sh (shell)
      printf("init: exec sh failed\n");
      exit(1);
    }

    for(;;){ // 等候 shell 行程退出，之後會再啟動另一個 shell
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *) 0);
      if(wpid == pid){
        // the shell exited; restart it.
        break;
      } else if(wpid < 0){
        printf("init: wait returned an error\n");
        exit(1);
      } else {
        // it was a parentless process; do nothing.
      }
    }
  }
}
```

這個 init.c 程式才是本文想要討論的主角，特別是裡面的 fork() 與 exec()

熟悉 linux 系統程式的人應該都知道， fork() 函數是讓該行程 (process)《一分為二》，原本的程式還在，但是多出了一個 child。(也就是 parent 還在，但生出了一個 child)。

所以當 pid=fork() 被執行後，若 fork() 的傳回值 pid == 0，就代表該行程為 child，於是 init.c 讓子行程載入 sh (shell)

```cpp
    pid = fork(); // fork 出一個子行程
    // ...
    if(pid == 0){
      exec("sh", argv); // 將該子行程替換為 sh (shell)
      printf("init: exec sh failed\n");
      exit(1);
    }
```

所以 init 的主要任務就是載入 shell，而且讓 shell 不斷執行 (就算跳出後也繼續創出下一個 shell 執行)。

本文所要探討的重點就是， fork() 和 exec() 兩個函數到底是怎麼實作的，然後藉此觀察整個 xv6 的行程管理系統，到底是怎麼運作的。

## fork

fork() 函數定義在 proc.c 裡面，其中的 `np = allocproc()` 會分配一個新的子行程，然後透過 `uvmcopy(p->pagetable, np->pagetable, p->sz)` 這行指令複製其分頁表給子行程。

kernel/proc.c

```cpp
// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *p = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){ // 分配新的子行程
    return -1;
  }

  // Copy user memory from parent to child.
  if(uvmcopy(p->pagetable, np->pagetable, p->sz) < 0){ // 將分頁表複製給子行程。
    freeproc(np);
    release(&np->lock);
    return -1;
  }
  np->sz = p->sz; // 子行程大小和父行程相同

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe); // 暫存器也相同

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0; // 子行程的 fork 傳回值應為 0 (注意，父行程傳回值沒修改)

  // increment reference counts on open file descriptors.
  for(i = 0; i < NOFILE; i++)
    if(p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]); // 複製檔案表
  np->cwd = idup(p->cwd); // 複製 cwd 目前目錄 

  safestrcpy(np->name, p->name, sizeof(p->name)); // 複製名稱

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE; // 設定子行程為 RUNNABLE
  release(&np->lock);

  return pid;
}

```

透過 fork() 函數，我們可以創建一個和父行程幾乎一模一樣的子行程。

然後再透過 exec()，我們就可以載入任何想載入的執行檔 (ELF 格式)了，讓我們看看 exec() 的原始碼。

kernel/exec.c

```cpp
#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "elf.h"

static int loadseg(pde_t *pgdir, uint64 addr, struct inode *ip, uint offset, uint sz);

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG+1], stackbase;
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  struct proc *p = myproc();

  begin_op();

  if((ip = namei(path)) == 0){ // 取得 path ELF 檔對應的 inode ptr (ip)
    end_op();
    return -1;
  }
  ilock(ip);

  // Check ELF header
  if(readi(ip, 0, (uint64)&elf, 0, sizeof(elf)) != sizeof(elf)) // 讀取該 inode
    goto bad;
  if(elf.magic != ELF_MAGIC) // 若不是 ELF 則失敗
    goto bad;

  if((pagetable = proc_pagetable(p)) == 0) // 分配頁表
    goto bad;

  // Load program into memory.
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    uint64 sz1;
    if((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz)) == 0) // 為每個ELF段分配內存
      goto bad;
    sz = sz1;
    if(ph.vaddr % PGSIZE != 0)
      goto bad;
    if(loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0) // 把每個段加載到內存中 (loadseg用walkaddr找到分配的內存的物理地址，在這個地址上寫入ELF段的每一頁，用readi從文件中讀取)
      goto bad; 
  }
  iunlockput(ip);
  end_op();
  ip = 0;

  p = myproc();
  uint64 oldsz = p->sz;

  // Allocate two pages at the next page boundary. 為何分配兩頁？第二個是堆疊，那第一個幹嘛用？
  // Use the second as the user stack.  答：第一個是不可訪問頁，當堆疊溢位時會觸發錯誤中斷。
  sz = PGROUNDUP(sz);
  uint64 sz1;
  if((sz1 = uvmalloc(pagetable, sz, sz + 2*PGSIZE)) == 0)
    goto bad;
  sz = sz1;
  uvmclear(pagetable, sz-2*PGSIZE);
  sp = sz;
  stackbase = sp - PGSIZE;

  // Push argument strings, prepare rest of stack in ustack. 在堆疊中推入 argv 字串
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    if(sp < stackbase)
      goto bad;
    if(copyout(pagetable, sp, argv[argc], strlen(argv[argc]) + 1) < 0) // 複製失敗就離開
      goto bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push the array of argv[] pointers. 推入 argv 的指標
  sp -= (argc+1) * sizeof(uint64);
  sp -= sp % 16;
  if(sp < stackbase)
    goto bad;
  if(copyout(pagetable, sp, (char *)ustack, (argc+1)*sizeof(uint64)) < 0)
    goto bad;

  // arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp; // 設定 a1=argv

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(p->name, last, sizeof(p->name));
    
  // Commit to the user image.
  oldpagetable = p->pagetable; // 註：oldpagetable 指向 fork 時的 process，現在已經換成新 process 了。
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry;  // initial program counter = main (進入點為 main)
  p->trapframe->sp = sp; // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  return argc; // this ends up in a0, the first argument to main(argc, argv)

 bad:
  if(pagetable)
    proc_freepagetable(pagetable, sz);
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}

```

exec() 函數的程式碼很長，首先是用 `ip = namei(path)` 取得 ELF 格式執行檔的 inode，接著用 `readi(ip, 0, (uint64)&elf, 0, sizeof(elf)` 將 《ELF 檔頭》讀進來，放在 elf 變數裏。

接著是把每個分段逐一載入到記憶體中，以組合《程式段、資料段、BSS段》成為完整的機器碼。

```cpp
  // Load program into memory.
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    uint64 sz1;
    if((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz)) == 0) // 為每個ELF段分配內存
      goto bad;
    sz = sz1;
    if(ph.vaddr % PGSIZE != 0)
      goto bad;
    if(loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0) // 把每個段加載到內存中 (loadseg用walkaddr找到分配的內存的物理地址，在這個地址上寫入ELF段的每一頁，用readi從文件中讀取)
      goto bad; 
  }
```

剩下的就是建構 argv 等參數，還有新行程的分頁表等等，這樣就完成了換掉該行程執行檔的功能，於是行程就《改頭換面》了。

所以 init.c 當中的下列段落，才能創建出 shell (sh) 行程。

```cpp
    pid = fork(); // fork 出一個子行程
    // ...
    if(pid == 0){
      exec("sh", argv); // 將該子行程替換為 sh (shell)
      printf("init: exec sh failed\n");
      exit(1);
    }
```

shell 會接受使用者的輸入命令，並且執行該命令，而這些動作，則又是靠一連串的 fork() + exec() 所達成的，以下是 sh 的原始程式碼之重要部分。

```cpp

int
main(void)
{
  static char buf[100];
  int fd;

  // Ensure that three file descriptors are open.
  while((fd = open("console", O_RDWR)) >= 0){
    if(fd >= 3){
      close(fd);
      break;
    }
  }

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      // Chdir must be called by the parent, not the child.
      buf[strlen(buf)-1] = 0;  // chop \n
      if(chdir(buf+3) < 0)
        fprintf(2, "cannot cd %s\n", buf+3);
      continue;
    }
    if(fork1() == 0)
      runcmd(parsecmd(buf));
    wait(0);
  }
  exit(0);
}

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
  int p[2];
  struct backcmd *bcmd;
  struct execcmd *ecmd;
  struct listcmd *lcmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    exit(1);

  switch(cmd->type){
  default:
    panic("runcmd");

  case EXEC:
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    break;

  case REDIR:
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    if(open(rcmd->file, rcmd->mode) < 0){
      fprintf(2, "open %s failed\n", rcmd->file);
      exit(1);
    }
    runcmd(rcmd->cmd);
    break;

  case LIST:
    lcmd = (struct listcmd*)cmd;
    if(fork1() == 0)
      runcmd(lcmd->left);
    wait(0);
    runcmd(lcmd->right);
    break;

  case PIPE:
    pcmd = (struct pipecmd*)cmd;
    if(pipe(p) < 0)
      panic("pipe");
    if(fork1() == 0){
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    if(fork1() == 0){
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    close(p[0]);
    close(p[1]);
    wait(0);
    wait(0);
    break;

  case BACK:
    bcmd = (struct backcmd*)cmd;
    if(fork1() == 0)
      runcmd(bcmd->cmd);
    break;
  }
  exit(0);
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}
```

其中的 fork1() 呼叫了 fork()，而 runcmd() 的下列段落則用了 exec() 去載入執行某 ELF 執行檔。

```cpp
  case EXEC:
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit(1);
    exec(ecmd->argv[0], ecmd->argv);
    fprintf(2, "exec %s failed\n", ecmd->argv[0]);
    break;
```

於是透過 xv6 提供的 fork(), exec() 等函數，我們所寫的程式都可以被編譯為 ELF 執行檔之後，由 fork() + exec() 來載入執行了！

## 結語

本文透過追蹤 init, fork() 與 exec() 等程式碼，應該能讓讀者理解 xv6 是如何載入 ELF 執行檔並執行的那些機制。

這些機制搭配前文關於檔案系統的 open(), read(), write(), close() 等功能，就形成了 xv6 作業系統的《經線與緯線》，這兩個脈絡串起了整個 UNIX 使用者程式的基本架構。

當然，背後還有像《分頁表，中斷機制》等骨架，才能形成一個完整的 xv6 作業系統！

這也是我們接下來的文章所要探索的重點！
