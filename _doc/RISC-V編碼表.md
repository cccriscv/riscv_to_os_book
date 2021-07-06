# RISC-V 編碼表 (RV32I)

## 指令型態

格式  | 編碼
--|--------------------------------------------
R | f7 rs2 rs1 f3 rd op
I |   i12  rs1 f3 rd op
S | i7 rs2 rs1 f3 i5 op  i7=i[11:5] i5=i[4:0]
B | i7 rs2 rs1 f3 i5 op  i7=i[12|10:5] i5=i[4:1|11]
U |   i20         rd op  i20=i[31:12]
J |   i20         rd op  i20=i[20|10:1|11|19:12]

## RV32I

* https://github.com/riscv/riscv-opcodes/blob/master/opcodes-rv32i

格式 | 指令              | op      | f3  | f7      |
-----|------------------|---------|-----|---------|-------
R | SLL  rd, rs1, rs2   | 0110011 | 001 | 0000000
I | SLLI rd, rs1, shamt | 0010011 | 001 | 0000000
R | SRL  rd, rs1, rs2   | 0110011 | 101 | 0000000
I | SRLI rd, rs1, shamt | 0010011 | 101 | 0000000
R | SRA  rd, rs1, rs2   | 0110011 | 101 | 0100000
I | SRAI rd, rs1, shamt | 0010011 | 101 | 0100000
R | ADD  rd, rs1, rs2   | 0110011 | 000 | 0000000
I | ADDI rd, rs1, imm   | 0010011 | 000 |
R | SUB  rd, rs1, rs2   | 0110011 | 000 | 0100000
U | LUI  rd, imm        | 0110111 | |
U | AUIPC rd, imm       | 0010111 | |
R | XOR  rd, rs1, rs2   | 0110011 | 100 | 0000000
I | XORI rd, rs1, imm   | 0010011 | 100 |
R | OR   rd, rs1, rs2   | 0110011 | 110 | 0000000
I | ORI  rd, rs1, imm   | 0010011 | 110 |
R | AND  rd, rs1, rs2   | 0110011 | 111 | 0000000
I | ANDI rd, rs1, imm   | 0010011 | 111 |
R | SLT  rd, rs1, rs2   | 0110011 | 010 | 0000000
I | SLTI rd, rs1, imm   | 0010011 | 010 |
R | SLTU rd, rs1, rs2   | 0110011 | 011 | 0000000
I | SLTIU rd, rs1, imm  | 0010011 | 011 |
B | BEQ rs1, rs2, imm   | 1100011 | 000 |
B | BNE rs1, rs2, imm   | 1100011 | 001 |
B | BLT rs1, rs2, imm   | 1100011 | 100 |
B | BGE rs1, rs2, imm   | 1100011 | 101 | 
B | BLTU rs1, rs2, imm  | 1100011 | 110 | 
B | BGEU rs1, rs2, imm  | 1100011 | 111 |
J | JAL  rd, imm        | 1101111 | |
I | JALR rd, rs1, imm   | 1100111| |
I | FENCE               | 0001111 | 000 |
I | FENCE.I             | 0001111 | 001 |
I | ECALL               | 1110011 | 000 |          | 000000000000
I | EBREAK              | 1110011 | 000 |          | 000000000001
I | CSRRW rd, csr, rs1  | 1110011 | 001 |
I | CSRRS rd, csr, rs1  | 1110011 | 010 |
I | CSRRC rd, csr, rs1  | 1110011 | 011 |
I | CSRRWI rd, csr, imm | 1110011 | 101 |  // 把 rs1 改為 imm (只有五位元的無號整數)
I | CSRRSI rd, csr, imm | 1110011 | 110 |
I | CSRRCI rd, csr, imm | 1110011 | 111 |
I | LB   rd, rs1, imm   | 0000011 | 000 |
I | LH   rd, rs1, imm   | 0000011 | 001 |
I | LW   rd, rs1, imm   | 0000011 | 010 |
I | LBU  rd, rs1, imm   | 0000011 | 100 |
I | LHU  rd, rs1, imm   | 0000011 | 101 |
S | SB   rs1, rs2, imm  | 0100011 | 000 |
S | SH   rs1, rs2, imm  | 0100011 | 001 |
S | SW   rs1, rs2, imm  | 0100011 | 010 |

## RV32M

格式 | 指令
-----|--------------------
R | MUL  rd, rs1, rs2
R | MULH rd, rs1, rs2
R | MULHSU rd, rs1, rs2
R | MULHU rd, rs1, rs2
R | DIV  rd, rs1, rs2
R | DIVU rd, rs1, rs2
R | REM  rd, rs1, rs2
R | REMU rd, rs1, rs2


## 特權

格式 | 指令
-----|--------------------
R | MRET
R | SRET
R | WFI
R | SFENCE.WMA rs1, rs2
R | SRAW  rd, rs1, rs2
I | SRAIW rd, rs1, shamt
R | ADDW  rd, rs1, rs2
I | ADDIW rd, rs1, imm
R | SUBW rd, rs1, rs2
I | LWU  rd, rs1, imm
I | LD   rd, rs1, imm
S | SD   rs1, rs2, imm

## 原子

格式 | 指令
-----|--------------------
R | LR.W rd, rs1
R | SC.W rd, rs1, rs2
R | AMOSWAP.W rd, rs1, rs2
R | AMOXOR.W  rd, rs1, rs2
R | AMOAND.W  rd, rs1, rs2
R | AMOMINU.W  rd, rs1, rs2
R | AMOMAXU.W  rd, rs1, rs2

## 浮點

格式 | 指令
-----|--------------------
R | FMV.W.X rd, rs1
R | FMV.X.W rd, rs1
R | FCVT.{S|D}.W  rd, rs1
R | FCVT.{S|D}.WU rd, rs1
R | FCVT.W.{S|D}  rd, rs1
R | FCVT.WU.{S|D} rd, rs1
R | FL.{W,D}.W rd, rs1, imm
R | FS.{W,D}.W rs1, rs2, imm
R | FADD.{S|D} rd, rs1, rs2
R | FSUB.{S|D} rd, rs1, rs2
R | FMUL.{S|D} rd, rs1, rs2
R | FDIV.{S|D} rd, rs1, rs2
R | FSQRT.{S|D} rd, rs1
R | FMADD.{S|D} rd, rs1, rs2, rs3
R | FMSUB.{S|D} rd, rs1, rs2, rs3
R | FNMSUB.{S|D} rd, rs1, rs2, rs3
R | FNMADD.{S|D} rd, rs1, rs2, rs3
R | FSGNJ.{S|D} rd, rs1, rs2
R | FSGNJN.{S|D} rd, rs1, rs2
R | FSGNJX.{S|D} rd, rs1, rs2
R | FMIN.{S|D} rd, rs1, rs2
R | FMAX.{S|D} rd, rs1, rs2
R | FEQ.{S|D} rd, rs1, rs2
R | FLT.{S|D} rd, rs1, rs2
R | FLE.{S|D} rd, rs1, rs2
R | FCLASS.{S|D} rd, rs1
R | FRCSR rd
R | FRPM rd
R | FRFLAGS rd
R | FSCSR rd, rs1
R | FSRM rd, rs1
R | FSFLAGS rd, rs1
R | FSRMI rd, imm
R | FSFLAGSI rd, imm

## 向量

格式 | 指令
-----|--------------------
R | SETVL rd, rs1
R | VMULH rd, rs1, rs2
R | VREM rd, rs1, rs2
R | VSLL rd, rs1, rs2
R | VSRL rd, rs1, rs2
R | VSRA rd, rs1, rs2
I | VLD rd, rs1, imm
R | VLDS rd, rs1, rs2
R | VLDX rd, rs1, rs2
S | VST rd, rs1, imm
R | VSTS rd, rs1, rs2
R | VSTX rd, rs1, rs2
R | AMOSWAP rd, rs1, rs2
R | AMOADD rd, rs1, rs2
R | AMOXOR rd, rs1, rs2
R | AMOAND rd, rs1, rs2
R | AMOOR rd, rs1, rs2
R | AMOMIN rd, rs1, rs2
R | AMOMAX rd, rs1, rs2
R | VPEQ rd, rs1, rs2
R | VPNE rd, rs1, rs2
R | VPLT rd, rs1, rs2
R | VPGE rd, rs1, rs2
R | VPAND rd, rs1, rs2
R | VPANDN rd, rs1, rs2
R | VPOR rd, rs1, rs2
R | VPXOR rd, rs1, rs2
R | VPNOT rd, rs1, rs2
R | VPSWAP rd, rs1, rs2
R | VMOV rd, rs1, rs2
R | VCVT rd, rs1, rs2
R | VADD rd, rs1, rs2
R | VSUB rd, rs1, rs2
R | VMUL rd, rs1, rs2
R | VDIV rd, rs1, rs2
R | VSQRT rd, rs1, rs2
R | VFMADD rd, rs1, rs2
R | VFMSUB rd, rs1, rs2
R | VFNMSUB rd, rs1, rs2
R | VFNMADD rd, rs1, rs2
R | VSGNJ rd, rs1, rs2
R | VSGNJN rd, rs1, rs2
R | VSGNJX rd, rs1, rs2
R | VMIN rd, rs1, rs2
R | VMAX rd, rs1, rs2
R | VXOR rd, rs1, rs2
R | VOR  rd, rs1, rs2
R | VAND rd, rs1, rs2
R | VCLASS rd, rs1
R | VSETDCFG rd, rs1
R | VEXTRACT rd, rs1, rs2
R | VMERGE rd, rs1, rs2
R | VSELECT rd, rs1, rs2

## RV64I

格式 | 指令
-----|--------------------
R | SLLW  rd, rs1, rs2
I | SLLIW rd, rs1, shamt
R | SRLW  rd, rs1, rs2
I | SRLIW rd, rs1, shamt
R | SRAW  rd, rs1, rs2
I | SRAIW rd, rs1, shamt
R | ADDW  rd, rs1, rs2
I | ADDIW rd, rs1, imm
R | SUBW rd, rs1, rs2
I | LWU  rd, rs1, imm
I | LD   rd, rs1, imm
S | SD   rs1, rs2, imm

## 特權暫存器

* https://people.eecs.berkeley.edu/~krste/papers/riscv-privileged-v1.9.1.pdf

2.2 CSR Listing

Number Privilege Name Description
User Trap Setup
0x000 URW ustatus User status register.
0x004 URW uie User interrupt-enable register.
0x005 URW utvec User trap handler base address.
User Trap Handling
0x040 URW uscratch Scratch register for user trap handlers.
0x041 URW uepc User exception program counter.
0x042 URW ucause User trap cause.
0x043 URW ubadaddr User bad address.
0x044 URW uip User interrupt pending.
User Floating-Point CSRs
0x001 URW fflags Floating-Point Accrued Exceptions.
0x002 URW frm Floating-Point Dynamic Rounding Mode.
0x003 URW fcsr Floating-Point Control and Status Register (frm + fflags).
User Counter/Timers
0xC00 URO cycle Cycle counter for RDCYCLE instruction.
0xC01 URO time Timer for RDTIME instruction.
0xC02 URO instret Instructions-retired counter for RDINSTRET instruction.
0xC03 URO hpmcounter3 Performance-monitoring counter.
0xC04 URO hpmcounter4 Performance-monitoring counter.
.
.
.
0xC1F URO hpmcounter31 Performance-monitoring counter.
0xC80 URO cycleh Upper 32 bits of cycle, RV32I only.
0xC81 URO timeh Upper 32 bits of time, RV32I only.
0xC82 URO instreth Upper 32 bits of instret, RV32I only.
0xC83 URO hpmcounter3h Upper 32 bits of hpmcounter3, RV32I only.
0xC84 URO hpmcounter4h Upper 32 bits of hpmcounter4, RV32I only.
.
.
.
0xC9F URO hpmcounter31h Upper 32 bits of hpmcounter31, RV32I only.

Table 2.2: Currently allocated RISC-V user-level CSR addresses.

Number Privilege Name Description
Supervisor Trap Setup
0x100 SRW sstatus Supervisor status register.
0x102 SRW sedeleg Supervisor exception delegation register.
0x103 SRW sideleg Supervisor interrupt delegation register.
0x104 SRW sie Supervisor interrupt-enable register.
0x105 SRW stvec Supervisor trap handler base address.
Supervisor Trap Handling
0x140 SRW sscratch Scratch register for supervisor trap handlers.
0x141 SRW sepc Supervisor exception program counter.
0x142 SRW scause Supervisor trap cause.
0x143 SRW sbadaddr Supervisor bad address.
0x144 SRW sip Supervisor interrupt pending.
Supervisor Protection and Translation
0x180 SRW sptbr Page-table base register.
Table 2.3: Currently allocated RISC-V supervisor-level CSR addresses.

Number Privilege Name Description
Hypervisor Trap Setup
0x200 HRW hstatus Hypervisor status register.
0x202 HRW hedeleg Hypervisor exception delegation register.
0x203 HRW hideleg Hypervisor interrupt delegation register.
0x204 HRW hie Hypervisor interrupt-enable register.
0x205 HRW htvec Hypervisor trap handler base address.
Hypervisor Trap Handling
0x240 HRW hscratch Scratch register for hypervisor trap handlers.
0x241 HRW hepc Hypervisor exception program counter.
0x242 HRW hcause Hypervisor trap cause.
0x243 HRW hbadaddr Hypervisor bad address.
0x244 HRW hip Hypervisor interrupt pending.
Hypervisor Protection and Translation
0x28X TBD TBD TBD.
Table 2.4: Currently allocated RISC-V hypervisor-level CSR addresses.

Number Privilege Name Description
Machine Information Registers
0xF11 MRO mvendorid Vendor ID.
0xF12 MRO marchid Architecture ID.
0xF13 MRO mimpid Implementation ID.
0xF14 MRO mhartid Hardware thread ID.
Machine Trap Setup
0x300 MRW mstatus Machine status register.
0x301 MRW misa ISA and extensions
0x302 MRW medeleg Machine exception delegation register.
0x303 MRW mideleg Machine interrupt delegation register.
0x304 MRW mie Machine interrupt-enable register.
0x305 MRW mtvec Machine trap-handler base address.
Machine Trap Handling
0x340 MRW mscratch Scratch register for machine trap handlers.
0x341 MRW mepc Machine exception program counter.
0x342 MRW mcause Machine trap cause.
0x343 MRW mbadaddr Machine bad address.
0x344 MRW mip Machine interrupt pending.
Machine Protection and Translation
0x380 MRW mbase Base register.
0x381 MRW mbound Bound register.
0x382 MRW mibase Instruction base register.
0x383 MRW mibound Instruction bound register.
0x384 MRW mdbase Data base register.
0x385 MRW mdbound Data bound register.
Table 2.5: Currently allocated RISC-V machine-level CSR addresses.

Number Privilege Name Description
Machine Counter/Timers
0xB00 MRW mcycle Machine cycle counter.
0xB02 MRW minstret Machine instructions-retired counter.
0xB03 MRW mhpmcounter3 Machine performance-monitoring counter.
0xB04 MRW mhpmcounter4 Machine performance-monitoring counter.
.
.
.
0xB1F MRW mhpmcounter31 Machine performance-monitoring counter.
0xB80 MRW mcycleh Upper 32 bits of mcycle, RV32I only.
0xB82 MRW minstreth Upper 32 bits of minstret, RV32I only.
0xB83 MRW mhpmcounter3h Upper 32 bits of mhpmcounter3, RV32I only.
0xB84 MRW mhpmcounter4h Upper 32 bits of mhpmcounter4, RV32I only.
.
.
.
0xB9F MRW mhpmcounter31h Upper 32 bits of mhpmcounter31, RV32I only.
Machine Counter Setup
0x320 MRW mucounteren User-mode counter enable.
0x321 MRW mscounteren Supervisor-mode counter enable.
0x322 MRW mhcounteren Hypervisor-mode counter enable.
0x323 MRW mhpmevent3 Machine performance-monitoring event selector.
0x324 MRW mhpmevent4 Machine performance-monitoring event selector.
.
.
.
0x33F MRW mhpmevent31 Machine performance-monitoring event selector.
Debug/Trace Registers (shared with Debug Mode)
0x7A0 MRW tselect Debug/Trace trigger register select.
0x7A1 MRW tdata1 First Debug/Trace trigger data register.
0x7A2 MRW tdata2 Second Debug/Trace trigger data register.
0x7A3 MRW tdata3 Third Debug/Trace trigger data register.
Debug Mode Registers
0x7B0 DRW dcsr Debug control and status register.
0x7B1 DRW dpc Debug PC.
0x7B2 DRW dscratch Debug scratch register.
Table 2.6: Currently allocated RISC-V machine-level CSR addresses


3 Machine-Level ISA 17
3.1 Machine-Level CSRs . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 17
3.1.8 Virtualization Management Field in mstatus Register . . . . . . . . . . . . . 23
3.1.9 Memory Privilege in mstatus Register . . . . . . . . . . . . . . . . . . . . . . 24
3.1.10 Extension Context Status in mstatus Register . . . . . . . . . . . . . . . . . 24
3.1.11 Machine Trap-Vector Base-Address Register (mtvec) . . . . . . . . . . . . . . 28
3.1.12 Machine Trap Delegation Registers (medeleg and mideleg) . . . . . . . . . . 28
3.1.13 Machine Interrupt Registers (mip and mie) . . . . . . . . . . . . . . . . . . . 29
3.1.14 Machine Timer Registers (mtime and mtimecmp) . . . . . . . . . . . . . . . . 31
3.1.15 Hardware Performance Monitor . . . . . . . . . . . . . . . . . . . . . . . . . . 32
3.1.16 Machine Counter-Enable Registers (m[h|s|u]counteren) . . . . . . . . . . . 33
3.1.17 Machine Scratch Register (mscratch) . . . . . . . . . . . . . . . . . . . . . . 34
3.1.18 Machine Exception Program Counter (mepc) . . . . . . . . . . . . . . . . . . 34
3.1.19 Machine Cause Register (mcause) . . . . . . . . . . . . . . . . . . . . . . . . 35
3.1.20 Machine Bad Address (mbadaddr) Register . . . . . . . . . . . . . . . . . . . 35
3.2 Machine-Mode Privileged Instructions . . . . . . . . . . . . . . . . . . . . . . . . . . 36
3.2.1 Trap-Return Instructions . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 36
3.2.2 Wait for Interrupt . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 37

4 Supervisor-Level ISA 47
4.1 Supervisor CSRs . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 47
4.1.1 Supervisor Status Register (sstatus) . . . . . . . . . . . . . . . . . . . . . . 47
4.1.2 Memory Privilege in sstatus Register . . . . . . . . . . . . . . . . . . . . . . 48
4.1.3 Supervisor Trap Vector Base Address Register (stvec) . . . . . . . . . . . . . 49
4.1.4 Supervisor Interrupt Registers (sip and sie) . . . . . . . . . . . . . . . . . . 49
4.1.5 Supervisor Timers and Performance Counters . . . . . . . . . . . . . . . . . . 50
4.1.6 Supervisor Scratch Register (sscratch) . . . . . . . . . . . . . . . . . . . . . 50
4.1.7 Supervisor Exception Program Counter (sepc) . . . . . . . . . . . . . . . . . 50
4.1.8 Supervisor Cause Register (scause) . . . . . . . . . . . . . . . . . . . . . . . 51
4.1.9 Supervisor Bad Address (sbadaddr) Register . . . . . . . . . . . . . . . . . . 51
4.1.10 Supervisor Page-Table Base Register (sptbr) . . . . . . . . . . . . . . . . . . 52