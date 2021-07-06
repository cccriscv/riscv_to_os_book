# Chapter 5 -- Interrupts and device drivers

驱动程序是操作系统中管理特定设备的代码：它配置设备硬件，告诉设备执行操作，处理由此产生的中断，并与可能等待设备I/O的进程进行交互。驱动程序代码可能很棘手，因为驱动程序与它所管理的设备同时执行。此外，驱动程序必须理解设备的硬件接口，而硬件接口可能是复杂的，而且文档化程度不高。

需要操作系统关注的设备通常可以被配置为产生中断，这是陷阱的一种类型。内核陷阱处理代码会在设备引发中断时识别并调用驱动的中断处理程序；在xv6中，这个调度发生在devintr(kernel/trap.c:177)。

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

许多设备驱动程序在两个上下文中执行代码：上半部分在进程的内核线程中运行，下半部分在中断时执行。上半部分是通过系统调用（如read和write）来调用，希望设备执行I/O。这段代码可能会要求硬件开始一个操作（比如要求磁盘读取一个块）；然后代码会等待操作完成。最终设备完成操作，并引发一个中断。驱动程序的中断处理程序作为下半部分，计算出什么操作已经完成，如果合适的话，唤醒一个等待的进程，并告诉硬件开始对任何等待的下一个操作进行工作。

## 5.1 Code: Console input

控制台驱动程序(console.c)是驱动程序结构的一个简单说明。控制台驱动程序通过连接到RISC-V上的UART串行端口硬件，接受人输入的字符。控制台驱动程序每次积累一行输入，处理特殊的输入字符，如退格键和control-u。用户进程，如shell，使用read系统调用从控制台获取输入行。当你在QEMU中向xv6键入输入时，你的击键会通过QEMU的模拟UART硬件传递给xv6。

驱动程序与之对话的UART硬件是由QEMU仿真的16550芯片[11]。在真实的计算机上，16550将管理一个连接到终端或其他计算机的RS232串行链接。

当运行QEMU时，它连接到你的键盘和显示器上。

UART硬件在软件看来是一组内存映射的控制寄存器。也就是说，有一些RISC-V硬件连接到UART设备的物理地址，因此加载和存储与设备硬件而不是RAM交互。UART的内存映射地址从0x10000000开始，即UART0（kernel/memlayout.h:21）。有一些UART控制寄存器，每个寄存器的宽度是一个字节。它们与UART0的偏移量定义在(kernel/uart.c:22)。例如，LSR寄存器中包含了表示输入字符是否在等待软件读取的位。这些字符（如果有的话）可以从RHR寄存器中读取。每次读取一个字符，UART硬件就会将其从内部等待字符的FIFO中删除，并在FIFO为空时清除LSR中的 "ready "位。UART发送硬件在很大程度上是独立于接收硬件的，如果软件向THR写入一个字节，UART就会发送该字节。

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

Xv6的main调用consoleinit（kernel/console.c:184）来初始化UART硬件。这段代码配置了UART，当UART接收到每个字节的输入时，产生一个接收中断，当UART每次完成发送一个字节的输出时，产生一个发送完成中断(kernel/uart.c:53)。

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

xv6的shell通过init.c打开的文件描述符（user/init.c:19）从控制台读取。

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

对读取系统调用的调用通过内核进入consoleread (kernel/console.c:82)。consoleread等待输入的到来(通过中断)并在cons.buf中缓冲，将输入复制到用户空间，然后(在一整行到达后)返回到用户进程。如果用户还没有输入一整行，任何读取进程都会在sleep调用中等待（kernel/console.c:98）（第7章解释了sleep的细节）。

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

当用户键入一个字符时，UART硬件要求RISC-V引发一个中断，从而激活xv6的陷阱处理程序。陷阱处理程序调用devintr(kernel/trap.c:177)，它查看RISC-V的scause寄存器，发现中断来自外部设备。然后它要求一个叫做PLIC[1]的硬件单元告诉它是哪个设备中断了(kernel/trap.c:186)。如果是UART，devintr调用uartintr。

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

uartintr (kernel/uart.c:180) 从UART硬件中读取任何等待的输入字符，并将它们交给consoleintr (kernel/console.c:138)；它不会等待字符，因为未来的输入会引发一个新的中断。consoleintr的工作是在cons.buf中积累输入字符，直到有整行字符到来。 consoleintr会特别处理退格键和其他一些字符。当一个新行到达时，consoleintr会唤醒一个等待的consoleread（如果有的话）。

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

一旦被唤醒，consoleread将观察cons.buf中的整行，将其复制到用户空间，然后返回（通过系统调用机制）到用户空间。

## 5.2 Code: Console output

对连接到控制台的文件描述符的写系统调用最终会到达uartputc(kernel/uart.c:87)。设备驱动维护了一个输出缓冲区(uart_tx_buf)，这样写过程就不必等待UART完成发送；相反，uartputc将每个字符追加到缓冲区，调用uartstart来启动设备发送(如果还没有的话)，然后返回。uartputc等待的唯一情况是缓冲区已经满了。

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

uartintr调用uartstart，检查设备是否真的已经完成发送，并将下一个缓冲区的输出字符交给设备。因此，如果一个进程向控制台写入多个字节，通常第一个字节将由uartputc对uartstart的调用发送，剩余的缓冲字节将由uartintr的uartstart调用发送，因为发送完成中断到来。

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

一个需要注意的一般模式是通过缓冲和中断将设备活动与进程活动解耦。控制台驱动程序可以处理输入，即使没有进程等待读取它；随后的读取将看到输入。同样，进程可以发送输出，而不必等待设备。这种解耦可以通过允许进程与设备I/O并发执行来提高性能，当设备速度很慢（如UART）或需要立即关注（如呼应打字字符）时，这种解耦尤为重要。这种想法有时被称为I/O并发。


## 5.3 Concurrency in drivers

你可能已经注意到在consoleread和consoleintr中调用获取。这些调用会获取一个锁，保护控制台驱动的数据结构不被并发访问。这里有三个并发危险：不同CPU上的两个进程可能会调用consoleread 同时；硬件可能会要求一个CPU传递一个控制台（实际上是UART）中断，而这个CPU已经在consoleread里面执行了；硬件可能会在consoleread执行的时候在另一个CPU上传递一个控制台中断。第6章将探讨锁如何在这些情况下提供帮助。

并发需要驱动程序小心的另一种方式是，一个进程可能正在等待来自设备的输入，但当一个不同的进程（或者根本没有进程）正在运行时，输入的中断信号可能会到达。因此，中断处理程序不允许思考被中断的进程或代码。例如，中断处理程序不能用当前进程的页表安全地调用copyout。中断处理程序通常只做相对较少的工作（例如，只是将输入数据复制到缓冲区），并唤醒上半部分代码来做剩下的工作。

## 5.4 Timer interrupts

Xv6使用定时器中断来维持它的时钟，并使它能够在计算绑定的进程之间进行切换；usertrap和kerneltrap中的yield调用导致这种切换。定时器中断来自连接到每个RISC-V CPU的时钟硬件。Xv6对这个时钟硬件进行编程
来周期性地中断每个CPU。

RISC-V要求在机器模式下采取定时器中断，而不是主管模式。RISCV机器模式执行时不需要分页，而且有一套单独的控制寄存器，所以在机器模式下运行普通的xv6内核代码是不现实的。因此，xv6对定时器中断的处理与上面布置的陷阱机制完全分开。

在main之前的start.c中，在机器模式下执行的代码，设置为接收定时器中断（kernel/start.c:57）。一部分工作是对CLINT硬件（核心本地中断器）进行编程，使其在一定的延迟后产生一个中断。另一部分是设置一个类似于trapframe的scratch区域，帮助定时器中断处理程序保存寄存器和CLINT寄存器的地址。

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

最后，start将mtvec设置为timervec，启用定时器中断。

定时器中断可能发生在用户或内核代码执行的任何时候；内核没有办法在关键操作期间禁用定时器中断。因此，定时器中断处理程序必须以保证不干扰被中断的内核代码的方式进行工作。基本的策略是处理程序要求RISC-V提出一个 "软件中断 "并立即返回。RISC-V用普通的陷阱机制将软件中断传递给内核，并允许内核将其禁用。处理定时器中断产生的软件中断的代码可以在devintr（kernel/trap.c:204）中看到。

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

机器模式的定时器中断向量是timervec(kernel/kernelvec.S:93)。它在start准备的scratch区域保存几个寄存器，告诉clint什么时候产生下一个定时器中断，要求RISC-V提出一个软件中断，恢复寄存器，然后返回。在定时器中断处理程序中没有C代码。

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

Xv6允许在内核中执行以及在执行用户程序时使用设备和定时器中断。定时器中断可以强制从定时器中断处理程序进行线程切换（调用让步），即使是在内核中执行。如果内核线程有时会花费大量的时间进行计算，而不返回用户空间，那么在内核线程之间公平地对CPU进行时间划分的能力是很有用的。然而，内核代码需要注意它可能会被暂停（由于定时器中断），然后在不同的CPU上恢复，这是xv6中一些复杂的根源。

如果设备和定时器中断只发生在执行用户代码的时候，内核可以变得更简单一些。

在一台典型的计算机上支持所有设备的全部功能是一件很辛苦的事情，因为设备很多，设备有很多功能，设备和驱动程序之间的协议可能很复杂，而且文档也不完善。在许多操作系统中，驱动程序所占的代码比核心内核还要多。

UART驱动器通过读取UART控制寄存器，一次检索一个字节的数据；这种模式被称为编程I/O，因为软件在驱动数据移动。程序化I/O简单，但速度太慢，无法在高数据速率下使用。需要高速移动大量数据的设备通常使用直接内存访问（DMA）。DMA设备硬件直接将传入数据写入RAM，并从RAM中读取传出数据。现代磁盘和网络设备都使用DMA。

DMA设备的驱动程序会在RAM中准备数据，然后用对控制寄存器的一次写入告诉设备处理准备好的数据。

当设备在不可预知的时间需要关注时，中断是有意义的，而且不要太频繁。

但中断对CPU的开销很大。因此，高速设备，如网络和磁盘控制器，使用了减少对中断需求的技巧。其中一个技巧是对整批传入或传出的请求提出一个单一的中断。另一个技巧是让驱动程序完全禁用中断，并定期检查设备是否需要关注。这种技术称为轮询。如果设备执行操作的速度非常快，轮询是有意义的，但如果设备大部分时间处于空闲状态，则会浪费CPU时间。一些驱动程序根据当前设备的负载情况，在轮询和中断之间动态切换。

UART驱动首先将传入的数据复制到内核的缓冲区，然后再复制到用户空间。

这在低数据速率下是有意义的，但对于那些快速生成或消耗数据的设备来说，这样的双重拷贝会显著降低性能。一些操作系统能够直接在用户空间缓冲区和设备硬件之间移动数据，通常使用DMA。

## 5.6 Exercises

1- 修改uart.c，使其完全不使用中断。你可能还需要修改 console.c。

2- 为以太网卡添加一个驱动程序。

