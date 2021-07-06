#include <stdio.h>
#include "nix.h"

struct ftable ftable;
struct devsw devsw[NDEV];

ssize_t read(int fd, void *buf, size_t n) {
	struct file *f = &ftable.files[fd];
	return std_read(f, buf, n);
}

ssize_t write(int fd, const void *buf, size_t n) {
	struct file *f = &ftable.files[fd];
	return std_write(f, buf, n);
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
  devsw[CONSOLE].read = console_read;
  devsw[CONSOLE].write = console_write;
  FILE fin  = { .type=FD_DEVICE, .major=CONSOLE, .readable=1, .writable=0 };
  FILE fout = { .type=FD_DEVICE, .major=CONSOLE, .readable=0, .writable=1 };
  memcpy(&ftable.files[STDIN], &fin, sizeof(FILE));
  memcpy(&ftable.files[STDOUT], &fout, sizeof(FILE));
  memcpy(&ftable.files[STDERR], &fout, sizeof(FILE));
  /*
	ftable.files[STDIN].type = FD_DEVICE;
	ftable.files[STDOUT].type = FD_DEVICE;
	ftable.files[STDERR].type = FD_DEVICE;
	ftable.files[STDIN].major = CONSOLE;
	ftable.files[STDOUT].major = CONSOLE;
	ftable.files[STDERR].major = CONSOLE;
  */
}

int nix_start(void) {
	nix_init();
	std_init();
	main(); // 呼叫 C 語言主程式
	while (1) {} // 讓主程式卡在這裡，用無窮迴圈停止！
	return 0;
}
