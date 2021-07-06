# xv6 作業系統閱讀筆記 (5) 中斷 Interrupt (Trap)

## main.c

在 xv6 這類的多工 (多行程) 系統中，中斷是《打斷又串起》所有行程執行順序的關鍵。

xv6 啟動 (boot) 後會進入 main() 函數，然後開始設定整個作業系統必須的環境與功能。

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

main() 當中的下列段落就是在設定中斷處理函數

```cpp
    trapinit();      // trap vectors  // 設定好 trap 中斷
    trapinithart();  // install kernel trap vector  // 安裝核心的中斷向量
    plicinit();      // set up interrupt controller // 設定中斷控制器
    plicinithart();  // ask PLIC for device interrupts // 設定裝置中斷
```

讓我們一個一個追蹤進去！

```cpp
struct spinlock tickslock;
uint ticks;
//...
void
trapinit(void)
{
  initlock(&tickslock, "time");
}

// ...
void
clockintr() // 時間中斷
{
  acquire(&tickslock);
  ticks++;
  wakeup(&ticks);
  release(&tickslock);
}
```

trapinit() 只是設定了時間中斷的鎖 tickslock，於是在 clockintr() 中我們可以透過 acquire() 和 release() 該鎖，避免修改 ticks 變數時會有競爭情況發生。

其中 wakeup(&ticks) 會喚醒等待 &ticks 這個 channel 的行程，讓這些行程恢復執行狀態。

(註：我想這應該是喚醒那些之前被時間中斷切換出去的行程，讓他們有機會恢復執行)

kernel/proc.c

```cpp
// Wake up all processes sleeping on chan.
// Must be called without any p->lock.
void
wakeup(void *chan)
{
  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++) {
    if(p != myproc()){
      acquire(&p->lock);
      if(p->state == SLEEPING && p->chan == chan) {
        p->state = RUNNABLE;
      }
      release(&p->lock);
    }
  }
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  // Must acquire p->lock in order to
  // change p->state and then call sched.
  // Once we hold p->lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup locks p->lock),
  // so it's okay to release lk.

  acquire(&p->lock);  //DOC: sleeplock1
  release(lk);

  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  release(&p->lock);
  acquire(lk);
}
```

接著 trapinit() 之後的 trapinithart() ，則是設定了核心的中斷向量 kernelvec，於是中斷機制開始真正被啟動。

```cpp
// set up to take exceptions and traps while in the kernel.
void
trapinithart(void)
{
  w_stvec((uint64)kernelvec); // 設定核心的中斷函數為 kernelvec
}
```

kernelvec 主要是將暫存器先保存在核心堆疊裏，然後就呼叫 C 語言的 kerneltrap() 函數。

kernel/kernelvec.S

```s
	#
        # interrupts and exceptions while in supervisor
        # mode come here.
        #
        # push all registers, call kerneltrap(), restore, return.
        #
.globl kerneltrap
.globl kernelvec
.align 4
kernelvec:
        // make room to save registers. # 分配堆疊以儲存暫存器
        addi sp, sp, -256

        // save the registers.
        sd ra, 0(sp)
        sd sp, 8(sp)
        sd gp, 16(sp)
        sd tp, 24(sp)
        sd t0, 32(sp)
        sd t1, 40(sp)
        sd t2, 48(sp)
        sd s0, 56(sp)
        sd s1, 64(sp)
        sd a0, 72(sp)
        sd a1, 80(sp)
        sd a2, 88(sp)
        sd a3, 96(sp)
        sd a4, 104(sp)
        sd a5, 112(sp)
        sd a6, 120(sp)
        sd a7, 128(sp)
        sd s2, 136(sp)
        sd s3, 144(sp)
        sd s4, 152(sp)
        sd s5, 160(sp)
        sd s6, 168(sp)
        sd s7, 176(sp)
        sd s8, 184(sp)
        sd s9, 192(sp)
        sd s10, 200(sp)
        sd s11, 208(sp)
        sd t3, 216(sp)
        sd t4, 224(sp)
        sd t5, 232(sp)
        sd t6, 240(sp)

	// call the C trap handler in trap.c # 呼叫 C 語言的 kerneltrap() 函數。
        call kerneltrap

        // restore registers. // 恢復暫存器
        ld ra, 0(sp)
        ld sp, 8(sp)
        ld gp, 16(sp)
        // not this, in case we moved CPUs: ld tp, 24(sp)
        ld t0, 32(sp)
        ld t1, 40(sp)
        ld t2, 48(sp)
        ld s0, 56(sp)
        ld s1, 64(sp)
        ld a0, 72(sp)
        ld a1, 80(sp)
        ld a2, 88(sp)
        ld a3, 96(sp)
        ld a4, 104(sp)
        ld a5, 112(sp)
        ld a6, 120(sp)
        ld a7, 128(sp)
        ld s2, 136(sp)
        ld s3, 144(sp)
        ld s4, 152(sp)
        ld s5, 160(sp)
        ld s6, 168(sp)
        ld s7, 176(sp)
        ld s8, 184(sp)
        ld s9, 192(sp)
        ld s10, 200(sp)
        ld s11, 208(sp)
        ld t3, 216(sp)
        ld t4, 224(sp)
        ld t5, 232(sp)
        ld t6, 240(sp)

        addi sp, sp, 256 # 恢復堆疊指標

        // return to whatever we were doing in the kernel.
        sret

```

於是焦點又轉回 trap.c 裏的 kerneltrap() 函數。

```cpp
// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void 
kerneltrap()
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();
  
  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if((which_dev = devintr()) == 0){ // 1. 裝置中斷
    printf("scause %p\n", scause);
    printf("sepc=%p stval=%p\n", r_sepc(), r_stval());
    panic("kerneltrap");
  }

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2 && myproc() != 0 && myproc()->state == RUNNING)  // 1. 時間中斷，禮讓給別人
    yield(); // 註：時間中斷對 user mode 與 kernel mode 都是有效的，都必須禮讓給別人。

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}
```

kerneltrap() 會判斷中斷類型並做出如下反應

1. 裝置中斷：呼叫 devintr() 處理之
2. 時間中斷：呼叫 yield() 將處理器讓給下一個行程。

對於裝置中斷，devintr() 會判斷是哪個裝置 (UART鍵盤/VIRTIO磁碟/TIMER定時器)發生了中斷，然後呼叫對應的處理函數。

1. UART: uartintr()
2. VIRTIO: virtio_disk_intr()
3. TIMER: clockintr()

kernel/trap.c
```cpp
// ...
// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr() // 裝置中斷
{
  uint64 scause = r_scause();

  if((scause & 0x8000000000000000L) &&
     (scause & 0xff) == 9){ // 1. 硬體外部中斷
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    int irq = plic_claim();

    if(irq == UART0_IRQ){ // UART 中斷
      uartintr();
    } else if(irq == VIRTIO0_IRQ){ // 磁碟中斷
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if(irq)
      plic_complete(irq); // 可以允許再次中斷了。

    return 1;
  } else if(scause == 0x8000000000000001L){ // 2. 時間中斷
    // software interrupt from a machine-mode timer interrupt,
    // forwarded by timervec in kernelvec.S.

    if(cpuid() == 0){
      clockintr();
    }
    
    // acknowledge the software interrupt by clearing
    // the SSIP bit in sip.
    w_sip(r_sip() & ~2); // 註：sip 是 software interrupt-pending

    return 2;
  } else {
    return 0;
  }
}
```

這些對應裝置的中斷處理函數，會讀取裝置傳來的資料，然後啟動對應處理函數。

1. UART: uartintr()
2. VIRTIO: virtio_disk_intr()
3. TIMER: clockintr()

```cpp
// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from trap.c.
void
uartintr(void)
{
  // read and process incoming characters.
  while(1){
    int c = uartgetc();
    if(c == -1)
      break;
    consoleintr(c);
  }

  // send buffered characters.
  acquire(&uart_tx_lock);
  uartstart();
  release(&uart_tx_lock);
}

```

像是 uartintr() 會啟動 consoleintr(c)，去處理該字元的詮釋動作，例如碰到 `\b` (backspace) 時該退一格等事項，並且透過 `cons.buf[cons.e++ % INPUT_BUF] = c` 這行將處理後的字元放進輸入行緩衝區。

kernel/console.c

```cpp
#define BACKSPACE 0x100
#define C(x)  ((x)-'@')  // Control-x

consoleintr(int c)
{
  acquire(&cons.lock);

  switch(c){
  case C('P'):  // Print process list.
    procdump();
    break;
  case C('U'):  // Kill line.
    while(cons.e != cons.w &&
          cons.buf[(cons.e-1) % INPUT_BUF] != '\n'){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  case C('H'): // Backspace
  case '\x7f':
    if(cons.e != cons.w){
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  default:
    if(c != 0 && cons.e-cons.r < INPUT_BUF){
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % INPUT_BUF] = c;

      if(c == '\n' || c == C('D') || cons.e == cons.r+INPUT_BUF){
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    break;
  }
  
  release(&cons.lock);
}

// ...
//
// send one character to the uart.
// called by printf, and to echo input characters,
// but not from write().
//
void
consputc(int c)
{
  if(c == BACKSPACE){
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b'); uartputc_sync(' '); uartputc_sync('\b');
  } else {
    uartputc_sync(c);
  }
}
```

kernel/uart.c

```cpp
// alternate version of uartputc() that doesn't 
// use interrupts, for use by kernel printf() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
void
uartputc_sync(int c)
{
  push_off();

  if(panicked){
    for(;;)
      ;
  }

  // wait for Transmit Holding Empty to be set in LSR.
  while((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  pop_off();
}
```

磁碟的中斷處理函數 virtio_disk_intr() 則定義在 virtio_disk.c 當中，主要是用來喚醒那些磁碟存取已經完成的行程。

kernel/virtio_disk.c

```cpp
void
virtio_disk_intr()
{
  acquire(&disk.vdisk_lock);

  // the device won't raise another interrupt until we tell it
  // we've seen this interrupt, which the following line does.
  // this may race with the device writing new entries to
  // the "used" ring, in which case we may process the new
  // completion entries in this interrupt, and have nothing to do
  // in the next interrupt, which is harmless.
  *R(VIRTIO_MMIO_INTERRUPT_ACK) = *R(VIRTIO_MMIO_INTERRUPT_STATUS) & 0x3;

  __sync_synchronize();

  // the device increments disk.used->idx when it
  // adds an entry to the used ring.

  while(disk.used_idx != disk.used->idx){
    __sync_synchronize();
    int id = disk.used->ring[disk.used_idx % NUM].id;

    if(disk.info[id].status != 0)
      panic("virtio_disk_intr status");

    struct buf *b = disk.info[id].b;
    b->disk = 0;   // disk is done with buf
    wakeup(b);

    disk.used_idx += 1;
  }

  release(&disk.vdisk_lock);
}

```

至於軟體中斷和時間中斷，我們已經在前幾篇的文章中看過了，在此就不重複了！

## 結語

以上就是 xv6 關於中斷的重點程式碼，特別是有關《裝置中斷》的部分！

看完 xv6 的中斷之後，我們下一篇將進入《記憶體管理》 mmu 的主題，特別是有關分頁表與虛擬位址如何對應到實體位址的那些事情，我們下回見！


