#define BLOCK(i) (BSIZE*(i))

void binit(void) {
  
}

struct buf* bread(uint dev, uint blockno) {
  memcpy()
  // disk_rw(b, DISK_READ);
  return b;
}

void bwrite(struct buf *b) {
  // disk_rw(b, DISK_WRITE);
}
