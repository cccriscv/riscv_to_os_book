#include <unistd.h>
#include <unix.h>

void _fputc(int fd, int ch) {
  write(fd, &ch, 1); 
}

int _fgetc(int fd) {
  char ch;
  read(fd, &ch, 1);
  return (int) ch;
}

void _putc(int ch) {
  _fputc(STDOUT, ch);
}

int _getc() {
  _fgetc(STDIN);
}

