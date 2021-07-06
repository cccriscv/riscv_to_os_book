#include "nix.h"

ssize_t console_read(void *buf, size_t n) {
  return cdev_read(buf, n, uart_getc);
}

ssize_t console_write(const void *buf, size_t n) {
  return cdev_write(buf, n, uart_putc);
}
