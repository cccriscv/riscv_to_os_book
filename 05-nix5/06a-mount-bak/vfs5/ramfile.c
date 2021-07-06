#include "vfs.h"
#include <lib.h>

#define RAMSIZE 8192
#define pos ramfile->offset

char ram[RAMSIZE];
int size = 0;

int ramfile_open(int flags) {
  if (flags & O_TRUNC) size = 0;
  pos = 0;
  return STDRAMF;
}

ssize_t ramfile_read(void *buf, size_t n) {
  int len = MIN(RAMSIZE - pos, n);
  memcpy(buf, (void*)(&ram[pos]), len);
  pos += len;
  return len;
}

ssize_t ramfile_write(const void *buf, size_t n) {
  int len = MIN(RAMSIZE - pos, n);
  memcpy(&ram[pos], buf, len);
  pos += len;
  if (pos > size) size=pos;
  return len;
}

int ramfile_close() {
  pos = -1;
  return 0;
}

void ramfile_init() {
  FILE f = { .type=FD_DEVICE, .major=RAMFILE, .readable=1, .writable=1 };
  devsw[RAMFILE].read = ramfile_read;
  devsw[RAMFILE].write = ramfile_write;
  memcpy(&ftable.files[STDRAMF], &f, sizeof(FILE));
}
