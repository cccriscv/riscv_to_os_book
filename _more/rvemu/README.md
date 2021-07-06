# RVEMU

* https://github.com/d0iasm/rvemu (RISCV-模擬器) (RUST) 
    * https://github.com/d0iasm/book.rvemu (書，值得參考)
    * https://github.com/d0iasm/rvemu-for-book
    * https://github.com/d0iasm/rvemu-for-youtube

## Cpu

```
/// The CPU to contain registers, a program coutner, status, and a privileged mode.
pub struct Cpu {
    /// 64-bit integer registers.
    pub xregs: XRegisters,
    /// 64-bit floating-point registers.
    pub fregs: FRegisters,
    /// Program coutner.
    pub pc: u64,
    /// Control and status registers (CSR).
    pub state: State,
    /// Privilege level.
    pub mode: Mode,
    /// Previous privilege level.
    pub prev_mode: Mode,
    /// System bus.
    pub bus: Bus,
    /// SV39 paging flag.
    pub enable_paging: bool,
    /// physical page number (PPN) × PAGE_SIZE (4096).
    pub page_table: u64,
}
```

## io

* https://www.sifive.com/blog/risc-v-qemu-part-1-privileged-isa-hifive1-virtio
* https://www.ovpworld.org/library/wikka.php?wakka=VirtIO
* https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c
* https://github.com/d0iasm/rvemu/blob/master/src/devices/virtio.rs