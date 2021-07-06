#include "dasm.hpp"

void vm(uint8_t *m, uint32_t mTop) {
  char sasm[LMAX];
  pc = 0;
  while (1) {
    ir = bytesToWord(&m[pc]);
    Op *o = ir2asm(ir, sasm);
    exec(o);
    printf("%04X:%08X %-30s x[%d]=%d\n", pc, ir, sasm, rd, x[rd]);
    if (pc >= mTop) break;
  }
}

#ifdef __VM__
// run: ./vm <file.bin>
int main(int argc, char *argv[]) {
  riscv_init();
  char *binFileName = argv[1];
  FILE *binFile = fopen(binFileName, "rb");
  mTop = fread(m, sizeof(uint8_t), MMAX, binFile);
  fclose(binFile);
  vm(m, mTop);
  return 0;
}
#endif