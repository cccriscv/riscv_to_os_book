#include <unistd.h>

void _putc(int ch) {
  write(1, &ch, 1);
}

int _getc() {
  char ch;
  read(0, &ch, 1);
  return (int) ch;
}
