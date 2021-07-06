#ifndef __RISCV_H__
#define __RISCV_H__

#include "lib.hpp"

#define MMAX 32768

enum Id { None, Sll, Slli, Srl, Srli, Sra, Srai, Add, Addi, Sub, Subi, Xor, Xori, Or, Ori, And, Andi,
Slt, Slti, Sltu, Sltiu, Beq, Bne, Blt, Bge, Bltu, Bgeu, Jal, Jalr, Lb, Lh, Lw, Lhu, Lbu, Sb, Sh, Sw,
Csrrw, Csrrs, Csrrc, Csrrwi, Csrrsi, Csrrci, Lui, Auipc, Fence, Fencei, Ecall, Ebreak };

typedef struct _Op {
  enum Id id;
  string name;
  char type;
  uint32_t op, f3, f7;
} Op;

// Machine-level CSRs.
#define MHARTID 0xf14 // Hardware thread ID.
#define MSTATUS 0x300 // Machine status register.
#define MEDELEG 0x302 // Machine exception delefation register.
#define MIDELEG 0x303 // Machine interrupt delefation register.
#define MIE 0x304 // Machine interrupt-enable register.
#define MTVEC 0x305 // Machine trap-handler base address.
#define MCOUNTEREN 0x306 // Machine counter enable.
#define MSCRATCH 0x340 // Scratch register for machine trap handlers.
#define MEPC 0x341 // Machine exception program counter.
#define MCAUSE 0x342 // Machine trap cause.
#define MTVAL 0x343 // Machine bad address or instruction.
#define MIP 0x344 // Machine interrupt pending.

// Supervisor-level CSRs.
#define SSTATUS 0x100 // Supervisor status register.
#define SIE 0x104 // Supervisor interrupt-enable register.
#define STVEC 0x105 // Supervisor trap handler base address.
#define SSCRATCH 0x140 // Scratch register for supervisor trap handlers.
#define SEPC 0x141 // Supervisor exception program counter.
#define SCAUSE 0x142 // Supervisor trap cause.
#define STVAL 0x143 // Supervisor bad address or instruction.
#define SIP 0x144 // Supervisor interrupt pending.
#define SATP 0x180 // Supervisor address translation and protection.

extern map<enum Id, Op*> opIdMap;
extern map<string, Op*> opMap;
extern map<string, int> rMap;
extern map<string, int> csrMap;
extern map<int, string> i2csrMap;

extern Op* opFind(char *name);
extern int rFind(char *name);
extern int csrFind(char *name);
extern const char *csrName(uint32_t csr);
extern void riscv_init();

#endif


/* 指令型態
  | R, I, S, B, U, J
--|--------------------------------------------
R | f7 rs2 rs1 f3 rd op
I |   i12  rs1 f3 rd op
S | i7 rs2 rs1 f3 i5 op  i7=i[11:5] i5=i[4:0]
B | i7 rs2 rs1 f3 i5 op  i7=i[12|10:5] i5=i[4:1|11]
U |   i20         rd op  i20=i[31:12]
J |   i20         rd op  i20=i[20|10:1|11|19:12]
*/

/* 指令表
U: lui rd,imm`: `rd = imm * 2^12; pc = pc + 4` with `-2^19 <= imm < 2^19`
I: addi rd,rs1,imm`: `rd = rs1 + imm; pc = pc + 4` with `-2^11 <= imm < 2^11`
I: lw?? ld rd,imm(rs1)`: `rd = memory[rs1 + imm]; pc = pc + 4` with `-2^11 <= imm < 2^11`
I: sw?? sd rs2,imm(rs1)`: `memory[rs1 + imm] = rs2; pc = pc + 4` with `-2^11 <= imm < 2^11`
R: add rd,rs1,rs2`: `rd = rs1 + rs2; pc = pc + 4`   op:0110011 f7:0000000 
R: sub rd,rs1,rs2`: `rd = rs1 - rs2; pc = pc + 4`   op:0110011 f7:0100000
R: mul rd,rs1,rs2`: `rd = rs1 * rs2; pc = pc + 4`
R: divu rd,rs1,rs2`: `rd = rs1 / rs2; pc = pc + 4` where `rs1` and `rs2` are unsigned integers.
R: remu rd,rs1,rs2`: `rd = rs1 % rs2; pc = pc + 4` where `rs1` and `rs2` are unsigned integers.
R: sltu rd,rs1,rs2`: `if (rs1 < rs2) { rd = 1 } else { rd = 0 } pc = pc + 4` where `rs1` and `rs2` are unsigned integers.
B: beq rs1,rs2,imm`: `if (rs1 == rs2) { pc = pc + imm } else { pc = pc + 4 }` with `-2^12 <= imm < 2^12` and `imm % 2 == 0`
J: jal rd,imm`: `rd = pc + 4; pc = pc + imm` with `-2^20 <= imm < 2^20` and `imm % 2 == 0`
I: jalr rd,imm(rs1)`: `tmp = ((rs1 + imm) / 2) * 2; rd = pc + 4; pc = tmp` with `-2^11 <= imm < 2^11`
I: ecall`: system call number is in `a7`, parameters are in `a0-a2`, return value is in `a0`.
*/
