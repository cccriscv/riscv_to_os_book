# Step 9 -- Virtio

* https://github.com/d0iasm/rvemu-for-book/blob/master/step09/src/virtio.rs


* https://github.com/d0iasm/rvemu-for-book/blob/master/step09/src/bus.rs

```rust
/// The system bus.
pub struct Bus {
    clint: Clint,
    plic: Plic,
    pub uart: Uart,
    pub virtio: Virtio,
    memory: Memory,
}


impl Bus {
    /// Create a new system bus object.
    pub fn new(binary: Vec<u8>, disk_image: Vec<u8>) -> Bus {
        Self {
            clint: Clint::new(),
            plic: Plic::new(),
            uart: Uart::new(),
            virtio: Virtio::new(disk_image),
            memory: Memory::new(binary),
        }
    }

    pub fn load(&mut self, addr: u64, size: u64) -> Result<u64, Exception> {
        if CLINT_BASE <= addr && addr < CLINT_BASE + CLINT_SIZE {
            return self.clint.load(addr, size);
        }
        if PLIC_BASE <= addr && addr < PLIC_BASE + PLIC_SIZE {
            return self.plic.load(addr, size);
        }
        if UART_BASE <= addr && addr < UART_BASE + UART_SIZE {
            return self.uart.load(addr, size);
        }
        if VIRTIO_BASE <= addr && addr < VIRTIO_BASE + VIRTIO_SIZE {
            return self.virtio.load(addr, size);
        }
        if MEMORY_BASE <= addr {
            return self.memory.load(addr, size);
        }
        Err(Exception::LoadAccessFault)
    }

    pub fn store(&mut self, addr: u64, size: u64, value: u64) -> Result<(), Exception> {
        if CLINT_BASE <= addr && addr < CLINT_BASE + CLINT_SIZE {
            return self.clint.store(addr, size, value);
        }
        if PLIC_BASE <= addr && addr < PLIC_BASE + PLIC_SIZE {
            return self.plic.store(addr, size, value);
        }
        if UART_BASE <= addr && addr < UART_BASE + UART_SIZE {
            return self.uart.store(addr, size, value);
        }
        if VIRTIO_BASE <= addr && addr < VIRTIO_BASE + VIRTIO_SIZE {
            return self.virtio.store(addr, size, value);
        }
        if MEMORY_BASE <= addr {
            return self.memory.store(addr, size, value);
        }
        Err(Exception::StoreAMOAccessFault)
    }
}
```

* https://github.com/d0iasm/rvemu-for-book/blob/master/step09/src/cpu.rs

```rust
        // Check external interrupt for uart and virtio.
        let irq;
        if self.bus.uart.is_interrupting() {
            irq = UART_IRQ;
        } else if self.bus.virtio.is_interrupting() {
            // Access disk by direct memory access (DMA). An interrupt is raised after a disk
            // access is done.
            Virtio::disk_access(self);
            irq = VIRTIO_IRQ;
        } else {
            irq = 0;
        }
```

