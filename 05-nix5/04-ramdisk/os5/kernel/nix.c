#include <stdio.h>
#include "nix.h"

struct ftable ftable;
struct devsw devsw[NDEV];

int open(const char *pathname, int flags) {
  if (strcmp(pathname, "/ramdisk")==0) return ramdisk_open(flags);
  return -1;
}

ssize_t read(int fd, void *buf, size_t n) {
	struct file *f = &ftable.files[fd];
	return std_read(f, buf, n);
}

ssize_t write(int fd, const void *buf, size_t n) {
	struct file *f = &ftable.files[fd];
	return std_write(f, buf, n);
}

int close(int fd) {
  if (fd == FD_RAMDISK) return ramdisk_close();
  return -1;
}

ssize_t cdev_read(void *buf, size_t n, int (*_getc)(void)) {
  char *cbuf = (char*)buf;
  int i;
  for(i = 0; i < n; i++){
    int ch = _getc();
    cbuf[i] = ch;
    if (ch == EOF) break;
  }
  return i;
}

ssize_t cdev_write(const void *buf, size_t n, int (*_putc)(int)) {
  const char *cbuf = buf;
  int i;
  for(i = 0; i < n; i++){
    int ch = (int) cbuf[i];
    _putc(ch);
  }
  return i;
}

void nix_init(void) {
  console_init();
  ramdisk_init();
	std_init();
}

int nix_start(void) {
	nix_init();
	main(); // 呼叫 C 語言主程式
	while (1) {} // 讓主程式卡在這裡，用無窮迴圈停止！
	return 0;
}
