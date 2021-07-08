# xv6 作業系統閱讀筆記 (7) 平行與鎖 lock -- 多核心的控管

在多工系統中，為了避免同時存取某變數，會使用鎖 (lock) 來防止競爭情況。

在 xv6 當中，主要使用《旋轉鎖》(spinlock) ，其結構定義在 spinlock.h 裏。

kernel/spinlock.h

```cpp
// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};
```

舉例而言，檔案系統 file.c 裏的 filedup() 函數，就使用了 acquire() 鎖定，然後使用 release() 解鎖。

kernel/file.c

```cpp
// Increment ref count for file f.
struct file*
filedup(struct file *f)
{
  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  return f;
}
```

這樣就可以防止多個行程同時存取檔案表，造成 f->ref 變數因競爭情況而導致不一致的問題。

在 xv6 中 acquire() 與 release() 函數的實作如下：

kernel/spinlock.c

```cpp
// Acquire the lock.
// Loops (spins) until the lock is acquired.
void
acquire(struct spinlock *lk)
{
  push_off(); // disable interrupts to avoid deadlock. 禁止中斷
  if(holding(lk)) // 如果重複鎖定，那就是嚴重錯誤
    panic("acquire");

  // On RISC-V, sync_lock_test_and_set turns into an atomic swap:
  //   a5 = 1
  //   s1 = &lk->locked
  //   amoswap.w.aq a5, a5, (s1)
  while(__sync_lock_test_and_set(&lk->locked, 1) != 0) // 等待直到鎖定成功
    ;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen strictly after the lock is acquired.
  // On RISC-V, this emits a fence instruction.
  __sync_synchronize(); // 要求編譯器載入儲存指令不跨越此邊界

  // Record info about lock acquisition for holding() and debugging.
  lk->cpu = mycpu(); // 設定上鎖者的處理器代號
}

// Release the lock.
void
release(struct spinlock *lk)
{
  if(!holding(lk)) // 如果沒有鎖定就呼叫 release()，那就是嚴重錯誤
    panic("release");

  lk->cpu = 0;

  // Tell the C compiler and the CPU to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other CPUs before the lock is released,
  // and that loads in the critical section occur strictly before
  // the lock is released.
  // On RISC-V, this emits a fence instruction.
  __sync_synchronize(); // 要求編譯器載入儲存指令不跨越此邊界

  // Release the lock, equivalent to lk->locked = 0.
  // This code doesn't use a C assignment, since the C standard
  // implies that an assignment might be implemented with
  // multiple store instructions.
  // On RISC-V, sync_lock_release turns into an atomic swap:
  //   s1 = &lk->locked
  //   amoswap.w zero, zero, (s1)
  __sync_lock_release(&lk->locked); // 解除鎖定

  pop_off(); // 允許中斷
}

```

要避免競爭情況，首先要禁止中斷，所以 acquire() 當中 呼叫了 push_off() 禁止中斷，並在 release() 當中解除禁令。

kernel/spinlock.c

```cpp

// push_off/pop_off are like intr_off()/intr_on() except that they are matched:
// it takes two pop_off()s to undo two push_off()s.  Also, if interrupts
// are initially off, then push_off, pop_off leaves them off.

void
push_off(void)
{
  int old = intr_get();

  intr_off();
  if(mycpu()->noff == 0)
    mycpu()->intena = old;
  mycpu()->noff += 1;
}

void
pop_off(void)
{
  struct cpu *c = mycpu();
  if(intr_get())
    panic("pop_off - interruptible");
  if(c->noff < 1)
    panic("pop_off");
  c->noff -= 1;
  if(c->noff == 0 && c->intena)
    intr_on();
}

```

acquire() 中有些編譯器專屬的特殊展開，像是 `__sync_lock_test_and_set(&lk->locked, 1)` 會被展開成以下 RISC-V 的組合語言

```s
a5 = 1
s1 = &lk->locked
amoswap.w.aq a5, a5, (s1)
```

其中的 amoswap 是原子指令，可以在單一指令內交換暫存器 a5 與記憶體 (s1)，也就是將 lk->locked 設定為 1，透過這樣來達成鎖定的目的。

另外，__sync_synchronize() 也是編譯器指引，告訴編譯器別讓 load, store 等記憶體存取指令跨越該行，以避免編譯器優化造成的競爭存取問題。

release() 中的 `__sync_lock_release(&lk->locked)` 會被展開成以下組合語言：

```s
s1 = &lk->locked
amoswap.w zero, zero, (s1)
```

這樣就能透過原子指令 amoswap 將 lk->locked 設定為 0，完成解鎖動作。

您可能會注意到 acquire() 與 release() 當中都有使用 holding() 函數檢查是否該鎖已經被同一個 CPU 鎖住 (如果是就會導致 panic 恐慌並讓作業系統停止) (panic 類似一般程式的 assert 函數，會停掉並印錯誤訊息)

```cpp
// Check whether this cpu is holding the lock.
// Interrupts must be off.
int
holding(struct spinlock *lk)
{
  int r;
  r = (lk->locked && lk->cpu == mycpu());
  return r;
}
```

上述程式中的 mycpu() 定義在 proc.c 裏

kernel/proc.c

```cpp
// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
int
cpuid()
{
  int id = r_tp();
  return id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
struct cpu*
mycpu(void) {
  int id = cpuid();
  struct cpu *c = &cpus[id];
  return c;
}
```

r_tp() 函數則定義在 riscv.h 當中，呼叫嵌入式組合語言指令 `mv %0, tp` 去讀取 tp (thread pointer 暫存器)，tp 暫存器裏儲存了目前的 hartid (核心代號)。

kernel/riscv.h

```cpp
// read and write tp, the thread pointer, which holds
// this core's hartid (core number), the index into cpus[].
static inline uint64
r_tp()
{
  uint64 x;
  asm volatile("mv %0, tp" : "=r" (x) );
  return x;
}
```

在整個鎖定機制中，RISC-V 處理器的原子指令 amoswap 扮演關鍵角色，關於原子指令的詳細說明可以參考 

* [The RISC-V Instruction Set Manual Volume I: Unprivileged ISA](https://github.com/riscv/riscv-isa-manual/releases/download/Ratified-IMAFDQC/riscv-spec-20191213.pdf) -- Chapter 8 “A” Standard Extension for Atomic Instructions, Version 2.1

其中的 Figure 8.2 展示了使用 amoswap 鎖定與解鎖的組合語言範例：

```s
li t0, 1 # Initialize swap value.
again:
lw t1, (a0) # Check if lock is held.
bnez t1, again # Retry if held.
amoswap.w.aq t1, t0, (a0) # Attempt to acquire lock.
bnez t1, again # Retry if held.
# ...
# Critical section.
# ...
amoswap.w.rl x0, x0, (a0) # Release lock by storing 0.
Figure 8.2: Sample code for mutual exclusion. a0 contains the address of the lock.
```

一旦 acquire()/release() 這樣的函數建構完成後，我們就能在 xv6 核心中運用鎖定機制避免競爭情況，確保臨界區間的存取不會出問題了。

