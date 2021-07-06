# Step 8 -- Interrupts

* https://github.com/d0iasm/rvemu-for-book/blob/master/step08/src/trap.rs
* https://github.com/d0iasm/rvemu-for-book/blob/master/step08/src/cpu.rs

```rust
    pub fn check_pending_interrupt(&mut self) -> Option<Interrupt> {
        // 3.1.6.1 Privilege and Global Interrupt-Enable Stack in mstatus register
        // "When a hart is executing in privilege mode x, interrupts are globally enabled when x
        // IE=1 and globally disabled when x IE=0."
        match self.mode {
            Mode::Machine => {
                // Check if the MIE bit is enabled.
                if (self.load_csr(MSTATUS) >> 3) & 1 == 0 {
                    return None;
                }
            }
            Mode::Supervisor => {
                // Check if the SIE bit is enabled.
                if (self.load_csr(SSTATUS) >> 1) & 1 == 0 {
                    return None;
                }
            }
            _ => {}
        }

        // Check external interrupt for uart.
        let irq;
        if self.bus.uart.is_interrupting() {
            irq = UART_IRQ;
        } else {
            irq = 0;
        }

        if irq != 0 {
            self.bus
                .store(PLIC_SCLAIM, 32, irq)
                .expect("failed to write an IRQ to the PLIC_SCLAIM");
            self.store_csr(MIP, self.load_csr(MIP) | MIP_SEIP);
        }

        // "An interrupt i will be taken if bit i is set in both mip and mie, and if interrupts are globally enabled.
        // By default, M-mode interrupts are globally enabled if the hart’s current privilege mode is less than
        // M, or if the current privilege mode is M and the MIE bit in the mstatus register is set. If bit i
        // in mideleg is set, however, interrupts are considered to be globally enabled if the hart’s current
        // privilege mode equals the delegated privilege mode (S or U) and that mode’s interrupt enable bit
        // (SIE or UIE in mstatus) is set, or if the current privilege mode is less than the delegated privilege
        // mode."

        let pending = self.load_csr(MIE) & self.load_csr(MIP);

        if (pending & MIP_MEIP) != 0 {
            self.store_csr(MIP, self.load_csr(MIP) & !MIP_MEIP);
            return Some(Interrupt::MachineExternalInterrupt);
        }
        if (pending & MIP_MSIP) != 0 {
            self.store_csr(MIP, self.load_csr(MIP) & !MIP_MSIP);
            return Some(Interrupt::MachineSoftwareInterrupt);
        }
        if (pending & MIP_MTIP) != 0 {
            self.store_csr(MIP, self.load_csr(MIP) & !MIP_MTIP);
            return Some(Interrupt::MachineTimerInterrupt);
        }
        if (pending & MIP_SEIP) != 0 {
            self.store_csr(MIP, self.load_csr(MIP) & !MIP_SEIP);
            return Some(Interrupt::SupervisorExternalInterrupt);
        }
        if (pending & MIP_SSIP) != 0 {
            self.store_csr(MIP, self.load_csr(MIP) & !MIP_SSIP);
            return Some(Interrupt::SupervisorSoftwareInterrupt);
        }
        if (pending & MIP_STIP) != 0 {
            self.store_csr(MIP, self.load_csr(MIP) & !MIP_STIP);
            return Some(Interrupt::SupervisorTimerInterrupt);
        }
        None
    }

```