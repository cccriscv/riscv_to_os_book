# Chapter 4 -- Traps and system calls

有三种事件会导致CPU搁置普通指令的执行，强制将控制权转移给处理该事件的特殊代码。一种情况是系统调用，当用户程序执行ecall指令要求内核为其做某事时。另一种情况是异常：一条指令（用户或内核）做了一些非法的事情，如除以零或使用无效的虚拟地址。第三种情况是设备中断，当一个设备发出需要注意的信号时，例如当磁盘硬件完成一个读写请求时。

本书使用陷阱作为这些情况的通用术语。通常不管在陷阱发生时正在执行的代码以后都需要恢复，不应该需要意识到发生了什么特别的事情。也就是说，我们通常希望陷阱是透明的；这一点对于中断来说尤其重要，被中断的代码通常不会想到会发生这种情况。通常的顺序是：陷阱迫使控制权转移到内核；内核保存寄存器和其他状态，以便恢复执行；内核执行适当的处理程序代码（例如，系统调用实现或设备驱动程序）；内核恢复保存的状态，并从陷阱中返回；原始代码从原来的地方恢复。

xv6内核会处理所有的陷阱。这对于系统调用来说是很自然的。这对中断是有意义的，因为隔离要求用户进程不能直接使用设备，而且只有内核才有设备处理所需的状态。这对异常也是有意义的，因为xv6对所有来自用户空间的异常的响应是杀死违规程序。

Xv6的陷阱处理分为四个阶段：RISC-V CPU的硬件操作，为内核C代码准备的汇编 "向量"，决定如何处理陷阱的C陷阱处理程序，以及系统调用或设备驱动服务例程。虽然这三种陷阱类型的共性表明，内核可以用一个单一的代码路径来处理所有的陷阱，但事实证明，为三种不同的情况，即来自用户空间的陷阱、来自内核空间的陷阱和定时器中断，分别设置不同的汇编向量和C陷阱处理程序是很方便的。

## 4.1 RISC-V trap machinery

每个RISC-V CPU都有一组控制寄存器，内核写入这些寄存器来告诉CPU如何处理陷阱，内核可以通过读取这些寄存器来发现已经发生的陷阱。RISC-V文档包含了完整的故事[1]。riscv.h（kernel/riscv.h:1）包含了xv6使用的定义。这里是最重要的寄存器的概要。

* stvec。内核在这里写下陷阱处理程序的地址；RISC -V在这里跳转来处理陷阱。
* sepc: 当一个陷阱发生时，RISC-V将程序计数器保存在这里（因为pc会被stvec覆盖）。sret (ret from trap)指令将sepc复制到pc中。内核可以写入sepc来控制sret的去向。
* 吓。RISC -V在这里写了一个数字来描述陷阱的原因.
* scause: RISC-V在这里放一个数字来描述陷阱的原因。内核在这里放了一个值，在陷阱处理程序开始的时候很方便。
* sstatus: sstatus中的SIE位用来描述陷阱的原因。sstatus: sstatus中的SIE位控制设备中断是否被激活. 如果内核清除SIE，RISC-V将推迟设备中断，直到内核设置SIE。SPP位表示一个陷阱是来自用户模式还是上级模式，并控制sret返回到什么模式

riscv.h

```cpp
// Supervisor Trap-Vector Base Address
// low two bits are mode.
static inline void 
w_stvec(uint64 x)
{
  asm volatile("csrw stvec, %0" : : "r" (x));
}

static inline uint64
r_stvec()
{
  uint64 x;
  asm volatile("csrr %0, stvec" : "=r" (x) );
  return x;
}

// Machine-mode interrupt vector
static inline void 
w_mtvec(uint64 x)
{
  asm volatile("csrw mtvec, %0" : : "r" (x));
}
```

上述寄存器与在主管模式下处理的陷阱有关，在用户模式下不能读或写。对于机器模式下处理的陷阱，有一组等效的控制寄存器，xv6只在定时器中断的特殊情况下使用它们。

多核芯片上的每个CPU都有自己的一组这些寄存器，而且任何时候都可能有多个CPU在处理一个陷阱。

当需要强制陷阱时，RISC-V硬件对所有的陷阱类型（除定时器中断外）都会进行以下操作。

1. 如果陷阱是设备中断，且sstatus SIE位为清，不要做以下任何操作。
2. 通过清除SIE禁用中断。
3. 将pc复制到sepc。
4. 在sstatus的SPP位中保存当前模式（用户或主管）。
5. 设置scause来反映陷阱的原因。
6. 将模式设置为supervisor。
7. 将stvec复制到pc上。

注意，CPU不会切换到内核页表，不会切换到内核中的堆栈，也不会保存pc以外的任何寄存器。内核软件必须执行这些任务。

CPU在陷阱期间做最少的工作的原因之一是为了给软件提供灵活性；例如，一些操作系统在某些情况下不需要切换页表，这可以提高性能。

你可能会想，CPU硬件的陷阱处理顺序是否可以进一步简化。例如，假设CPU没有切换程序计数器。那么陷阱可能会切换到主管模式，同时还在运行用户指令。这些用户指令可以打破用户/内核的隔离，例如通过修改satp寄存器指向一个允许访问所有物理内存的页表。因此，CPU必须切换到内核指定的指令地址，即stvec。

## 4.2 Traps from user space

在用户空间执行时，如果用户程序进行了系统调用(ecall指令)，或者做了一些非法的事情，或者设备中断，都可能发生陷阱。从用户空间发出陷阱的高级路径是uservec(kernel/trampoline.S:16)，然后是usertrap(kernel/trap.c:37)；返回时是usertrapret(kernel/trap.c:90)，然后是userret(kernel/trampoline.S:16)。

```cpp
//
// handle an interrupt, exception, or system call from user space.
// called from trampoline.S
//
void
usertrap(void)
{
  int which_dev = 0;

  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();
  
  // save user program counter.
  p->trapframe->epc = r_sepc();
  
  if(r_scause() == 8){
    // system call

    if(p->killed)
      exit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change sstatus &c registers,
    // so don't enable until done with those registers.
    intr_on();

    syscall();
  } else if((which_dev = devintr()) != 0){
    // ok
  } else {
    printf("usertrap(): unexpected scause %p pid=%d\n", r_scause(), p->pid);
    printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
    p->killed = 1;
  }

  if(p->killed)
    exit(-1);

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();

  usertrapret();
}
```

来自用户代码的陷阱比来自内核的陷阱更具挑战性，因为satp指向的用户页表并不映射内核，而且堆栈指针可能包含一个无效甚至恶意的值。

由于RISC-V硬件在陷阱过程中不会切换页表，所以用户页表必须包含uservec的映射，即stvec指向的陷阱向量指令，uservec必须切换satp指向内核页表；为了在切换后继续执行指令，uservec必须在内核页表中映射到与用户页表相同的地址。

Xv6用一个包含uservec的trampoline页来满足这些约束。Xv6在内核页表和每个用户页表中的同一个虚拟地址上映射了trampoline页。

这个虚拟地址就是trampoline（如我们在图2.3和图3.3中看到的）。trampoline内容设置在trampoline.S中，（执行用户代码时）stvec设置为uservec（kernel/trampoline.S:16）。

![](../img/Figure2.3.png)

当uservec启动时，所有32个寄存器都包含被中断的代码所拥有的值。但是uservec需要能够修改一些寄存器，以便设置satp和生成保存寄存器的地址。RISC-V以sscratch寄存器的形式提供了帮助。

uservec开头的csrrw指令将a0和sscratch的内容互换。现在用户代码的a0被保存了；uservec有一个寄存器（a0）可以玩；a0包含了内核之前放在sscratch中的值。

```s
	.section trampsec
.globl trampoline
trampoline:
.align 4
.globl uservec
uservec:    
	#
        # trap.c sets stvec to point here, so
        # traps from user space start here,
        # in supervisor mode, but with a
        # user page table.
        #
        # sscratch points to where the process's p->trapframe is
        # mapped into user space, at TRAPFRAME.
        #
        
	# swap a0 and sscratch
        # so that a0 is TRAPFRAME
        csrrw a0, sscratch, a0

        # save the user registers in TRAPFRAME
        sd ra, 40(a0)
        sd sp, 48(a0)
        sd gp, 56(a0)
        sd tp, 64(a0)
        sd t0, 72(a0)
        sd t1, 80(a0)
        sd t2, 88(a0)
        sd s0, 96(a0)
        sd s1, 104(a0)
        sd a1, 120(a0)
        sd a2, 128(a0)
        sd a3, 136(a0)
        sd a4, 144(a0)
        sd a5, 152(a0)
        sd a6, 160(a0)
        sd a7, 168(a0)
        sd s2, 176(a0)
        sd s3, 184(a0)
        sd s4, 192(a0)
        sd s5, 200(a0)
        sd s6, 208(a0)
        sd s7, 216(a0)
        sd s8, 224(a0)
        sd s9, 232(a0)
        sd s10, 240(a0)
        sd s11, 248(a0)
        sd t3, 256(a0)
        sd t4, 264(a0)
        sd t5, 272(a0)
        sd t6, 280(a0)

	# save the user a0 in p->trapframe->a0
        csrr t0, sscratch
        sd t0, 112(a0)

        # restore kernel stack pointer from p->trapframe->kernel_sp
        ld sp, 8(a0)

        # make tp hold the current hartid, from p->trapframe->kernel_hartid
        ld tp, 32(a0)

        # load the address of usertrap(), p->trapframe->kernel_trap
        ld t0, 16(a0)

        # restore kernel page table from p->trapframe->kernel_satp
        ld t1, 0(a0)
        csrw satp, t1
        sfence.vma zero, zero

        # a0 is no longer valid, since the kernel page
        # table does not specially map p->tf.

        # jump to usertrap(), which does not return
        jr t0
```

uservec的下一个任务是保存用户寄存器。在进入用户空间之前，内核先前设置sscratch指向一个每进程的trapframe，这个trapframe（除其他外）有保存所有用户寄存器的空间（kernel/proc.h:44）。因为satp仍然是指用户页表，所以uservec需要将trapframe映射到用户地址空间中。在创建每个进程时，xv6为进程的trapframe分配一个页面，并安排它始终映射在用户虚拟地址TRAPFRAME，也就是TRAMPOLINE的下方。进程的p->trapframe也指向trapframe，不过是在它的物理地址，这样内核就可以通过内核页表来使用它。

因此在交换a0和sscratch后，a0持有指向当前进程的trapframe的指针。

uservec现在将所有用户寄存器保存在那里，包括从sscratch读取的用户a0。

trapframe包含指向当前进程的内核栈、当前CPU的hartid、usertrap的地址和内核页表的地址的指针，uservec检索这些值，将satp切换到内核页表，然后调用usertrap。

usertrap的工作是确定陷阱的原因，处理它，然后返回(kernel/trap.c:37)。如上所述，它首先改变stvec，这样在内核中的陷阱将由kernelvec处理。它保存了sepc(保存的用户程序计数器)，这也是因为usertrap中可能会有一个进程切换，导致sepc被覆盖。如果陷阱是系统调用，则syscall处理；如果是设备中断，则devintr处理；否则就是异常，内核会杀死故障进程。系统调用路径在保存的用户pc上增加了4个，因为RISC-V在系统调用的情况下，会留下指向ecall指令的程序指针。

在退出时，usertrap检查进程是否已经被杀死或应该让出CPU（如果这个陷阱是一个定时器中断）。

回到用户空间的第一步是调用usertrapret(kernel/trap.c:90)。这个函数设置RISC-V控制寄存器，为将来从用户空间的陷阱做准备。这包括改变stvec来引用uservec，准备uservec所依赖的trapframe字段，并将sepc设置为先前保存的用户程序计数器。最后，usertrapret在用户页表和内核页表中映射的trampoline页上调用userret，原因是userret中的汇编代码会切换页表。

```cpp
//
// return to user space
//
void
usertrapret(void)
{
  struct proc *p = myproc();

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(), so turn off interrupts until
  // we're back in user space, where usertrap() is correct.
  intr_off();

  // send syscalls, interrupts, and exceptions to trampoline.S
  w_stvec(TRAMPOLINE + (uservec - trampoline));

  // set up trapframe values that uservec will need when
  // the process next re-enters the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.
  
  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  x |= SSTATUS_SPIE; // enable interrupts in user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);

  // tell trampoline.S the user page table to switch to.
  uint64 satp = MAKE_SATP(p->pagetable);

  // jump to trampoline.S at the top of memory, which 
  // switches to the user page table, restores user registers,
  // and switches to user mode with sret.
  uint64 fn = TRAMPOLINE + (userret - trampoline);
  ((void (*)(uint64,uint64))fn)(TRAPFRAME, satp);
}
```

usertrapret对userret的调用在a0中传递一个指向进程用户页表的指针，在a1中传递一个指向TRAPFRAME的指针(kernel/trampoline.S:88)，userret将satp切换到进程的用户页表。回顾一下，用户页表同时映射了trampoline页和TRAPFRAME，但没有映射内核的其他内容。同样，事实上，trampoline页在用户页表和内核页表中映射在同一个虚拟地址上，这也是允许uservec在改变satp后继续执行的原因。

```s
.globl userret
userret:
        # userret(TRAPFRAME, pagetable)
        # switch from kernel to user.
        # usertrapret() calls here.
        # a0: TRAPFRAME, in user page table.
        # a1: user page table, for satp.

        # switch to the user page table.
        csrw satp, a1
        sfence.vma zero, zero

        # put the saved user a0 in sscratch, so we
        # can swap it with our a0 (TRAPFRAME) in the last step.
        ld t0, 112(a0)
        csrw sscratch, t0

        # restore all but a0 from TRAPFRAME
        ld ra, 40(a0)
        ld sp, 48(a0)
        ld gp, 56(a0)
        ld tp, 64(a0)
        ld t0, 72(a0)
        ld t1, 80(a0)
        ld t2, 88(a0)
        ld s0, 96(a0)
        ld s1, 104(a0)
        ld a1, 120(a0)
        ld a2, 128(a0)
        ld a3, 136(a0)
        ld a4, 144(a0)
        ld a5, 152(a0)
        ld a6, 160(a0)
        ld a7, 168(a0)
        ld s2, 176(a0)
        ld s3, 184(a0)
        ld s4, 192(a0)
        ld s5, 200(a0)
        ld s6, 208(a0)
        ld s7, 216(a0)
        ld s8, 224(a0)
        ld s9, 232(a0)
        ld s10, 240(a0)
        ld s11, 248(a0)
        ld t3, 256(a0)
        ld t4, 264(a0)
        ld t5, 272(a0)
        ld t6, 280(a0)

	# restore user a0, and save TRAPFRAME in sscratch
        csrrw a0, sscratch, a0
        
        # return to user mode and user pc.
        # usertrapret() set up sstatus and sepc.
        sret
```

userret将trapframe保存的用户a0复制到sscratch中，为以后与TRAPFRAME交换做准备。从这时开始，userret能使用的数据只有寄存器内容和trapframe的内容。接下来userret从trapframe中恢复保存的用户寄存器，对a0和sscratch做最后的交换，恢复用户a0并保存TRAPFRAME，为下一次的陷阱做准备，并使用sret返回用户空间。

## 4.3 Code: Calling system calls

第2章以initcode.S调用exec系统调用结束（user/initcode.S:11）。我们来看看用户调用是如何在内核中实现exec系统调用的。

用户代码将exec的参数放在寄存器a0和a1中，并将系统调用号放在a7中。系统调用号与函数指针表syscalls数组（kernel/syscall.c:108）中的条目相匹配。ecall指令陷阱进入内核，并执行uservec、usertrap，然后执行syscall，就像我们上面看到的那样。

initcode.S

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

syscall (kernel/syscall.c:133)从trapframe中保存的a7中检索系统调用号，并将其用于索引到syscalls中。对于第一次系统调用，a7包含SYS_exec（kernel/syscall.h:8），结果调用系统调用实现函数sys_exec。

syscall.c

```cpp
// ...
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

当系统调用实现函数返回时，syscall将其返回值记录在p->trapframe->a0中。这将使原来的用户空间调用exec()返回该值，因为RISC-V上的C调用惯例将返回值放在a0中。系统调用惯例返回负数表示错误，0或正数表示成功。如果系统调用号无效，syscall会打印一个错误并返回-1。

## 4.4 Code: System call arguments

内核中的系统调用实现需要找到用户代码传递的参数。因为用户代码调用系统调用封装函数，所以参数最初在RISC-V C调用惯例放置的地方：寄存器中。内核陷阱代码将用户寄存器保存到当前进程的陷阱帧中，内核代码可以在那里找到它们。函数argint、argaddr和argfd从陷阱帧中以整数、指针或文件描述符的形式检索第n个系统调用参数。它们都调用argraw来检索保存的用户寄存器(kernel/syscall.c:35)。

(kernel/syscall.c:35)

```cpp
static uint64
argraw(int n)
{
  struct proc *p = myproc();
  switch (n) {
  case 0:
    return p->trapframe->a0;
  case 1:
    return p->trapframe->a1;
  case 2:
    return p->trapframe->a2;
  case 3:
    return p->trapframe->a3;
  case 4:
    return p->trapframe->a4;
  case 5:
    return p->trapframe->a5;
  }
  panic("argraw");
  return -1;
}
```

一些系统调用传递指针作为参数，而内核必须使用这些指针来读取或写入用户内存。例如，exec系统调用会向内核传递一个指向用户空间中的字符串参数的指针数组。这些指针带来了两个挑战。首先，用户程序可能是错误的或恶意的，可能会传递给内核一个无效的指针或一个旨在欺骗内核访问内核内存而不是用户内存的指针。第二，xv6内核页表映射与用户页表映射不一样，所以内核不能使用普通指令从用户提供的地址加载或存储。

内核实现了一些函数，可以安全地将数据传输到用户提供的地址，或者从用户提供的地址传输数据。

fetchstr是一个例子(kernel/syscall.c:25)。文件系统调用，如exec，使用fetchstr从用户空间中获取字符串文件名参数，fetchstr调用copyinstr来完成这项艰难的工作。

(kernel/syscall.c:25)

```cpp
// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
int
fetchstr(uint64 addr, char *buf, int max)
{
  struct proc *p = myproc();
  int err = copyinstr(p->pagetable, buf, addr, max);
  if(err < 0)
    return err;
  return strlen(buf);
}
```

copyinstr (kernel/vm.c:406) 将用户页表 pagetable 中的虚拟地址 srcva 复制到 dst，最多可复制最大字节。它使用 walkaddr (调用 walk) 在软件中走页表来确定 srcva 的物理地址 pa0。由于内核将所有的物理地址
walkaddr(kernel/vm.c:95)检查用户提供的虚拟地址是否是进程的用户地址空间的一部分，所以程序不能欺骗内核读取其他内存。

(kernel/vm.c)

```cpp
// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
int
copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max)
{
  uint64 n, va0, pa0;
  int got_null = 0;

  while(got_null == 0 && max > 0){
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    if(pa0 == 0)
      return -1;
    n = PGSIZE - (srcva - va0);
    if(n > max)
      n = max;

    char *p = (char *) (pa0 + (srcva - va0));
    while(n > 0){
      if(*p == '\0'){
        *dst = '\0';
        got_null = 1;
        break;
      } else {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  if(got_null){
    return 0;
  } else {
    return -1;
  }
}
```

类似的函数copyout，可以将数据从内核复制到用户提供的地址。

## 4.5 Traps from kernel space

Xv6根据用户代码还是内核代码在执行，对CPU陷阱寄存器的配置有些不同。当内核在CPU上执行时，内核将stvec指向kernelvec处的汇编代码（kernel/kernelvec.S:10）。由于xv6已经在内核中，kernelvec可以依靠satp被设置为内核页表，以及堆栈指针指向一个有效的内核堆栈，kernelvec保存了所有的寄存器，这样被中断的代码最终可以不受干扰地恢复。

kernelvec将寄存器保存在被中断的内核线程的栈上，这很有意义，因为寄存器的值属于那个线程。如果陷阱导致切换到不同的线程，这一点尤其重要--在这种情况下，陷阱实际上会在新线程的堆栈上返回，将被中断线程保存的寄存器安全地留在其堆栈上。

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
        // make room to save registers.
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

	// call the C trap handler in trap.c
        call kerneltrap

        // restore registers.
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

        addi sp, sp, 256

        // return to whatever we were doing in the kernel.
        sret
```

如果kerneltrap是由于定时器中断而被调用的，并且一个进程的内核线程正在运行（而不是调度器线程），kerneltrap调用屈服来给其他线程一个运行的机会。

在某些时候，这些线程中的一个线程会屈服，让我们的线程和它的kerneltrap再次恢复。第7章解释了yield中发生的事情。

当kerneltrap的工作完成后，它需要返回到被陷阱打断的任何代码。因为yield可能会干扰sstatus中保存的sepc和保存的前一个模式，所以kerneltrap在启动时保存了它们。kernelvec从堆栈中弹出保存的寄存器并执行sret，sret将sepc复制到pc中并恢复被中断的内核代码。

值得思考的是，如果kerneltrap由于定时器中断而调用yield，陷阱返回是如何发生的。

当一个CPU从用户空间进入内核时，Xv6会将该CPU的stvec设置为kernelvec；你可以在usertrap（kernel/trap.c:29）中看到这一点。有一个时间窗口，当内核在执行时，stvec被设置为uservec，在这个窗口中禁用设备中断是非常重要的。幸运的是RISC-V在开始采取陷阱时总是禁用中断，xv6在设置stvec之后才会再次启用中断。

（kernel/trap.c:29）

```cpp
// set up to take exceptions and traps while in the kernel.
void
trapinithart(void)
{
  w_stvec((uint64)kernelvec);
}

```

## 4.6 Page-fault exceptions

Xv6对异常的响应相当无聊：如果异常发生在用户空间，内核就会杀死故障进程。如果一个异常发生在内核中，内核就会恐慌。真正的操作系统通常会以更有趣的方式进行响应。

举个例子，许多内核使用页面故障来实现写后复制（COW）fork。要解释copy-on-write fork，可以考虑xv6的fork，在第3章中介绍过。fork通过调用uvmcopy(kernel/vm.c:309)为子程序分配物理内存，并将父程序的内存复制到子程序中，使子程序拥有与父程序相同的内存内容。如果子代和父代能够共享父代的物理内存，效率会更高。然而，直接实现这个方法是行不通的，因为这会导致父代和子代对共享栈和堆的写入，从而扰乱对方的执行。

uvmcopy(kernel/vm.c:309)

```cpp
// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  char *mem;

  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walk(old, i, 0)) == 0)
      panic("uvmcopy: pte should exist");
    if((*pte & PTE_V) == 0)
      panic("uvmcopy: page not present");
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    if((mem = kalloc()) == 0)
      goto err;
    memmove(mem, (char*)pa, PGSIZE);
    if(mappages(new, i, PGSIZE, (uint64)mem, flags) != 0){
      kfree(mem);
      goto err;
    }
  }
  return 0;

 err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  return -1;
}

```

父母和子女可以安全地共享物理内存，使用copy-on-write fork，由page faults驱动。当CPU不能将虚拟地址翻译成物理地址时，CPU会产生一个页故障异常。RISC-V有三种不同的页故障：加载页故障（当加载指令不能翻译其虚拟地址时）、存储页故障（当存储指令不能翻译其虚拟地址时）和指令页故障（当指令的地址不能翻译时）。scause寄存器中的值表示页面故障的类型，stval寄存器中包含无法翻译的地址。

COW fork中的基本计划是父级和子级最初共享所有的物理页面，但将它们映射为只读。因此，当子代或父代执行存储指令时，RISC-V CPU会引发一个页面故障异常。作为对这个异常的响应，内核会对包含故障地址的页面做一个拷贝。它将一个副本读/写映射在子地址空间，另一个副本读/写映射在父地址空间。更新页表后，内核在引起故障的指令处恢复故障处理。因为内核已经更新了相关的PTE，允许写入，所以现在故障指令将无故障执行。

这个COW计划对fork很有效，因为往往子程序在fork后立即调用exec，用新的地址空间替换其地址空间。在这种常见的情况下，子程序只会遇到一些页面故障，而内核可以避免进行完整的复制。此外，COW fork是透明的：不需要对应用程序进行修改，应用程序就能受益。

页表和页故障的结合，开启了除COW fork之外的多种有趣的可能性。另一个被广泛使用的特性叫做懒惰分配，它有两个部分。首先，当一个应用程序调用sbrk时，内核会增长地址空间，但在页表中把新的地址标记为无效。第二，当这些新地址中的一个出现页面故障时，内核分配物理内存并将其映射到页表中。由于应用程序经常要求获得比他们需要的更多的内存，所以懒惰分配是一个胜利：内核只在应用程序实际使用时才分配内存。像COW fork一样，内核可以对应用程序透明地实现这个功能。

然而，另一个被广泛使用的利用页面故障的功能是从磁盘上分页。如果应用程序需要的内存超过了可用的物理RAM，内核可以驱逐一些页面：将它们写入一个存储设备，比如磁盘，并将其PTE标记为无效。如果一个应用程序读取或写入一个被驱逐的页面，CPU将遇到一个页面故障。内核就可以检查故障地址。如果该地址属于磁盘上的页面，内核就会分配一个物理内存的页面，从磁盘上读取页面到该内存，更新PTE为有效并引用该内存，然后恢复应用程序。为了给该页腾出空间，内核可能要驱逐另一个页。这个特性不需要对应用程序进行任何修改，如果应用程序具有引用的位置性（即它们在任何时候都只使用其内存的一个子集），这个特性就能很好地发挥作用。

其他结合分页和分页错误异常的功能包括自动扩展堆栈和内存映射文件。

## 4.7 Real world

如果将内核内存映射到每个进程的用户页表中（使用适当的PTE权限标志），就可以消除对特殊的蹦床页的需求。这也将消除从用户空间捕捉到内核时对页表切换的需求。这又可以让内核中的系统调用实现利用当前进程的用户内存被映射的优势，让内核代码直接去引用用户指针。很多操作系统都采用了这些思路来提高效率。Xv6避免了这些想法，一是为了减少内核中因无意中使用用户指针而出现安全漏洞的几率，二是为了减少一些复杂性，以保证用户和内核虚拟地址不重叠。

## 4.8 Exercises

1- 函数copyin和copyinstr在软件中走用户页表。设置内核页表，使内核有用户程序映射，copyin和copyinstr可以使用memcpy将系统调用参数复制到内核空间，依靠硬件来完成页表的行走。

2- 实现懒惰的内存分配

3- 实施COW分叉