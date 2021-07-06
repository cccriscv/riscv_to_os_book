# Step 4 -- Privileged Instruction Set

* https://github.com/d0iasm/rvemu-for-book/blob/master/step04/src/cpu.rs

```rust
            0x73 => {
                let csr_addr = ((inst & 0xfff00000) >> 20) as usize;
                match funct3 {
                    0x0 => {
                        match (rs2, funct7) {
                            (0x2, 0x8) => {
                                // sret
                                // The SRET instruction returns from a supervisor-mode exception
                                // handler. It does the following operations:
                                // - Sets the pc to CSRs[sepc].
                                // - Sets the privilege mode to CSRs[sstatus].SPP.
                                // - Sets CSRs[sstatus].SIE to CSRs[sstatus].SPIE.
                                // - Sets CSRs[sstatus].SPIE to 1.
                                // - Sets CSRs[sstatus].SPP to 0.
                                self.pc = self.load_csr(SEPC);
                                // When the SRET instruction is executed to return from the trap
                                // handler, the privilege level is set to user mode if the SPP
                                // bit is 0, or supervisor mode if the SPP bit is 1. The SPP bit
                                // is the 8th of the SSTATUS csr.
                                self.mode = match (self.load_csr(SSTATUS) >> 8) & 1 {
                                    1 => Mode::Supervisor,
                                    _ => Mode::User,
                                };
                                // The SPIE bit is the 5th and the SIE bit is the 1st of the
                                // SSTATUS csr.
                                self.store_csr(
                                    SSTATUS,
                                    if ((self.load_csr(SSTATUS) >> 5) & 1) == 1 {
                                        self.load_csr(SSTATUS) | (1 << 1)
                                    } else {
                                        self.load_csr(SSTATUS) & !(1 << 1)
                                    },
                                );
                                self.store_csr(SSTATUS, self.load_csr(SSTATUS) | (1 << 5));
                                self.store_csr(SSTATUS, self.load_csr(SSTATUS) & !(1 << 8));
                            }
                            (0x2, 0x18) => {
                                // mret
                                // The MRET instruction returns from a machine-mode exception
                                // handler. It does the following operations:
                                // - Sets the pc to CSRs[mepc].
                                // - Sets the privilege mode to CSRs[mstatus].MPP.
                                // - Sets CSRs[mstatus].MIE to CSRs[mstatus].MPIE.
                                // - Sets CSRs[mstatus].MPIE to 1.
                                // - Sets CSRs[mstatus].MPP to 0.
                                self.pc = self.load_csr(MEPC);
                                // MPP is two bits wide at [11..12] of the MSTATUS csr.
                                self.mode = match (self.load_csr(MSTATUS) >> 11) & 0b11 {
                                    2 => Mode::Machine,
                                    1 => Mode::Supervisor,
                                    _ => Mode::User,
                                };
                                // The MPIE bit is the 7th and the MIE bit is the 3rd of the
                                // MSTATUS csr.
                                self.store_csr(
                                    MSTATUS,
                                    if ((self.load_csr(MSTATUS) >> 7) & 1) == 1 {
                                        self.load_csr(MSTATUS) | (1 << 3)
                                    } else {
                                        self.load_csr(MSTATUS) & !(1 << 3)
                                    },
                                );
                                self.store_csr(MSTATUS, self.load_csr(MSTATUS) | (1 << 7));
                                self.store_csr(MSTATUS, self.load_csr(MSTATUS) & !(0b11 << 11));
                            }
                            (_, 0x9) => {
                                // sfence.vma
                                // Do nothing.
                            }
                            _ => {
                                println!(
                                    "not implemented yet: opcode {:#x} funct3 {:#x} funct7 {:#x}",
                                    opcode, funct3, funct7
                                );
                                return Err(());
                            }
                        }
                    }

```