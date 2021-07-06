#include "lib.hpp"

int mapFind(map<string, int> *m, char *name) {
  string name0(name);
  map<string, int>::const_iterator pos = m->find(name0);
  if (pos == m->end()) return -1;
  return pos->second;
}

void mapDump(map<string, int> *m) {
  printf("=================symMap===================\n");
  for(auto it = m->cbegin(); it != m->cend(); ++it) {
    cout << it->first << " " << it->second << "\n";
  }
}

void mapReverse(map<string, int> *m, map<int, string> *rm) {
  for(auto it = m->cbegin(); it != m->cend(); ++it) {
    (*rm)[it->second] = it->first;
  }
}

// Convert i32 to Little Endian
void word2bytes(uint32_t word, uint8_t bytes[], enum Endian endian) {
  ASSERT(endian==LittleEndian, "word2bytes(): only support Little Endian");
  bytes[0] = BITS(word, 0, 7);
  bytes[1] = BITS(word, 8, 15);
  bytes[2] = BITS(word, 16, 23);
  bytes[3] = BITS(word, 24, 31);
}

// Convert Little Endian to i32
uint32_t bytes2word(uint8_t bytes[], enum Endian endian) {
  ASSERT(endian==LittleEndian, "bytes2word(): only support Little Endian");
  return (uint32_t)bytes[0] | (uint32_t)bytes[1] << 8 | (uint32_t)bytes[2] << 16 | (uint32_t)bytes[3] << 24;
}

void bytes2hex(uint8_t *bytes, int size, char *hex) {
  char *hp = hex;
  for (int i=0; i<size; i++) {
    sprintf(hp, "%02X", bytes[i]);
    hp += 2;
  }
  *hp = '\0';
}
