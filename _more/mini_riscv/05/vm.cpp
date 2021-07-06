#include "dasm.hpp"
#include "io.hpp"

int isDebug = 0;

int32_t load(uint32_t addr, enum BaseType type) {
  if (addr < MMAX) {
    if (type = i8)  return *(int8_t*)&m[addr];
    if (type = u8)  return *(uint8_t*)&m[addr];
    if (type = i16) return *(int16_t*)&m[addr];
    if (type = u16) return *(uint16_t*)&m[addr];
    if (type = i32) return *(int32_t*)&m[addr];
  } else if (addr >= IO_BASE) {
  }
  return -1;
}

void store(uint32_t addr, int32_t value, enum BaseType type) {
  if (addr < MMAX) {
    if (type = i8)  *(int8_t*)&m[addr]=(int8_t)value;
    if (type = i16) *(int16_t*)&m[addr]=(int16_t)value;
    if (type = i32) *(int32_t*)&m[addr]=(int32_t)value;
  } else if (addr >= IO_BASE) {
    if (addr == PUTC) { putc(value, stdout); }
  }
}

void exec(Op *o) {
  int isJump = 0;
  if (o==&oSll) x[rd]=(uint32_t)x[rs1]<<x[rs2];
  else if (o==&oSrl) x[rd]=(uint32_t)x[rs1]>>x[rs2];
  else if (o==&oSra) x[rd]=x[rs1]>>x[rs2];
  else if (o==&oAdd) x[rd]=x[rs1]+x[rs2];
  else if (o==&oSub) x[rd]=x[rs1]-x[rs2];
  else if (o==&oXor) x[rd]=x[rs1]^x[rs2];
  else if (o==&oAnd) x[rd]=x[rs1]&x[rs2];
  else if (o==&oSlli) x[rd]=(uint32_t)x[rs1]<<imm;
  else if (o==&oSrli) x[rd]=(uint32_t)x[rs1]>>imm;
  else if (o==&oSrai) x[rd]=x[rs1]>>imm;
  else if (o==&oAddi) x[rd]=x[rs1]+imm;
  else if (o==&oXori) x[rd]=x[rs1]^imm;
  else if (o==&oAndi) x[rd]=x[rs1]&imm;
  else if (o==&oSlt) x[rd] = (x[rs1] < x[rs2]);
  else if (o==&oSlti) x[rd] = (x[rs1] < imm);
  else if (o==&oSltu) x[rd] = ((uint32_t)x[rs1] < (uint32_t)x[rs2]);
  else if (o==&oSltiu) x[rd] = ((uint32_t)x[rs1] < (uint32_t)imm);
  else if (o==&oLb)  x[rd] = load(x[rs1]+imm, i8);
  else if (o==&oLbu) x[rd] = load(x[rs1]+imm, u8);
  else if (o==&oLh)  x[rd] = load(x[rs1]+imm, i16);
  else if (o==&oLhu) x[rd] = load(x[rs1]+imm, u16);
  else if (o==&oLw)  x[rd] = load(x[rs1]+imm, i32);
  else if (o==&oSb) store(x[rs1]+imm, x[rs2], i8);
  else if (o==&oSh) store(x[rs1]+imm, x[rs2], i16);
  else if (o==&oSw) store(x[rs1]+imm, x[rs2], i32);
  else if (o==&oLui) x[rd]=imm;
  else if (o==&oAuipc) x[rd]=pc+imm;
  else if (o==&oCsrrw) { x[rd]=csrs[csr]; csrs[csr]=x[rs1]; }
  else if (o==&oCsrrs) { x[rd]=csrs[csr]; csrs[csr]|=x[rs1]; }
  else if (o==&oCsrrc) { x[rd]=csrs[csr]; csrs[csr]&=(~x[rs1]); }
  else if (o==&oCsrrwi) { x[rd]=csrs[csr]; csrs[csr]=imm; }
  else if (o==&oCsrrsi) { x[rd]=csrs[csr]; csrs[csr]|=imm; }
  else if (o==&oCsrrci) { x[rd]=csrs[csr]; csrs[csr]&=(~imm); }
  else {
    isJump = 1;
    if (o==&oJal) { x[rd]=pc; pc+= imm; }
    else if (o==&oJalr) { int32_t t=pc+4; pc=x[rs1]+imm; x[rd] = t; } // 保存返回位址然後跳到 x[rs1]+imm
    else if (o==&oBeq) { if (x[rs1] == x[rs2]) pc+=imm; else pc+=4; } // 跳躍位址應該是 pc 加上 sign-extended 13-bit，而非直接設定。
    else if (o==&oBlt) { if (x[rs1] <  x[rs2]) pc+=imm; else pc+=4; }
    else if (o==&oBne) { if (x[rs1] != x[rs2]) pc+=imm; else pc+=4; }
    else if (o==&oBge) { if (x[rs1] >= x[rs2]) pc+=imm; else pc+=4; }
    else if (o==&oBltu) { if ((uint32_t)x[rs1] <  (uint32_t)x[rs2]) pc += imm; else pc+=4; }
    else if (o==&oBgeu) { if ((uint32_t)x[rs1] >= (uint32_t)x[rs2]) pc += imm; else pc+=4; }
    else { printf("ir=%08x : command not found!\n", ir); exit(1); }
  }
  if (!isJump) pc += 4;
  x[0] = 0; // x0 永遠為 0，不可寫入。
}

void vm(uint8_t *m, uint32_t mTop) {
  char sasm[LMAX];
  pc = 0;
  while (1) {
    ir = bytes2word(&m[pc], LittleEndian);
    Op *o = ir2asm(ir, sasm);
    exec(o);
    if (isDebug) printf("%04X:%08X %-30s x[%d]=%d\n", pc, ir, sasm, rd, x[rd]);
    if (pc >= mTop) break;
  }
}

#ifdef __VM__
// run: ./vm <file.bin>
int main(int argc, char *argv[]) {
  if (argc > 2 && strcmp(argv[2], "-d")==0) isDebug = 1;
  riscv_init();
  char *binFileName = argv[1];
  FILE *binFile = fopen(binFileName, "rb");
  mTop = fread(m, sizeof(uint8_t), MMAX, binFile);
  fclose(binFile);
  vm(m, mTop);
  return 0;
}
#endif