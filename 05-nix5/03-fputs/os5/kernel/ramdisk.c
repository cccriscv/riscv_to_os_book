#include "nix.h"

#define NRAMDISK 8192

char ramdisk[NRAMDISK];
size_t pos;

void ramdisk_open() {
  pos = 0;
}

ssize_t ramdisk_read(void *buf, size_t n) {
  ssize_t len = bdev_read(&ramdisk[pos], buf, n);
  pos += len;
  return len;
}

ssize_t ramdisk_write(const void *buf, size_t n) {
  ssize_t len = bdev_write(&ramdisk[pos], buf, n);
  pos += len;
  return len;
}

void ramdisk_close() {
  pos = -1;
}
