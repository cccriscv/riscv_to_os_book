#include <stdio.h>

int puts(const char *s) {
  const char *p = s;
	while (*p) putchar(*p++);
  return p-s;
}
