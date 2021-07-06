#include "vfs.h"
#include <io.h>

ssize_t console_read(void *buf, size_t n) {
  return io_read(buf, n, uart_getc);
}

ssize_t console_write(const void *buf, size_t n) {
  return io_write(buf, n, uart_putc);
}

void console_init() {
  devsw[CONSOLE].read = console_read;
  devsw[CONSOLE].write = console_write;
  FILE fin  = { .type=FD_DEVICE, .major=CONSOLE, .readable=1, .writable=0 };
  FILE fout = { .type=FD_DEVICE, .major=CONSOLE, .readable=0, .writable=1 };
  memcpy(&ftable.files[STDIN], &fin, sizeof(FILE));
  memcpy(&ftable.files[STDOUT], &fout, sizeof(FILE));
  memcpy(&ftable.files[STDERR], &fout, sizeof(FILE));
}
