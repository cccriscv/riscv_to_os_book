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

void exec() {
  int isJump = 0;
  switch (id) {
    case Sll: x[rd]=(uint32_t)x[rs1]<<x[rs2]; break;
    case Srl: x[rd]=(uint32_t)x[rs1]>>x[rs2]; break;
    case Sra: x[rd]=x[rs1]>>x[rs2]; break;
    case Add: x[rd]=x[rs1]+x[rs2]; break;
    case Sub: x[rd]=x[rs1]-x[rs2]; break;
    case Xor: x[rd]=x[rs1]^x[rs2]; break;
    case And: x[rd]=x[rs1]&x[rs2]; break;
    case Slli: x[rd]=(uint32_t)x[rs1]<<imm; break;
    case Srli: x[rd]=(uint32_t)x[rs1]>>imm; break;
    case Srai: x[rd]=x[rs1]>>imm; break;
    case Addi: x[rd]=x[rs1]+imm; break;
    case Xori: x[rd]=x[rs1]^imm; break;
    case Andi: x[rd]=x[rs1]&imm; break;
    case Slt: x[rd] = (x[rs1] < x[rs2]); break;
    case Slti: x[rd] = (x[rs1] < imm); break;
    case Sltu: x[rd] = ((uint32_t)x[rs1] < (uint32_t)x[rs2]); break;
    case Sltiu: x[rd] = ((uint32_t)x[rs1] < (uint32_t)imm); break;
    case Lb:  x[rd] = load(x[rs1]+imm, i8); break;
    case Lbu: x[rd] = load(x[rs1]+imm, u8); break;
    case Lh:  x[rd] = load(x[rs1]+imm, i16); break;
    case Lhu: x[rd] = load(x[rs1]+imm, u16); break;
    case Lw:  x[rd] = load(x[rs1]+imm, i32); break;
    case Sb: store(x[rs1]+imm, x[rs2], i8); break;
    case Sh: store(x[rs1]+imm, x[rs2], i16); break;
    case Sw: store(x[rs1]+imm, x[rs2], i32); break;
    case Lui: x[rd]=imm; break;
    case Auipc: x[rd]=pc+imm; break;
    case Csrrw: x[rd]=csrs[csr]; csrs[csr]=x[rs1];  break;
    case Csrrs: x[rd]=csrs[csr]; csrs[csr]|=x[rs1]; break;
    case Csrrc: x[rd]=csrs[csr]; csrs[csr]&=(~x[rs1]); break;
    case Csrrwi: x[rd]=csrs[csr]; csrs[csr]=imm; break;
    case Csrrsi: x[rd]=csrs[csr]; csrs[csr]|=imm; break;
    case Csrrci: x[rd]=csrs[csr]; csrs[csr]&=(~imm); break;
    default:
      isJump = 1;
      int32_t t;
      switch (id) {
        case Jal: x[rd]=pc; pc+= imm; break;
        case Jalr: t=pc+4; pc=x[rs1]+imm; x[rd] = t; break; // 保存返回位址然後跳到 x[rs1]+imm
        case Beq: if (x[rs1] == x[rs2]) pc+=imm; else pc+=4; break; // 跳躍位址應該是 pc 加上 sign-extended 13-bit，而非直接設定。
        case Blt: if (x[rs1] <  x[rs2]) pc+=imm; else pc+=4; break;
        case Bne: if (x[rs1] != x[rs2]) pc+=imm; else pc+=4; break;
        case Bge: if (x[rs1] >= x[rs2]) pc+=imm; else pc+=4; break;
        case Bltu: if ((uint32_t)x[rs1] <  (uint32_t)x[rs2]) pc += imm; else pc+=4; break;
        case Bgeu: if ((uint32_t)x[rs1] >= (uint32_t)x[rs2]) pc += imm; else pc+=4; break;
        default: ERROR("ir=%08x : command not found!\n", ir);
      }
  }
  if (!isJump) pc += 4;
  x[0] = 0; // x0 永遠為 0，不可寫入。
}

void vm(uint8_t *m, uint32_t mTop) {
  char sasm[LMAX];
  pc = 0;
  while (1) {
    ir = bytes2word(&m[pc], LittleEndian);
    uint32_t pc0 = pc;
    ir2asm(ir, sasm);
    exec();
    if (isDebug) printf("%04X:%08X %-30s x[%d]=%d\n", pc0, ir, sasm, rd, x[rd]);
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
