#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdarg.h>
#include <stddef.h>

void _putc(int ch);
int _getc();
int putchar(int ch);
int getchar(void);
int puts(const char *s);
char *gets(char *s);

#endif
