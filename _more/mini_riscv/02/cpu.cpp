#include "cpu.hpp"

uint32_t pc, ir, op, f3, f7, rd, rs1, rs2, mTop;
int32_t imm;
char type;
const char *name;
uint32_t x[32];
float f[32];
uint8_t m[MMAX];

Op *decode(uint32_t ir) {
  Op *o = NULL;
  op=BITS(ir, 0, 6); 
  f3=BITS(ir, 12, 14);
  f7=BITS(ir, 25, 31);
  rd=BITS(ir, 7, 11);
  rs1=BITS(ir, 15, 19);
  rs2=BITS(ir, 20, 24);
  imm=0;
  switch (op) {
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
    case 0x23:
      switch (f3) {
        case 0: o = &oSb; break;
        case 1: o = &oSh; break;
        case 2: o = &oSw; break;
      }
      break;
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
    case 0x67:
      o = &oJalr; break;
    case 0x6F:
      o = &oJal; break;
    case 0x74:
      switch (f3) {
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
      case 'I': t=BITS(ir, 20, 31); imm = SGN(t, 11); break;
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
  else if (o==&oLb) x[rd]=*(int8_t*)&m[x[rs1]+imm];
  else if (o==&oLbu) x[rd]=*(uint8_t*)&m[x[rs1]+imm];
  else if (o==&oLh) x[rd]=*(int16_t*)&m[x[rs1]+imm];
  else if (o==&oLhu) x[rd]=*(uint16_t*)&m[x[rs1]+imm];
  else if (o==&oLw) x[rd]=*(int32_t*)&m[x[rs1]+imm];
  else if (o==&oSb) *(int8_t*)&m[x[rs1]+imm]=x[rd];
  else if (o==&oSh) *(int16_t*)&m[x[rs1]+imm]=x[rd];
  else if (o==&oSw) *(int32_t*)&m[x[rs1]+imm]=x[rd];
  else if (o==&oLui) x[rd]=imm<<12;
  else if (o==&oAuipc) x[rd]=pc+(imm<<12);
  else {
    isJump = 1;
    if (o==&oJal) { x[rd]=pc; pc+= imm; }
    else if (o==&oJalr) { int32_t t=pc+4; pc=x[rs1]+imm; x[rd] = t; } // 保存返回位址然後跳到 x[rs1]+imm
    else if (o==&oBeq) { if (x[rs1] == x[rs2]) pc+=imm; else pc+=4; } // 疑問：跳躍位址應該是 pc加上 sign-extended 13-bit，而非直接設定。
    else if (o==&oBlt) { if (x[rs1] <  x[rs2]) pc+=imm; else pc+=4; }
    else if (o==&oBne) { if (x[rs1] != x[rs2]) pc+=imm; else pc+=4; }
    else if (o==&oBge) { if (x[rs1] >= x[rs2]) pc+=imm; else pc+=4; }
    else if (o==&oBltu) { if ((uint32_t)x[rs1] <  (uint32_t)x[rs2]) pc += imm; else pc+=4; }
    else if (o==&oBgeu) { if ((uint32_t)x[rs1] >= (uint32_t)x[rs2]) pc += imm; else pc+=4; }
    else { printf("ir=%08x : command not found!\n", ir); exit(1); }
  }
  if (!isJump) pc += 4;
}
