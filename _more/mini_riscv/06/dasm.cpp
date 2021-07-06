#include "dasm.hpp"

void ir2asm(uint32_t ir, char *line) {
  decode(ir);
  switch (type) {
    case 'R': sprintf(line, "%s x%d,x%d,x%d", name, rd, rs1, rs2); break;
    case 'I':
      if (op==0x73) {
        if (f3 <= 3) {
          sprintf(line, "%s x%d,%s,x%d", name, rd, csrName(csr), rs1);
        } else {
          sprintf(line, "%s x%d,%s,%d", name, rd, csrName(csr), imm);
        }
      } else {
        sprintf(line, "%s x%d,x%d,%d", name, rd, rs1, imm);
      }
      break;
    case 'S': sprintf(line, "%s x%d,%d(x%d)", name, rs1, imm, rs2); break;
    case 'B': sprintf(line, "%s x%d,x%d,%d", name, rs1, rs2, imm); break;
    case 'U': sprintf(line, "%s x%d,%d", name, rd, imm); break;
    case 'J': sprintf(line, "%s %d(x%d)", name, imm, rd); break;
    default: sprintf(line, "Instruction error !");
  }
}

#ifdef __DASM__
// run: ./dasm <file.bin>
int main(int argc, char *argv[]) {
  riscv_init();
  char sasm[LMAX];
  char *binFileName = argv[1];
  FILE *binFile = fopen(binFileName, "rb");
  mTop = fread(m, sizeof(uint8_t), MMAX, binFile);
  fclose(binFile);
  for (pc = 0; pc<mTop; pc+=4) {
    ir = bytes2word(&m[pc], LittleEndian);
    ir2asm(ir, sasm);
    printf("%04X:%08X %s\n", pc, ir, sasm);
  }
  return 0;
}
#endif
