#include "asm.hpp"
#include "parser.hpp"

map<string, int> symMap {};

void pass1(char *inFile) {
  printf("============= PASS1 ================\n");
  char line[100]="";
  FILE *fp = fopen(inFile, "r");
  pc = 0;
  while (fgets(line, sizeof(line), fp)) {
    int size = parse(line);
    if (*label) {
      printf("symMap:add %s=%d\n", label, pc);
      string sym(label);
      symMap[sym] = pc;
    }
    pc += size;
  }
  fclose(fp);
}

uint32_t format() {
  switch (type) {
    case 'R':
      return (f7<<25) | (rs2<<20) | (rs1<<15) | (f3<<12) | (rd<<7) | (op);
    case 'I':
      if (op == 0x73) { 
        if (f3 <= 3) { // ex:csrrw rd, csr, rs1;
          return (csr<<20) | (rs1<<15) | (f3<<12) | (rd<<7) | (op);
        } else { // ex:csrrwi rd, csr, uimm4
          return (csr<<20) | (BITS(imm, 0, 4)<<15) | (f3<<12) | (rd<<7) | (op);
        }
      } else
        return (imm<<20) | (rs1<<15) | (f3<<12) | (rd<<7) | (op);
    case 'S':
      return (BITS(imm, 5, 11)<<25) | (rs2<<20) | (rs1<<15) | (f3<<12) | (BITS(imm, 0, 4)<<7) | (op);
    case 'B':
      return (BITS(imm, 12, 12)<<31) | (BITS(imm, 5, 10)<<25) | (rs2<<20) | (rs1<<15) | (f3<<12) | (BITS(imm, 1, 4)<<8) | (BITS(imm, 11, 11)<<7) | (op);
    case 'U':
      return (BITS(imm, 12, 31)<<12) | (rd<<7) | (op);
    case 'J':
      return (BITS(imm, 20, 20)<<31) | (BITS(imm, 1, 10)<<21) | (BITS(imm, 11, 11)<<20) | (BITS(imm, 12,19)<<12) | (rd<<7) | (op);
    default:
      printf("format: error! type=%c\n", type);
      return 0;
  }
}

int32_t arg2i(char *arg) {
  int32_t n;
  if (strchr("0123456789-", arg[0])!=NULL) {
    if (strncmp(arg, "0x", 2)==0) {
      sscanf(&arg[2], "%X", &n);
      return n;
    }
    return atoi(arg);
  } else {
    return mapFind(&symMap, arg);
  }
}

uint32_t gen() {
  Op *o = opFind(p0);
  ASSERT(o, "Error: gen() command %s not found!", p0);;
  type=o->type; op=o->op; f3=o->f3; f7=o->f7; rd = 0; rs1 = 0; rs2 = 0; imm = -1;
  switch (type) {
    case 'R': // ex: add rd, rs1, rs2
      rd=rFind(p1); rs1=rFind(p2); rs2=rFind(p3);
      break;
    case 'I':
      if (op == 0x73) { 
        if (f3 <= 3) { // ex:csrrw rd, csr, rs1;
          rd=rFind(p1); csr = csrFind(p2); rs1=rFind(p3);
        } else { // ex:csrrwi rd, csr, uimm4
          rd=rFind(p1); csr = csrFind(p2); imm=arg2i(p3);
        }
      } else if (isalpha(p2[0])) { // ex: addi rd, rs1, imm; 
        rd=rFind(p1); rs1=rFind(p2); imm=arg2i(p3);
      } else { // ex: lw	a5,-40(s0)
        rd=rFind(p1); imm=arg2i(p2); rs1=rFind(p3);
      }
      break;
    case 'S': // ex: SB rs2, imm(rs1)
      rs2=rFind(p1); imm=arg2i(p2); rs1=rFind(p3);
      break;
    case 'B': // ex: BEQ rs1, rs2, imm
      rs1=rFind(p1); rs2=rFind(p2); imm=arg2i(p3);
      if (!strchr("0123456789-", p3[0])) imm -= pc; // label:使用相對於 pc 的定址法。
      break;
    case 'U': // ex: LUI rd, imm; AUIPC rd, imm
      rd=rFind(p1); imm=arg2i(p2);
      break;
    case 'J': // ex: JAL imm(rd)
      imm=arg2i(p1); rd=rFind(p2); 
      break;
    default:
      printf("format: error! type=%c\n", o->type);
  }
  uint32_t i32 = format();
  return i32;
}

int asm2bytes(char *line, uint8_t *bytes) {
  int size = parse(line);
  // printf("asm2bytes:cmdType=%c\n", cmdType);
  if (cmdType == 'I') {
    uint32_t ir0 = gen();
    word2bytes(ir0, bytes, LittleEndian);
  } else if (cmdType == 'S') { // ex: helloworld: .ascii "Hello World!\n"
    strcpy((char*)bytes, p1);
  }
  return size;
}

void pass2(char* inFile, char* binFile) {
  printf("============= PASS2 ================\n");
  char line[SMAX], binary[17];
  FILE *fp = fopen(inFile, "r"); // 開啟組合語言檔
  FILE *bfp = fopen(binFile, "wb"); // 開啟輸出的 .bin 二進位檔
  pc = 0;
  while (fgets(line, sizeof(line), fp)) { // 一行一行讀
    uint8_t bytes[SMAX];
    char hex[SMAX];
    int size = asm2bytes(line, bytes);
    bytes2hex(bytes, size, hex);
    if (size > 0) printf("%04X:%-20s %c %02X %01X %02X r:%02d %02d %02d imm:%d\n", pc, hex, type, op, f3, f7, rd, rs1, rs2, imm);
    fwrite(bytes, 1, size, bfp); // 輸出 .bin 的二進位檔
    pc += size;
  }
  fclose(fp);
  fclose(bfp);
}

void assemble(char *file) {
  char inFile[100], hackFile[100], binFile[100];
  sprintf(inFile, "%s.s", file);
  sprintf(binFile, "%s.bin", file);
  mapDump(&symMap);
  pass1(inFile);
  mapDump(&symMap);
  pass2(inFile, binFile);
}

#ifdef __ASM__
// run: ./asm <file>
// notice : <file> with no extension.
int main(int argc, char *argv[]) {
  riscv_init();
  assemble(argv[1]);
}
#endif