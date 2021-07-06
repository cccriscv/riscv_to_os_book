#include "dasm.hpp"

Op* ir2asm(uint32_t ir, char *line) {
  Op *o = decode(ir);
  switch (type) {
    case 'R': sprintf(line, "%s x%d,x%d,x%d", name, rd, rs1, rs2); break;
    case 'I': sprintf(line, "%s x%d,x%d,%d", name, rd, rs1, imm); break;
    case 'S': sprintf(line, "%s x%d,%d(x%d)", name, rs1, imm, rs2); break;
    case 'B': sprintf(line, "%s x%d,x%d,%d", name, rs1, rs2, imm); break;
    case 'U': sprintf(line, "%s x%d,%d", name, rd, imm); break;
    case 'J': sprintf(line, "%s %d(x%d)", name, imm, rd); break;
    default: sprintf(line, "Instruction error !");
  }
  return o;
}

#ifdef __DASM__
// run: ./dasm <file.bin>
int main(int argc, char *argv[]) {
  char sasm[LMAX];
  char *binFileName = argv[1];
  FILE *binFile = fopen(binFileName, "rb");
  mTop = fread(m, sizeof(uint8_t), MMAX, binFile);
  fclose(binFile);
  for (pc = 0; pc<mTop; pc+=4) {
    ir = bytesToWord(&m[pc]);
    ir2asm(ir, sasm);
    printf("%04X:%08X %s\n", pc, ir, sasm);
  }
  return 0;
}
#endif
