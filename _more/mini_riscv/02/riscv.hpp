#ifndef __RISCV_H__
#define __RISCV_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <iostream> 
#include <map>

using namespace std;

#define SMAX 100
#define LMAX 256
#define TMAX 10000
#define MMAX 32768

typedef struct _Op {
  string name;
  char type;
  uint32_t op, f3, f7;
} Op;

extern Op oNull,oSll,oSlli,oSrl,oSrli,oSra,oSrai,oAdd,oAddi,oSub,oLui,oAuipc,
oXor,oXori,oOr,oOri,oAnd,oAndi,oSlt,oSlti,oSltu,oSltiu,oBeq,oBne,oBlt,oBge,oBltu,
oBgeu,oJal,oJalr,oFence,oFenceI,oEcall,oEbreak,oCsrrw,oCsrrs,oCsrrc,oCsrrwi,oCsrrsi,
oCsrrci,oLb,oLh,oLw,oLbu,oLhu,oSb,oSh,oSw;
extern map<string, Op> opMap;
extern map<string, int> rMap;
extern map<string, int> csrMap;

#define BITS(x, from, to) ((uint32_t)(x)<<(32-(to+1)))>>(32-(to-from+1)) // 包含 to, 用 BITS(x, 1, 30) 去思考
#define SGN(x, i) (BITS(x,i,i)==0)? x : (-(1<<i)+(int32_t)BITS(x, 0, i-1))+1 // x[i] 為 sign bit，轉為整數。

extern int mapFind(map<string, int> *iMap, char *name);
extern void mapDump(map<string, int> *map);
extern Op opFind(char *name);
extern int rFind(char *name);
extern void wordToBytes(uint32_t word, uint8_t bytes[]);
uint32_t bytesToWord(uint8_t bytes[]);

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
