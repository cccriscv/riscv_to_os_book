#include "asm.hpp"

map<string, int> symMap {};

int symFind(char *name) { return mapFind(&symMap, name); }

char p0[SMAX], p1[SMAX], p2[SMAX], p3[SMAX];

char parse(char *pline) {
  printf("// %s", pline);
  char line[SMAX], *s = line;
  strcpy(line, pline);
  while(*s != '\0') {
    if (strchr(",()\t\n", *s)!=NULL) *s=' ';
    if (*s == '#') break;
    s++;
  }
  *s = '\0';
  p0[0]='\0'; p1[0]='\0'; p2[0]='\0'; p3[0]='\0';
  sscanf(line, "%s", p0);
  if (strchr(p0, ':') > 0) return ':';
  if (p0[0] == '.') return '.';
  Op o = opFind(p0);
  sscanf(line, "%s %s %s %s", p0, p1, p2, p3);
  return o.type;
}

void pass1(char *inFile) {
  printf("============= PASS1 ================\n");
  char line[100]="";
  FILE *fp = fopen(inFile, "r");
  pc = 0;
  while (fgets(line, sizeof(line), fp)) {
    char type = parse(line);
    if (type==':') {
      char label[SMAX];
      sscanf(p0, "%[^:]", label);
      printf("label=%s\n", label);
      string slabel(label);
      symMap[slabel] = pc;
    } else if (strchr("RISBUJ", type)!=NULL) {
      pc += 4;
    }
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
    return symFind(arg);
  }
}

uint32_t gen() {
  Op o = opFind(p0);
  type=o.type; op=o.op; f3=o.f3; f7=o.f7; rd = 0; rs1 = 0; rs2 = 0; imm = -1;
  switch (o.type) {
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
      rs1=rFind(p1); rs2=rFind(p2); imm=arg2i(p3)-pc;
      break;
    case 'U': // ex: LUI rd, imm; AUIPC rd, imm
      rd=rFind(p1); imm=arg2i(p2);
      break;
    case 'J': // ex: JAL imm(rd)
      imm=arg2i(p1); rd=rFind(p2); 
      break;
    default:
      printf("format: error! type=%c\n", o.type);
  }
  uint32_t i32 = format();
  return i32;
}

uint32_t asm2ir(char *line) {
  char type = parse(line);
  if (strchr("RISBUJ", type)!=NULL) {
    return gen();
  }
  return 0;
}

void pass2(char* inFile, char* binFile) {
  printf("============= PASS2 ================\n");
  char line[SMAX], binary[17];
  FILE *fp = fopen(inFile, "r"); // 開啟組合語言檔
  FILE *bfp = fopen(binFile, "wb"); // 開啟輸出的 .bin 二進位檔
  pc = 0;
  while (fgets(line, sizeof(line), fp)) { // 一行一行讀
    uint32_t ir = asm2ir(line);
    if (ir != 0) {
      uint8_t bytes[4];
      printf("%04X:%08X %c %02X %01X %02X r:%02d %02d %02d imm:%d\n", pc, ir, type, op, f3, f7, rd, rs1, rs2, imm);
      wordToBytes(ir, bytes);
      fwrite(bytes, 1, sizeof(bytes), bfp); // 輸出 .bin 的二進位檔
      pc += 4;
    }
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
  assemble(argv[1]);
}
#endif