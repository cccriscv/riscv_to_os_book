#include "riscv.hpp"

Op oNull = { "", '?', 0, 0, 0 };
Op oSll = {"sll", 'R',0x33,0x1,0x0};
Op oSlli = {"slli", 'I',0x13,0x1,0x0};
Op oSrl = {"srl", 'R',0x33,0x5,0x0};
Op oSrli = {"srli", 'I',0x13,0x5,0x0};
Op oSra = {"sra", 'R',0x33,0x5,0x20};
Op oSrai = {"srai", 'I',0x13,0x5,0x20};
Op oAdd = {"add", 'R',0x33,0x0,0x0};
Op oAddi = {"addi", 'I',0x13,0x0,0x0};
Op oSub = {"sub", 'R',0x33,0x0,0x20};
Op oLui = {"lui", 'U',0x37};
Op oAuipc = {"auipc", 'U',0x17};
Op oXor = {"xor", 'R',0x33,0x4};
Op oXori = {"xori", 'I',0x13,0x4};
Op oOr = {"or", 'R',0x33,0x6};
Op oOri = {"ori", 'I',0x13,0x6};
Op oAnd = {"and", 'R',0x33,0x7};
Op oAndi = {"andi", 'I',0x13,0x7};
Op oSlt = {"slt", 'R',0x33,0x2};
Op oSlti = {"slti", 'I',0x13,0x2};
Op oSltu = {"sltu", 'R',0x33,0x3};
Op oSltiu = {"sltiu", 'I',0x13,0x3};
Op oBeq = {"beq", 'B',0x63,0x0};
Op oBne = {"bne", 'B',0x63,0x1};
Op oBlt = {"blt", 'B',0x63,0x4};
Op oBge = {"bge", 'B',0x63,0x5};
Op oBltu = {"bltu", 'B',0x63,0x6};
Op oBgeu = {"bgeu", 'B',0x63,0x7};
Op oJal = {"jal", 'J',0x6F};
Op oJalr = {"jalr", 'I',0x67};
Op oFence = {"fence", 'I',0x0F,0x0};
Op oFenceI = {"fence.i", 'I',0x0F,0x1};
Op oEcall = {"ecall", 'I',0x73};
Op oEbreak = {"ebreak", 'I',0x73};
Op oCsrrw = {"csrrw", 'I',0x73,0x1};
Op oCsrrs = {"csrrs", 'I',0x73,0x2};
Op oCsrrc = {"csrrc", 'I',0x73,0x3};
Op oCsrrwi = {"csrrwi", 'I',0x73,0x5}; // 格式 I 是把 rs1 改為 uimm
Op oCsrrsi = {"csrrsi", 'I',0x73,0x6};
Op oCsrrci = {"csrrci", 'I',0x73,0x7};
Op oLb = {"lb", 'I',0x03,0x0};
Op oLh = {"lh", 'I',0x03,0x1};
Op oLw = {"lw", 'I',0x03,0x2};
Op oLbu = {"lbu", 'I',0x03,0x4};
Op oLhu = {"lhu", 'I',0x03,0x5};
Op oSb = {"sb", 'S',0x23,0x0};
Op oSh = {"sh", 'S',0x23,0x1};
Op oSw = {"sw", 'S',0x23,0x2};
/*
Op oFence = {"fence", 'I',0x0F,0x0};
Op oFenceI = {"fence.i", 'I',0x0F,0x1};
Op oEcall = {"ecall", 'I',0x73};
Op oEbreak = {"ebreak", 'I',0x73};
Op oCsrrw = {"csrrw", 'I',0x73,0x1};
Op oCsrrs = {"csrrs", 'I',0x73,0x2};
Op oCsrrc = {"csrrc", 'I',0x73,0x3};
Op oCsrrwi = {"csrrwi", 'I',0x73,0x5}; // 格式 I 是把 rs1 改為 uimm
Op oCsrrsi = {"csrrsi", 'I',0x73,0x6};
Op oCsrrci = {"csrrci", 'I',0x73,0x7};
*/

map<string, Op> opMap {
  {"null", oNull}, 
  {"sll", oSll}, 
  {"slli",oSlli}, 
  {"srl", oSrl}, 
  {"srli",oSrli}, 
  {"sra",oSra}, 
  {"srai",oSrai}, 
  {"add", oAdd}, 
  {"addi",oAddi}, 
  {"sub", oSub}, 
  {"lui", oLui}, 
  {"auipc", oAuipc}, 
  {"xor", oXor}, 
  {"xori", oXori}, 
  {"or",  oOr}, 
  {"ori", oOri},
  {"and", oAnd},
  {"andi", oAndi},
  {"slt", oSlt},
  {"slti", oSlti},
  {"sltu", oSltu},
  {"sltiu", oSltiu},
  {"beq", oBeq},
  {"bne", oBne},
  {"blt", oBlt},
  {"bge", oBge},
  {"bltu", oBltu},
  {"bgeu", oBgeu},
  {"jal",  oJal},
  {"jalr", oJalr},
  {"fence", oFence},
  {"fence.i", oFenceI},
  {"ecall", oEcall},
  {"ebreak", oEbreak},
  {"csrrw", oCsrrw},
  {"csrrs", oCsrrs},
  {"csrrc", oCsrrc},
  {"csrrwi", oCsrrwi},
  {"csrrsi", oCsrrsi},
  {"csrrci", oCsrrci},
  {"lb", oLb},
  {"lh", oLh},
  {"lw", oLw},
  {"lbu", oLbu},
  {"lhu", oLhu},
  {"sb", oSb},
  {"sh", oSh},
  {"sw", oSw},
};

map<string, int> rMap {
  // 暫存器 x0-x31
  {"x0",0},{"x1",1},{"x2",2},{"x3",3},{"x4",4},{"x5",5},{"x6",6},{"x7",7},
  {"x8",8}, {"x9",9}, {"x10",10}, {"x11",11},{"x12",12}, {"x13",13}, {"x14",14}, {"x15",15},
  {"x16",16}, {"x17",17}, {"x18",18}, {"x19",19},{"x20",20}, {"x21",21}, {"x22",22}, {"x23",23},
  {"x24",24}, {"x25",25}, {"x26",26}, {"x27",27},{"x28",28}, {"x29",29}, {"x30",30}, {"x31",31},
  // 暫存器別名
  {"zero",0},{"ra",1},{"sp",2},{"gp",3},{"tp",4},{"t0",5},{"t1",6},{"t2",7},
  {"s0",8}, {"fp",8}, {"s1",9}, {"a0",10}, {"a1",11},{"a2",12}, {"a3",13}, {"a4",14}, {"a5",15},
  {"a6",16}, {"a7",17}, {"s2",18}, {"s3",19},{"s4",20}, {"s5",21}, {"s6",22}, {"s7",23},
  {"s8",24}, {"s9",25}, {"s10",26}, {"s11",27},{"t3",28}, {"t4",29}, {"t5",30}, {"t6",31}
};

map<string, int> csrMap {
  // 特權暫存器
  {"ustatus",0x0},{"uie",0x4},{"utvec",0x5},{"uscratch",0x40},{"uepc",0x41},{"ucause",0x42},{"ubadaddr",0x43},
  {"uip",0x44},{"fflag",0x1},{"frm",0x2},{"fcsr",0x3},
  {"cycle",0xC00},{"time",0xC01},{"instret",0xC02},
  {"hpmcounter3",0xC03},{"hpmcounter4",0xC04},/* ...*/ {"hpmcounter31",0xC1F},
  {"cycleh",0xC80},{"timeh",0xC81},{"instreth",0xC82},
  {"hpmcounter3h",0xC83},{"hpmcounter4h",0xC84},/*...*/{"hpmcounter31h",0xC9F},
  {"sstatus",0x100},{"sedeleg",0x102},{"sideleg",0x103},{"sie",0x104},{"stvec",0x105},
  {"sscratch",0x140},{"sepc",0x141},{"scause",0x142},{"sbadaddr",0x143},{"sip",0x144},{"sptbr",0x180},
  {"hstatus",0x200},{"hedeleg",0x202},{"hideleg",0x203},{"hie",0x204},{"htvec",0x205},
  {"hscratch",0x240},{"hepc",0x241},{"hcause",0x242},{"hbadaddr",0x243},{"hip",0x244},
  {"mvendorid",0xF11},{"marchid",0xF12},{"mimpid",0xF13},{"mhartid",0xF14},
  {"mstatus",0x300},{"misa",0x301},{"medeleg",0x302},{"mideleg",0x303},{"mie",0x304},{"mtvec",0x305},
  {"mscratch",0x340},{"mepc",0x341},{"mcause",0x342},{"mbadaddr",0x343},{"mip",0x344},
  {"mbase",0x380},{"mbound",0x381},{"mibase",0x382},{"mibound",0x383},{"mdbase",0x384},{"mdbound",0x385},
  {"mcycle",0xB00},{"minstret",0xB02},{"mhpmcounter3",0xB03},{"mhpmcounter4",0xB04},/*...*/{"mhpmcounter31",0xB1F},
  {"mcycleh",0xB80},{"minstreth",0xB82},{"mhpmcounter3h",0xB83},{"mhpmcounter4h",0xB84},/*...*/{"mhpmcounter31h",0xB9F},
  {"mucounteren",0x320},{"mscounteren",0x321},{"mhcounteren",0x322},{"mhpmevent3",0x323},{"mhpmevent4",0x324},/*...*/{"mhpmevent31",0x33F},
  {"tselect",0x7A0},{"tdata1",0x7A1},{"tdata2",0x7A2},{"tdata3",0x7A3},
  {"dcsr",0x7B0},{"dpc",0x7B1},{"dscratch",0x7B2},
};

int mapFind(map<string, int> *iMap, char *name) {
  string name0(name);
  map<string, int>::const_iterator pos = iMap->find(name0);
  if (pos == iMap->end()) return -1;
  return pos->second;
}

void mapDump(map<string, int> *map) {
  printf("=================symMap===================\n");
  for(auto it = map->cbegin(); it != map->cend(); ++it) {
      cout << it->first << " " << it->second << "\n";
  }
}

Op opFind(char *name) {
  string op0(name);
  map<string, Op>::const_iterator pos = opMap.find(op0);
  if (pos == opMap.end()) return oNull;
  return pos->second;
}

int rFind(char *name) { return mapFind(&rMap, name); }

// Convert i32 to Little Endian
void wordToBytes(uint32_t word, uint8_t bytes[]) {
  bytes[0] = BITS(word, 0, 7);
  bytes[1] = BITS(word, 8, 15);
  bytes[2] = BITS(word, 16, 23);
  bytes[3] = BITS(word, 24, 31);
}

// Convert Little Endian to i32
uint32_t bytesToWord(uint8_t bytes[]) {
  return (uint32_t)bytes[0] | (uint32_t)bytes[1] << 8 | (uint32_t)bytes[2] << 16 | (uint32_t)bytes[3] << 24;
}
