#ifndef __CPU_H__
#define __CPU_H__

#include "riscv.hpp"

extern Id id;
extern uint32_t pc, ir, op, f3, f7, rd, rs1, rs2, csr, mTop;
extern int32_t imm;
extern char type;
extern const char *name;
extern uint32_t x[];
extern float f[];
extern uint8_t m[];
extern uint32_t csrs[];

extern void decode(uint32_t ir);


#endif
