#include "cpu.hpp"

uint32_t pc, ir, op, f3, f7, rd, rs1, rs2, csr, mTop;
int32_t imm;
char type;
const char *name;
uint32_t x[32];
float f[32];
uint8_t m[MMAX];
uint32_t csrs[4096];

Op *decode(uint32_t ir) {
  Op *o = NULL;
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
        case 0: o = &oLb; break;
        case 1: o = &oLh; break;
        case 2: o = &oLw; break;
        case 4: o = &oLbu; break;
        case 5: o = &oLhu; break;
      }
      break;
    case 0x0F:
      switch (f3) {
        case 0: o = &oFence; break;
        case 1: o = &oFenceI; break;
      }
      break;
    case 0x13:
      switch (f3) {
        case 0: o = &oAddi; break;
        case 1: o = &oSlli; break;
        case 2: o = &oSlti; break;
        case 3: o = &oSltiu; break;
        case 4: o = &oXori; break;
        case 5: o = (f7==0)? &oSrli : &oSrai; break;
        case 6: o = &oOri; break;
        case 7: o = &oAndi; break;
      }
      break;
    case 0x17:
      o = &oAuipc;
      break;
    case 0x23:
      switch (f3) {
        case 0: o = &oSb; break;
        case 1: o = &oSh; break;
        case 2: o = &oSw; break;
      }
      break;
    case 0x33:
      switch (f3) {
        case 0: o = (f7==0)? &oAdd : &oSub; break;
        case 1: o = &oSll; break;
        case 2: o = &oSlt; break;
        case 3: o = &oSltu; break;
        case 4: o = &oXor; break;
        case 5: o = (f7==0)? &oSrl : &oSra; break;
        case 6: o = &oOr; break;
        case 7: o = &oAnd; break;
      }
      break;
    case 0x37: // lui
      o = &oLui;
      break;
    case 0x63:
      switch (f3) {
        case 0: o = &oBeq; break;
        case 1: o = &oBne; break;
        case 4: o = &oBlt; break;
        case 5: o = &oBge; break;
        case 6: o = &oBltu; break;
        case 7: o = &oBgeu; break;
      }
      break;
    case 0x67: o = &oJalr; break;
    case 0x6F: o = &oJal; break;
    case 0x73:
      switch (f3) {
        case 0: 
          if (ir == 0x00000073) o = &oEcall;
          if (ir == 0x00100073) o = &oEbreak;
          break;
        case 1: o = &oCsrrw; break;
        case 2: o = &oCsrrs; break;
        case 3: o = &oCsrrc; break;
        case 5: o = &oCsrrwi; break;
        case 6: o = &oCsrrsi; break;
        case 7: o = &oCsrrci; break;
      }
      break;
  }

  if (o) {
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
  return o;
}
