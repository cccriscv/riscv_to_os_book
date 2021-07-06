#include <stdio.h>
#include <string.h>

int putchar(int ch) {
  char c = (char) ch;
  if (c=='\n' || c=='\r') {
    _putc('\r');
    _putc('\n');
  } else {
    _putc(c);
  }
  return ch;
}

int getchar(void) {
  char ch = _getc();
  putchar(ch);
  return ch;
}

int puts(const char *s) {
  const char *p = s;
	while (*p) putchar(*p++);
  return p-s;
}

char *gets(char *s) {
	char *p = s;
	while (1) {
		char ch = getchar();
		if (ch == '\n' || ch == '\r') break; //
		*p++ = ch;
	}
	*p = '\0';
	return s;
}
