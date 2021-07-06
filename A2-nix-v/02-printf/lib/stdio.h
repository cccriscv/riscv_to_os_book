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
int vsprintf(char *text, const char *fmt, va_list ap);
int sprintf(char *text, const char *fmt, ...);
int printf(const char *fmt, ...);
int sscanf(const char *text, const char *fmt, ...);

#endif
