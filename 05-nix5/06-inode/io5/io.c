#include "io.h"
#include <stdio.h>

ssize_t io_read(void *buf, size_t n, int (*_getc)(void)) {
  char *cbuf = (char*)buf;
  int i;
  for(i = 0; i < n; i++){
    int ch = _getc();
    cbuf[i] = ch;
    if (ch == EOF) break;
  }
  return i;
}

ssize_t io_write(const void *buf, size_t n, int (*_putc)(int)) {
  const char *cbuf = buf;
  int i;
  for(i = 0; i < n; i++){
    int ch = (int) cbuf[i];
    _putc(ch);
  }
  return i;
}
