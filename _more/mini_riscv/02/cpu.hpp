#ifndef __CPU_H__
#define __CPU_H__

#include "riscv.hpp"

extern uint32_t pc, ir, op, f3, f7, rd, rs1, rs2, mTop;
extern int32_t imm;
extern char type;
extern const char *name;
extern uint32_t x[];
extern float f[];
extern uint8_t m[];

extern Op *decode(uint32_t ir);
extern void exec(Op *o);
extern void dumpInstr();
extern void wordToBytes(uint32_t i32, uint8_t bytes[]);
extern uint32_t bytesToWord(uint8_t bytes[]);

#endif
