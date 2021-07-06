# Step 6 -- PLIC (a platform-level interrupt controller) and CLINT (a core-local interrupter)

* https://github.com/d0iasm/rvemu-for-book/blob/master/step06/src/plic.rs
* https://github.com/d0iasm/rvemu-for-book/blob/master/step06/src/clint.rs

* https://github.com/d0iasm/rvemu-for-book/blob/master/step06/src/bus.rs

```rust
    pub fn load(&self, addr: u64, size: u64) -> Result<u64, Exception> {
        if CLINT_BASE <= addr && addr < CLINT_BASE + CLINT_SIZE {
            return self.clint.load(addr, size);
        }
        if PLIC_BASE <= addr && addr < PLIC_BASE + PLIC_SIZE {
            return self.plic.load(addr, size);
        }
        if MEMORY_BASE <= addr {
            return self.memory.load(addr, size);
        }
        Err(Exception::LoadAccessFault)
    }
```