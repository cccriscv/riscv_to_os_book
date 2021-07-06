#define NRAM 1024*1024
char ram[NRAM];
int  ram_size;

#define BLOCK(i) &ram[BSIZE*(i)]

void init(char *imgFile) {
  FILE *f = fopen(imgFile, "rb");
  ram_size = fread(ram, NRAM, 1, f);
  fclose(f);
}

struct buf* bread(uint dev, uint blockno) {
  struct buf b;
  b.data = BLOCK(blockno);
  return b;
}

void bwrite(struct buf *b) {
  memcpy(BLOCK(blockno), b->data, BSIZE);
}
