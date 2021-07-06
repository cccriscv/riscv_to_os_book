#include <stdio.h>

int _putc(char ch) {
  putchar(ch);
}

int puts(const char *s) {
  const char *p = s;
	while (*p) _putc(*p++);
  return p-s;
}

int main() {
  puts("Hello!\n");
}
