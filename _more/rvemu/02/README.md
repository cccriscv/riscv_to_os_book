# rvemu 02 -- RV64I Base Integer Instruction Set

Cpu + Bus + Memory

Cpu 透過 bus 可以存取 Memory

```rust
/// The system bus.
pub struct Bus {
    memory: Memory,
}

/// The `Cpu` struct that contains registers, a program coutner, system bus that connects
/// peripheral devices, and control and status registers.
pub struct Cpu {
    /// 32 64-bit integer registers.
    pub regs: [u64; 32],
    /// Program counter to hold the the memory address of the next instruction that would be executed.
    pub pc: u64,
    /// System bus that transfers data between CPU and peripheral devices.
    pub bus: Bus,
}
```


