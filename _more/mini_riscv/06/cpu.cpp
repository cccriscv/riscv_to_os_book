#include "cpu.hpp"

Id id;
uint32_t pc, ir, op, f3, f7, rd, rs1, rs2, csr, mTop;
int32_t imm;
char type;
const char *name;
uint32_t x[32];
float f[32];
uint8_t m[MMAX];
uint32_t csrs[4096];

void decode(uint32_t ir) {
  id=None;
  op=BITS(ir, 0, 6); 
  f3=BITS(ir, 12, 14);
  f7=BITS(ir, 25, 31);
  rd=BITS(ir, 7, 11);
  rs1=BITS(ir, 15, 19);
  rs2=BITS(ir, 20, 24);
  csr=0;
  imm=0;
  switch (op) {
    case 0x03:
      switch (f3) {
        case 0: id = Lb; break;
        case 1: id = Lh; break;
        case 2: id = Lw; break;
        case 4: id = Lbu; break;
        case 5: id = Lhu; break;
      }
      break;
    case 0x0F:
      switch (f3) {
        case 0: id = Fence; break;
        case 1: id = Fencei; break;
      }
      break;
    case 0x13:
      switch (f3) {
        case 0: id = Addi; break;
        case 1: id = Slli; break;
        case 2: id = Slti; break;
        case 3: id = Sltiu; break;
        case 4: id = Xori; break;
        case 5: id = (f7==0)? Srli : Srai; break;
        case 6: id = Ori; break;
        case 7: id = Andi; break;
      }
      break;
    case 0x17:
      id = Auipc;
      break;
    case 0x23:
      switch (f3) {
        case 0: id = Sb; break;
        case 1: id = Sh; break;
        case 2: id = Sw; break;
      }
      break;
    case 0x33:
      switch (f3) {
        case 0: id = (f7==0)? Add : Sub; break;
        case 1: id = Sll; break;
        case 2: id = Slt; break;
        case 3: id = Sltu; break;
        case 4: id = Xor; break;
        case 5: id = (f7==0)? Srl : Sra; break;
        case 6: id = Or; break;
        case 7: id = And; break;
      }
      break;
    case 0x37: // lui
      id = Lui;
      break;
    case 0x63:
      switch (f3) {
        case 0: id = Beq; break;
        case 1: id = Bne; break;
        case 4: id = Blt; break;
        case 5: id = Bge; break;
        case 6: id = Bltu; break;
        case 7: id = Bgeu; break;
      }
      break;
    case 0x67: id = Jalr; break;
    case 0x6F: id = Jal; break;
    case 0x73:
      switch (f3) {
        case 0: 
          if (ir == 0x00000073) id = Ecall;
          if (ir == 0x00100073) id = Ebreak;
          break;
        case 1: id = Csrrw; break;
        case 2: id = Csrrs; break;
        case 3: id = Csrrc; break;
        case 5: id = Csrrwi; break;
        case 6: id = Csrrsi; break;
        case 7: id = Csrrci; break;
      }
      break;
  }
  if (id != None) {
    Op *o = opIdMap[id];
    name = o->name.c_str();
    type = o->type;
    uint32_t t;
    switch (type) {
      case 'R': break;
      case 'I':
        if (op == 0x73) { // ex: csrrw, csrrwi
          csr=BITS(ir, 20, 31);
          imm=rs1=BITS(ir, 15, 19);
        } else {
          t=BITS(ir, 20, 31);
          imm = SGN(t, 11);
        }
        break;
      case 'S': t=BITS(ir, 25, 31)<<5|BITS(ir, 7, 11); imm=SGN(t,11); break;
      case 'B': t=BITS(ir, 31, 31)<<12|BITS(ir,7,7)<<11|BITS(ir,25,30)<<5|BITS(ir,8,11)<<1; imm=SGN(t, 12); break;
      case 'U': imm=(int32_t)(BITS(ir, 12, 31)<<12); break;
      case 'J': t=BITS(ir, 31, 31)<<20|BITS(ir,21,30)<<1|BITS(ir,20,20)<<11|BITS(ir,12,19)<<12; imm=SGN(t,20); break;
      default: printf("Instruction error !"); exit(1);
    }
  } else {
    type = '?';
  }
}
