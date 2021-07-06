#include <stdio.h>

int puts(const char *s) {
  const char *p = s;
	while (*p) putchar(*p++);
  return p-s;
}

char *gets(char *s) {
	char *p = s;
	while (1) {
		char ch = getchar();
		putchar(ch);
		if (ch == '\n' || ch == '\r') break;
		*p++ = ch;
	}
	*p = '\0';
	return s;
}