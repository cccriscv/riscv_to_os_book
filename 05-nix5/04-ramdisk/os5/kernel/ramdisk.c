#include "nix.h"

#define RAMSIZE 8192
#define pos ramdisk->offset

char ram[RAMSIZE];
int size = 0;

int ramdisk_open(int flags) {
  if (flags & O_CREAT & O_TRUNC) size = 0;
  pos = 0;
  return FD_RAMDISK;
}

ssize_t ramdisk_read(void *buf, size_t n) {
  int len = MIN(RAMSIZE - pos, n);
  memcpy(buf, (void*)(&ram[pos]), len);
  pos += len;
  return len;
}

ssize_t ramdisk_write(const void *buf, size_t n) {
  int len = MIN(RAMSIZE - pos, n);
  memcpy(&ram[pos], buf, len);
  pos += len;
  if (pos > size) size=pos;
  return len;
}

int ramdisk_close() {
  pos = -1;
  return 0;
}

void ramdisk_init() {
  FILE framdisk = { .type=FD_DEVICE, .major=RAMDISK, .readable=1, .writable=1 };
  devsw[RAMDISK].read = ramdisk_read;
  devsw[RAMDISK].write = ramdisk_write;
  memcpy(&ftable.files[FD_RAMDISK], &framdisk, sizeof(FILE));
}
