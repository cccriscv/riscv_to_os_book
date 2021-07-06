# Step 1 -- Setup and Implement Two Instructions

```rust
/// The CPU to contain registers, a program coutner, and memory.
struct Cpu {
    /// 32 64-bit integer registers.
    regs: [u64; 32],
    /// Program counter to hold the the memory address of the next instruction that would be executed.
    pc: u64,
    /// Computer memory to store executable instructions.
    memory: Vec<u8>,
}
```

