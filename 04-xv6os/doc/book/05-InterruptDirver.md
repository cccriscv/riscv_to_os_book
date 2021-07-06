# Chapter 5 -- Interrupts and device drivers

驅動程序是操作系統中管理特定設備的代碼：它配置設備硬件，告訴設備執行操作，處理由此產生的中斷，並與可能等待設備I/O的進程進行交互。驅動程序代碼可能很棘手，因為驅動程序與它所管理的設備同時執行。此外，驅動程序必須理解設備的硬件接口，而硬件接口可能是複雜的，而且文檔化程度不高。

需要操作系統關注的設備通常可以被配置為產生中斷，這是陷阱的一種類型。內核陷阱處理代碼會在設備引发中斷時識別並調用驅動的中斷處理程序；在xv6中，這個調度发生在devintr(kernel/trap.c:177)。

devintr(kernel/trap.c:177)

```cpp
// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr()
{
  uint64 scause = r_scause();

  if((scause & 0x8000000000000000L) &&
     (scause & 0xff) == 9){
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000001L){
    // software interrupt from a machine-mode timer interrupt,
    // forwarded by timervec in kernelvec.S.

    if(cpuid() == 0){
      clockintr();
    }
    
    // acknowledge the software interrupt by clearing
    // the SSIP bit in sip.
    w_sip(r_sip() & ~2);

    return 2;
  } else {
    return 0;
  }
}
```

許多設備驅動程序在兩個上下文中執行代碼：上半部分在進程的內核線程中運行，下半部分在中斷時執行。上半部分是通過系統調用（如read和write）來調用，希望設備執行I/O。這段代碼可能會要求硬件開始一個操作（比如要求磁盤讀取一個塊）；然後代碼會等待操作完成。最終設備完成操作，並引发一個中斷。驅動程序的中斷處理程序作為下半部分，計算出什麽操作已經完成，如果合適的話，喚醒一個等待的進程，並告訴硬件開始對任何等待的下一個操作進行工作。

## 5.1 Code: Console input

控制台驅動程序(console.c)是驅動程序結構的一個簡單說明。控制台驅動程序通過連接到RISC-V上的UART串行端口硬件，接受人輸入的字符。控制台驅動程序每次積累一行輸入，處理特殊的輸入字符，如退格鍵和control-u。用戶進程，如shell，使用read系統調用從控制台獲取輸入行。當你在QEMU中向xv6鍵入輸入時，你的擊鍵會通過QEMU的模擬UART硬件傳遞給xv6。

驅動程序與之對話的UART硬件是由QEMU仿真的16550芯片[11]。在真實的計算機上，16550將管理一個連接到終端或其他計算機的RS232串行鏈接。

當運行QEMU時，它連接到你的鍵盤和顯示器上。

UART硬件在軟件看來是一組內存映射的控制寄存器。也就是說，有一些RISC-V硬件連接到UART設備的物理地址，因此加載和存儲與設備硬件而不是RAM交互。UART的內存映射地址從0x10000000開始，即UART0（kernel/memlayout.h:21）。有一些UART控制寄存器，每個寄存器的寬度是一個字節。它們與UART0的偏移量定義在(kernel/uart.c:22)。例如，LSR寄存器中包含了表示輸入字符是否在等待軟件讀取的位。這些字符（如果有的話）可以從RHR寄存器中讀取。每次讀取一個字符，UART硬件就會將其從內部等待字符的FIFO中刪除，並在FIFO為空時清除LSR中的 "ready "位。UART发送硬件在很大程度上是獨立於接收硬件的，如果軟件向THR寫入一個字節，UART就會发送該字節。

```cpp
// Physical memory layout

// qemu -machine virt is set up like this,
// based on qemu's hw/riscv/virt.c:
//
// 00001000 -- boot ROM, provided by qemu
// 02000000 -- CLINT
// 0C000000 -- PLIC
// 10000000 -- uart0 
// 10001000 -- virtio disk 
// 80000000 -- boot ROM jumps here in machine mode
//             -kernel loads the kernel here
// unused RAM after 80000000.

// the kernel uses physical memory thus:
// 80000000 -- entry.S, then kernel text and data
// end -- start of kernel page allocation area
// PHYSTOP -- end RAM used by the kernel

// qemu puts UART registers here in physical memory.
#define UART0 0x10000000L
#define UART0_IRQ 10
```

Xv6的main調用consoleinit（kernel/console.c:184）來初始化UART硬件。這段代碼配置了UART，當UART接收到每個字節的輸入時，產生一個接收中斷，當UART每次完成发送一個字節的輸出時，產生一個发送完成中斷(kernel/uart.c:53)。

（kernel/console.c:184）

```cpp
void
consoleinit(void)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].read = consoleread;
  devsw[CONSOLE].write = consolewrite;
}
```

xv6的shell通過init.c打開的文件描述符（user/init.c:19）從控制台讀取。

（user/init.c）

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

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  for(;;){
    printf("init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf("init: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      exec("sh", argv);
      printf("init: exec sh failed\n");
      exit(1);
    }

    for(;;){
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

對讀取系統調用的調用通過內核進入consoleread (kernel/console.c:82)。consoleread等待輸入的到來(通過中斷)並在cons.buf中緩沖，將輸入複製到用戶空間，然後(在一整行到達後)返回到用戶進程。如果用戶還沒有輸入一整行，任何讀取進程都會在sleep調用中等待（kernel/console.c:98）（第7章解釋了sleep的細節）。

```cpp
//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dist indicates whether dst is a user
// or kernel address.
//
int
consoleread(int user_dst, uint64 dst, int n)
{
  uint target;
  int c;
  char cbuf;

  target = n;
  acquire(&cons.lock);
  while(n > 0){
    // wait until interrupt handler has put some
    // input into cons.buffer.
    while(cons.r == cons.w){
      if(myproc()->killed){
        release(&cons.lock);
        return -1;
      }
      sleep(&cons.r, &cons.lock);
    }

    c = cons.buf[cons.r++ % INPUT_BUF];

    if(c == C('D')){  // end-of-file
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      break;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    if(either_copyout(user_dst, dst, &cbuf, 1) == -1)
      break;

    dst++;
    --n;

    if(c == '\n'){
      // a whole line has arrived, return to
      // the user-level read().
      break;
    }
  }
  release(&cons.lock);

  return target - n;
}
```

當用戶鍵入一個字符時，UART硬件要求RISC-V引发一個中斷，從而激活xv6的陷阱處理程序。陷阱處理程序調用devintr(kernel/trap.c:177)，它查看RISC-V的scause寄存器，发現中斷來自外部設備。然後它要求一個叫做PLIC[1]的硬件單元告訴它是哪個設備中斷了(kernel/trap.c:186)。如果是UART，devintr調用uartintr。

(kernel/trap.c:177)

```cpp
// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr()
{
  uint64 scause = r_scause();

  if((scause & 0x8000000000000000L) &&
     (scause & 0xff) == 9){
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000001L){
    // software interrupt from a machine-mode timer interrupt,
    // forwarded by timervec in kernelvec.S.

    if(cpuid() == 0){
      clockintr();
    }
    
    // acknowledge the software interrupt by clearing
    // the SSIP bit in sip.
    w_sip(r_sip() & ~2);

    return 2;
  } else {
    return 0;
  }
}

```

uartintr (kernel/uart.c:180) 從UART硬件中讀取任何等待的輸入字符，並將它們交給consoleintr (kernel/console.c:138)；它不會等待字符，因為未來的輸入會引发一個新的中斷。consoleintr的工作是在cons.buf中積累輸入字符，直到有整行字符到來。 consoleintr會特別處理退格鍵和其他一些字符。當一個新行到達時，consoleintr會喚醒一個等待的consoleread（如果有的話）。

(kernel/uart.c:180)
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

一旦被喚醒，consoleread將觀察cons.buf中的整行，將其複製到用戶空間，然後返回（通過系統調用機制）到用戶空間。

## 5.2 Code: Console output

對連接到控制台的文件描述符的寫系統調用最終會到達uartputc(kernel/uart.c:87)。設備驅動維護了一個輸出緩沖區(uart_tx_buf)，這樣寫過程就不必等待UART完成发送；相反，uartputc將每個字符追加到緩沖區，調用uartstart來啟動設備发送(如果還沒有的話)，然後返回。uartputc等待的唯一情況是緩沖區已經滿了。

uartputc(kernel/uart.c:87)

```cpp
// add a character to the output buffer and tell the
// UART to start sending if it isn't already.
// blocks if the output buffer is full.
// because it may block, it can't be called
// from interrupts; it's only suitable for use
// by write().
void
uartputc(int c)
{
  acquire(&uart_tx_lock);

  if(panicked){
    for(;;)
      ;
  }

  while(1){
    if(uart_tx_w == uart_tx_r + UART_TX_BUF_SIZE){
      // buffer is full.
      // wait for uartstart() to open up space in the buffer.
      sleep(&uart_tx_r, &uart_tx_lock);
    } else {
      uart_tx_buf[uart_tx_w % UART_TX_BUF_SIZE] = c;
      uart_tx_w += 1;
      uartstart();
      release(&uart_tx_lock);
      return;
    }
  }
}
```

uartintr調用uartstart，檢查設備是否真的已經完成发送，並將下一個緩沖區的輸出字符交給設備。因此，如果一個進程向控制台寫入多個字節，通常第一個字節將由uartputc對uartstart的調用发送，剩余的緩沖字節將由uartintr的uartstart調用发送，因為发送完成中斷到來。

```cpp
// if the UART is idle, and a character is waiting
// in the transmit buffer, send it.
// caller must hold uart_tx_lock.
// called from both the top- and bottom-half.
void
uartstart()
{
  while(1){
    if(uart_tx_w == uart_tx_r){
      // transmit buffer is empty.
      return;
    }
    
    if((ReadReg(LSR) & LSR_TX_IDLE) == 0){
      // the UART transmit holding register is full,
      // so we cannot give it another byte.
      // it will interrupt when it's ready for a new byte.
      return;
    }
    
    int c = uart_tx_buf[uart_tx_r % UART_TX_BUF_SIZE];
    uart_tx_r += 1;
    
    // maybe uartputc() is waiting for space in the buffer.
    wakeup(&uart_tx_r);
    
    WriteReg(THR, c);
  }
}
```

一個需要注意的一般模式是通過緩沖和中斷將設備活動與進程活動解耦。控制台驅動程序可以處理輸入，即使沒有進程等待讀取它；隨後的讀取將看到輸入。同樣，進程可以发送輸出，而不必等待設備。這種解耦可以通過允許進程與設備I/O並发執行來提高性能，當設備速度很慢（如UART）或需要立即關注（如呼應打字字符）時，這種解耦尤為重要。這種想法有時被稱為I/O並发。


## 5.3 Concurrency in drivers

你可能已經注意到在consoleread和consoleintr中調用獲取。這些調用會獲取一個鎖，保護控制台驅動的數據結構不被並发訪問。這里有三個並发危險：不同CPU上的兩個進程可能會調用consoleread 同時；硬件可能會要求一個CPU傳遞一個控制台（實際上是UART）中斷，而這個CPU已經在consoleread里面執行了；硬件可能會在consoleread執行的時候在另一個CPU上傳遞一個控制台中斷。第6章將探討鎖如何在這些情況下提供幫助。

並发需要驅動程序小心的另一種方式是，一個進程可能正在等待來自設備的輸入，但當一個不同的進程（或者根本沒有進程）正在運行時，輸入的中斷信號可能會到達。因此，中斷處理程序不允許思考被中斷的進程或代碼。例如，中斷處理程序不能用當前進程的頁表安全地調用copyout。中斷處理程序通常只做相對較少的工作（例如，只是將輸入數據複製到緩沖區），並喚醒上半部分代碼來做剩下的工作。

## 5.4 Timer interrupts

Xv6使用定時器中斷來維持它的時鐘，並使它能夠在計算綁定的進程之間進行切換；usertrap和kerneltrap中的yield調用導致這種切換。定時器中斷來自連接到每個RISC-V CPU的時鐘硬件。Xv6對這個時鐘硬件進行編程
來周期性地中斷每個CPU。

RISC-V要求在機器模式下采取定時器中斷，而不是主管模式。RISCV機器模式執行時不需要分頁，而且有一套單獨的控制寄存器，所以在機器模式下運行普通的xv6內核代碼是不現實的。因此，xv6對定時器中斷的處理與上面布置的陷阱機制完全分開。

在main之前的start.c中，在機器模式下執行的代碼，設置為接收定時器中斷（kernel/start.c:57）。一部分工作是對CLINT硬件（核心本地中斷器）進行編程，使其在一定的延遲後產生一個中斷。另一部分是設置一個類似於trapframe的scratch區域，幫助定時器中斷處理程序保存寄存器和CLINT寄存器的地址。

（kernel/start.c:57）

```cpp
// set up to receive timer interrupts in machine mode,
// which arrive at timervec in kernelvec.S,
// which turns them into software interrupts for
// devintr() in trap.c.
void
timerinit()
{
  // each CPU has a separate source of timer interrupts.
  int id = r_mhartid();

  // ask the CLINT for a timer interrupt.
  int interval = 1000000; // cycles; about 1/10th second in qemu.
  *(uint64*)CLINT_MTIMECMP(id) = *(uint64*)CLINT_MTIME + interval;

  // prepare information in scratch[] for timervec.
  // scratch[0..2] : space for timervec to save registers.
  // scratch[3] : address of CLINT MTIMECMP register.
  // scratch[4] : desired interval (in cycles) between timer interrupts.
  uint64 *scratch = &timer_scratch[id][0];
  scratch[3] = CLINT_MTIMECMP(id);
  scratch[4] = interval;
  w_mscratch((uint64)scratch);

  // set the machine-mode trap handler.
  w_mtvec((uint64)timervec);

  // enable machine-mode interrupts.
  w_mstatus(r_mstatus() | MSTATUS_MIE);

  // enable machine-mode timer interrupts.
  w_mie(r_mie() | MIE_MTIE);
}

```

最後，start將mtvec設置為timervec，啟用定時器中斷。

定時器中斷可能发生在用戶或內核代碼執行的任何時候；內核沒有辦法在關鍵操作期間禁用定時器中斷。因此，定時器中斷處理程序必須以保證不幹擾被中斷的內核代碼的方式進行工作。基本的策略是處理程序要求RISC-V提出一個 "軟件中斷 "並立即返回。RISC-V用普通的陷阱機制將軟件中斷傳遞給內核，並允許內核將其禁用。處理定時器中斷產生的軟件中斷的代碼可以在devintr（kernel/trap.c:204）中看到。

devintr（kernel/trap.c）

```cpp
// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr()
{
  uint64 scause = r_scause();

  if((scause & 0x8000000000000000L) &&
     (scause & 0xff) == 9){
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000001L){
    // software interrupt from a machine-mode timer interrupt,
    // forwarded by timervec in kernelvec.S.

    if(cpuid() == 0){
      clockintr();
    }
    
    // acknowledge the software interrupt by clearing
    // the SSIP bit in sip.
    w_sip(r_sip() & ~2);

    return 2;
  } else {
    return 0;
  }
}


```

機器模式的定時器中斷向量是timervec(kernel/kernelvec.S:93)。它在start準備的scratch區域保存幾個寄存器，告訴clint什麽時候產生下一個定時器中斷，要求RISC-V提出一個軟件中斷，恢復寄存器，然後返回。在定時器中斷處理程序中沒有C代碼。

timervec(kernel/kernelvec.S:93)

```s

        #
        # machine-mode timer interrupt.
        #
.globl timervec
.align 4
timervec:
        # start.c has set up the memory that mscratch points to:
        # scratch[0,8,16] : register save area.
        # scratch[24] : address of CLINT's MTIMECMP register.
        # scratch[32] : desired interval between interrupts.
        
        csrrw a0, mscratch, a0
        sd a1, 0(a0)
        sd a2, 8(a0)
        sd a3, 16(a0)

        # schedule the next timer interrupt
        # by adding interval to mtimecmp.
        ld a1, 24(a0) # CLINT_MTIMECMP(hart)
        ld a2, 32(a0) # interval
        ld a3, 0(a1)
        add a3, a3, a2
        sd a3, 0(a1)

        # raise a supervisor software interrupt.
	li a1, 2
        csrw sip, a1

        ld a3, 16(a0)
        ld a2, 8(a0)
        ld a1, 0(a0)
        csrrw a0, mscratch, a0

        mret

```

## 5.5 Real world

Xv6允許在內核中執行以及在執行用戶程序時使用設備和定時器中斷。定時器中斷可以強制從定時器中斷處理程序進行線程切換（調用讓步），即使是在內核中執行。如果內核線程有時會花費大量的時間進行計算，而不返回用戶空間，那麽在內核線程之間公平地對CPU進行時間劃分的能力是很有用的。然而，內核代碼需要注意它可能會被暫停（由於定時器中斷），然後在不同的CPU上恢復，這是xv6中一些複雜的根源。

如果設備和定時器中斷只发生在執行用戶代碼的時候，內核可以變得更簡單一些。

在一台典型的計算機上支持所有設備的全部功能是一件很辛苦的事情，因為設備很多，設備有很多功能，設備和驅動程序之間的協議可能很複雜，而且文檔也不完善。在許多操作系統中，驅動程序所占的代碼比核心內核還要多。

UART驅動器通過讀取UART控制寄存器，一次檢索一個字節的數據；這種模式被稱為編程I/O，因為軟件在驅動數據移動。程序化I/O簡單，但速度太慢，無法在高數據速率下使用。需要高速移動大量數據的設備通常使用直接內存訪問（DMA）。DMA設備硬件直接將傳入數據寫入RAM，並從RAM中讀取傳出數據。現代磁盤和網絡設備都使用DMA。

DMA設備的驅動程序會在RAM中準備數據，然後用對控制寄存器的一次寫入告訴設備處理準備好的數據。

當設備在不可預知的時間需要關注時，中斷是有意義的，而且不要太頻繁。

但中斷對CPU的開銷很大。因此，高速設備，如網絡和磁盤控制器，使用了減少對中斷需求的技巧。其中一個技巧是對整批傳入或傳出的請求提出一個單一的中斷。另一個技巧是讓驅動程序完全禁用中斷，並定期檢查設備是否需要關注。這種技術稱為輪詢。如果設備執行操作的速度非常快，輪詢是有意義的，但如果設備大部分時間處於空閑狀態，則會浪費CPU時間。一些驅動程序根據當前設備的負載情況，在輪詢和中斷之間動態切換。

UART驅動首先將傳入的數據複製到內核的緩沖區，然後再複製到用戶空間。

這在低數據速率下是有意義的，但對於那些快速生成或消耗數據的設備來說，這樣的雙重拷貝會顯著降低性能。一些操作系統能夠直接在用戶空間緩沖區和設備硬件之間移動數據，通常使用DMA。

## 5.6 Exercises

1- 修改uart.c，使其完全不使用中斷。你可能還需要修改 console.c。

2- 為以太網卡添加一個驅動程序。
